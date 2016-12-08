#include <iostream>
#include <stdlib.h>
#include <string>
#include <bitset>
#define P_SIZE_IN_BYTES 64 // 512/8 = 64
#define Q_SIZE_IN_BYTES 64
#define N_SIZE_IN_BYTES P_SIZE_IN_BYTES+Q_SIZE_IN_BYTES

using namespace std;

typedef unsigned char byte; // size 1 byte

byte reminderBy2(string & s);
void divideBy2(string & s);
void fillArrayFromString(string & s, byte * s_arr, int size);
void multiplyLargeNums(byte * a, byte * b, byte * result, int a_size, int b_size, int res_size);

int main()
{
    byte p[P_SIZE_IN_BYTES] = {0}, q[Q_SIZE_IN_BYTES] = {0}, n[N_SIZE_IN_BYTES] = {0};
    string p_str, q_str, e_str, operation; 
    p_str = "109026109913291424366305511581086089650628117463925776754560048454991130443047109026109913291424366305511581086089650628117463925776754560048454991130443047";
    q_str = "109026109913291424366305511581086089650628117463925776754560048454991130443047109026109913291424366305511581086089600628117463925776754560048454991130443047";
    /*
    getline(cin, p_str);
    getline(cin, q_str);
    getline(cin, e_str);
    */
    fillArrayFromString(p_str, p, P_SIZE_IN_BYTES);
    fillArrayFromString(p_str, p, Q_SIZE_IN_BYTES);
    //fillArrayFromString(e_str, e, E_SIZE_IN_BYTES);
    
    while(true)
    {
        getline(cin, operation);
    }
    return 0;
}

void multiplyLargeNums(byte * a, byte * b, byte * result, int a_size, int b_size, int res_size)
{
    
}

void fillArrayFromString(string & s, byte * s_arr, int size)
{
    byte reminder;
    for(int i = 0; i < 8*size; i++)
    {
        reminder = reminderBy2(s);
        
        s_arr[i/8] += (reminder << i%8);
        //cout << "reminder: " << +reminder << " index " << i << "  " << +s_arr[i/8] << endl; // Debugging
        divideBy2(s);
        if(s[0] == '0')
        {
            s.erase(0, 1);
        }
        if(s.empty())
        {
            break;
        }
    }
}

void divideBy2(string & s)
{
    int nextAdditive = 0, additive;
    int digit;
    for(int i = 0; i < s.length(); i++)
    {
        digit = s[i] - '0';
        additive = nextAdditive;
        if((digit % 2) != 0) // if digit is odd
        {
            nextAdditive = 5;
        }
        else
        {
            nextAdditive = 0;
        }
        digit /= 2;
        digit += additive;
        s[i] = digit + '0';
    }
}

byte reminderBy2(string & s)
{
    return (s[s.length() - 1] - '0') % 2;
}


