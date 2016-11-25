#include <iostream>
#include <vector>

#define max_segment_size
#define N // window size

using namespace std;

// starts or restarts the timer
void start_timer();
// stops the timer
void stop_timer();
// returns true if timeout occured
bool timeout();
// return true if sender and receiver are connected
bool connected();

// cuts a segment from data and appends the nextseqnum and checksum in the beginning
unsigned char * make_pkt(int nextseqnum, unsigned char * data);
// udp send which adds 8 bytes headers and send data to network layer
void udp_send(unsigned char * data, int DestinationPort, int DestinationIP);
// udp receive removes 8 bytes headers and returns the data
unsigned char * udp_receive(int SrcPort, int SrcIP);
// calculates checksum on data
short calc_checksum(unsigned char * data);
// extracts checksum from headers
short get_checksum(unsigned char * pkt);
// extracts the seq_num from data and compares it to seqnum (return true if equal)
bool has_seqnum(unsigned char * data, int seqnum);
// extracts the data from the pkt
unsigned char * extract(unsigned char * pkt);
// deliver the data to application layer
void deliver_data(unsigned char * data);

void TCPOUDPSend(unsigned char * data, int DestinationPort, int DestinationIP);
unsigned char * TCPOUDPRcv(int SrcPort, int SrcIP);

int main(int argc, char *argv[])
{

    return 0;
}

void TCPOUDPSend(unsigned char * data, int DestinationPort, int DestinationIP)
{
    int nextseqnum = 1;
    int base = 1;
    vector<unsigned char *> sndpkt;
    unsigned char * pkt;
    unsigned char * ack;
    while(connected() && (base * max_segment_size) < sizeof(data))
    {
        if(nextseqnum < base+N)
        {
            pkt = make_pkt(nextseqnum, data);
            sndpkt.push_back(pkt);
            udp_send(sndpkt[nextseqnum], DestinationPort, DestinationIP);
            if(base == nextseqnum)
            {
                start_timer();
            }
            nextseqnum++;
        }
        if((ack = udp_receive(DestinationPort, DestinationIP)) != nullptr)
        {
            int checksum = calc_checksum(ack);
            if(checksum == get_checksum(ack)) // not corrupt
            {
                base = getacknum(ack);
                if(base == nextseqnum)
                {
                    stop_timer();
                }
                else
                {
                    start_timer();
                }
            }
        }
        if(timeout())
        {
            start_timer();
            for(int i = base; i < nextseqnum; i++)
            {
                udpsend(sndpkt[nextseqnum], DestinationPort, DestinationIP);
            }
        }
    }
}

unsigned char * TCPOUDPRcv(int SrcPort, int SrcIP)
{
    int expected_seqnum = 1;
    unsigned char * Ack = "";
    unsigned char * sndpkt = make_pkt(expected_seqnum, Ack);
    unsigned char * message;
    while(connected())
    {
        if((message = udp_receive(SrcPort, SrcIP)) != nullptr)
        {
            int checksum = calc_checksum(message);
            if(checksum == get_checksum(message) && has_seqnum(message, expected_seqnum))
            {
                unsigned char * data = extract(message);
                deliver_data(data);
                sndpkt = make_pkt(expected_seqnum, Ack);
                udp_send(sndpkt, SrcPort, SrcIP);
                expected_seqnum++;
            }
        }
        else
        {
            udp_send(sndpkt, SrcPort, SrcIP);
        }
    }
}

unsigned char * make_pkt(int nextseqnum, unsigned char * data)
{
    int size;
    int ptr = (nextseqnum - 1) * max_segment_size;
    if(sizeof(data) < max_segment_size - 14) // 8 from udp headers + 2 checksum + 4 seqnum
    {
        size = sizeof(data);
    }
    else
    {
        size = max_segment_size - 14;
    }
    unsigned char * pkt = unsigned char(size+14);
    int j = 0;
    for(int i = ptr; i < size; i++)
    {
        pkt[j+6] = data[i];
    }
    pkt[0] = nextseqnum >> 24;
    pkt[1] = nextseqnum >> 16;
    pkt[2] = nextseqnum >> 8;
    pkt[3] = nextseqnum >> 0;
    int checksum = calc_checksum(pkt);
    pkt[4] = checksum >> 8;
    pkt[5] = checksum >> 0;

    return pkt;
}

unsigned char * extract(unsigned char *pkt)
{
    unsigned char * data = unsigned char(sizeof(pkt) - 6);
    for(int i = 0; i < sizeof(pkt); i++)
    {
        data[i] = pkt[i+6];
    }
    return data;
}

short get_checksum(unsigned char * pkt)
{
    return ((pkt[4] << 8) + (pkt[5] << 0));
}

bool has_seqnum(unsigned char *data, int seqnum)
{
    int pkt_seqnum = (data[0] << 24) + (data[1] << 16) + (data[2] << 8) + (data[3] << 0);
    return (pkt_seqnum == seqnum)? true : false;
}
