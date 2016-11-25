#include <iostream>
#include "tcpoudp.h"

#define max_segment_size 1024
#define window_size

using namespace std;

typedef unsigned char byte;

/** assumed functions **/
// checks if timer is already running
bool is_timer_running();
// starts timer and sets timeout to true when timer reaches mseconds value
void start_timer(bool & timeout, int mseconds);
void stop_timer();
int udpsend(byte * msg, int dest_port, int dest_ip);
// non blocking udp receive
byte * udprcv(int port, int ip);
// interface from transport to app layer
void deliver_to_applayer(byte * data);
/** **************************************************************** **/

int get_seq(byte * pkt);
int get_ack(byte * pkt);
int get_length(byte * msg);
byte * extract(byte * pkt);
byte * make_pkt(byte * msg, int seq_num, int ack_num, int size, int sendbase = 0);

void TCPOUDPSend(byte * msg, int dest_port, int dest_ip);
byte * TCPOUDPReceive(int port, int ip);

// global variables to be modified by send and receive functions
int NextSeqNum = 0;
int NextAckNum = 0;

void main()
{
    // assume connection is established

    int servername = "localhost";
    int port = 12000;
    byte message[] = "Send This Message";
    byte * reply;
    TCPOUDPSend(message, port, servername);
    reply = TCPOUDPReceive(port, servername);
}

// extracts the seq number from the data in the packet (first 4 bytes)
int get_seq(byte *pkt)
{
    return ((byte)pkt[0] << 24) + ((byte)pkt[1] << 16) + ((byte)pkt[2] << 8) + ((byte)pkt[3]);
}

// extracts the ack number from the data in the packet (second 4 bytes)
int get_ack(byte *pkt)
{
    return ((byte)pkt[4] << 24) + ((byte)pkt[5] << 16) + ((byte)pkt[6] << 8) + ((byte)pkt[7]);
}

// returns the size of the byte array
int get_length(byte * msg)
{
    return sizeof(msg);
}

// remove the 8 bytes containing sequence number and ack number
byte * extract(byte *pkt)
{
    byte * msg = new byte(get_length(pkt) - 8);
    for(int i = 0; i < get_length(pkt); i++)
    {
        msg[i] = pkt[i+8];
    }
    return msg;
}

// creates a new byte array which contains 4 bytes seq_num and 4 bytes ack_num in the beginning
// this function also takes a slice of the message starting from sendbase with a size equal to s
// this is to send the message part by part if it is too large to be sent as a one packet
// the message slice is added after the 8 bytes of seq_num and ack_num
byte * make_pkt(byte * msg, int seq_num, int ack_num, int s, int sendbase)
{
    byte * pkt = new byte(s + 8);
    // add sequence and ack numbers to message
    // (take first byte of the int)
    pkt[3] = (byte) seq_num;
    // shift right 8 bits and convert to byte (take second byte of the int)
    pkt[2] = (byte) seq_num >> 8;
    // shift right 8 bits and convert to byte (take third byte of the int)
    pkt[1] = (byte) seq_num >> 16;
    // shift right 8 bits and convert to byte (take fourth byte of the int)
    pkt[0] = (byte) seq_num >> 24;
    pkt[7] = (byte) ack_num;
    pkt[6] = (byte) ack_num >> 8;
    pkt[5] = (byte) ack_num >> 16;
    pkt[4] = (byte) ack_num >> 24;
    for(int i = sendbase, j = 0; j < s; i++, j++)
    {
        pkt[j+8] = msg[i]; // copy message to the packet
    }
    return pkt;
}

// checksum is implemented in the udpsend function as it's a part of the udp headers
// seq_num and ack_num are sent as a part of the data through udp (under udp headers)
// so it is not needed to calculate checksum in this function
void TCPOUDPSend(byte * msg, int dest_port, int dest_ip)
{
    int unacked_sendbase = 0;
    bool timeout = false;
    int sendbase = 0;
    int size;
    byte * ack;
    byte * pkt;
    while(sendbase < get_length(msg))
    {
        // 8 bytes from udp headers + 8 bytes from seq and ack numbers = 16
        // divide the msg into segments if it cannot be sent as a single packet
        if(get_length(msg) - sendbase > max_segment_size - 16)
            size = max_segment_size - 16;
        else
            size = get_length(msg) - sendbase;

        if(timeout)
        {
            pkt = make_pkt(msg, unacked_sendbase, NextAckNum, size, unacked_sendbase);
            udpsend(pkt, dest_port, dest_ip);
            timeout = false;
            start_timer(timeout, 1000); // 1 second
        }
        // send segment if data available and window size allows
        else if(NextSeqNum < get_length(msg) && NextSeqNum < (window_size+sendbase))
        {
            pkt = make_pkt(msg, NextSeqNum, NextAckNum, size, sendbase);
            udpsend(pkt, dest_port, dest_ip);
            if(!is_timer_running())
            {
                start_timer(timeout, 1000);
                unacked_sendbase = sendbase;
            }
            NextSeqNum += size;
        }

        // if ack is received
        if((ack = udprcv(dest_port, dest_ip)) != nullptr)
        {
            int ack_num = get_ack(ack);
            if(ack_num > sendbase)
            {
                sendbase = ack_num;
            }
            if(sendbase == NextSeqNum)
            {
                stop_timer();
                unacked_sendbase = sendbase;
            }
            else
            {
                start_timer(timeout, 1000);
                unacked_sendbase = sendbase;
            }
        }
    }
}

byte * TCPOUDPReceive(int port, int ip)
{
    int ExpectedSeqNum = 0;
    byte empty_data[] = "";
    byte * pkt;
    byte * data;
    byte * ack_pkt;
    bool ack_timeout = false;

    while((pkt = udprcv(port, ip)) || is_timer_running())
    {
        int seq_num = get_seq(pkt);
        if(seq_num == ExpectedSeqNum)
        {
            data = extract(pkt);
            deliver_to_applayer(data);
            NextAckNum += get_length(data);
            ExpectedSeqNum = NextAckNum;
            ack_pkt = make_pkt(empty_data, NextSeqNum, NextAckNum, 0);
            udpsend(ack_pkt, port, ip);
            start_timer(ack_timeout, 500);
        }
        else if(seq_num > ExpectedSeqNum)
        {
            ack_pkt = make_pkt(empty_data, NextSeqNum, NextAckNum, 0);
            udpsend(ack_pkt, port, ip);
        }
        if(ack_timeout)
        {
            ack_timeout = false;
            start_timer(ack_timeout, 500);
            ack_pkt = make_pkt(empty_data, NextSeqNum, NextAckNum, 0);
            udpsend(ack_pkt, port, ip);
        }
    }
}
