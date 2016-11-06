#include <iostream>

using namespace std;

struct Process_Port {
    int sourceport;
    int process;
};

typedef unsigned char byte;

int getProcessHandler();
int getMessageLength(byte * message);
void send(byte * message, int dip);
int Sendto(byte * message, int dip, int dport);

int main(int argc, char *argv[])
{
    cout << "Hello World!" << endl;
    return 0;
}

void send(byte * message, int dip)
{

}

int getProcessHandler()
{
    return 0;
}

int getMessageLength(byte * message)
{
    return 1;
}

#define max_segment_length 1024

Process_Port Array[10000];

int Sendto(byte * message, int dip, int dport)
{
    int currentProcessHandler = getProcessHandler();
    int SourcePort;
    for(int i = 0; i < 10000; i++)
    {
        if(Array[i].process == currentProcessHandler)
        {
            SourcePort = Array[i].sourceport;
            break;
        }
    }
    if(SourcePort == NULL)
    {
        return 1; // failed to find source port
    }

    int messageLength = getMessageLength(message);
    int messagePtr = 0;

    // splits message into segments if length > max_segment_length
    while(messagePtr != messageLength)
    {
        // error occured while sending message
        // messagePtr shouldn't exceed message length
        if(messagePtr > messageLength)
        {
            return 2;
        }
        int currentSegmentSize = 0;
        // what is left of the message is < max_segment_size
        if((messageLength - messagePtr + 8) < max_segment_length)
        {
            currentSegmentSize = messageLength - messagePtr + 8;
        }
        // what is left of the message is > max_segment_size
        else
        {
            currentSegmentSize = max_segment_length;
        }
        byte segment[currentSegmentSize];
        for(int i = 0; i < (messagePtr + currentSegmentSize - 8); i++)
        {
            // copy the message into the segment one part at a time
            segment[i + 8] = message[messagePtr + i];
        }

        // evaluate checksum
        int CheckSum = 0;
        for(int i = 8; i < currentSegmentSize - 1; i++)
        {
           int TwoBytes = (segment[i] | segment[i + 1] << 8);
           CheckSum += TwoBytes;
           // check for carry 1 and wraparound if true
           if((CheckSum & 0x00010000) == 0x00010000)
           {
               // invert carry bit
               CheckSum ^= 0x00010000;
               // add 1 to sum
               CheckSum += 1;
           }
        }
        // invert all bits
        CheckSum = ~CheckSum;

        // add headers to the segment
        segment[0] = (byte) SourcePort;
        segment[1] = (byte) (SourcePort >> 8);
        segment[2] = (byte) dport;
        segment[3] = (byte) (dport >> 8);
        segment[4] = (byte) currentSegmentSize;
        segment[5] = (byte) (currentSegmentSize >> 8);
        segment[6] = (byte) CheckSum;
        segment[7] = (byte) (CheckSum >> 8);

        // update message pointer
        messagePtr += currentSegmentSize - 8;
        // send segment to destination ip
        send(segment, dip);
    }
    // return 0 on success
    return 0;
}
