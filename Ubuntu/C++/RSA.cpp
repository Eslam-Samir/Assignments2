#include <iostream>
#include <stdlib.h>
#include <string>

#define P_SIZE_IN_BYTES 64 // 512/8 = 64
#define Q_SIZE_IN_BYTES 64
#define N_SIZE_IN_BYTES P_SIZE_IN_BYTES+Q_SIZE_IN_BYTES

using namespace std;

typedef unsigned char byte; // size 1 byte

byte reminderBy2(string & s);
void divideBy2(string & s);
void fillArrayFromString(string & s, byte * s_arr, int size);
void multiplyBase256(byte * a, byte * b, byte * result, int a_size, int b_size, int res_size);
void divisionBase256(byte * a, byte * b, byte * result, int a_size, int b_size, int res_size);
void print(byte * arr, int size);

byte mulTable[256][256][2];
byte addTable[256][256][2];

int main()
{
    byte p[P_SIZE_IN_BYTES] = {0}, q[Q_SIZE_IN_BYTES] = {0}, n[N_SIZE_IN_BYTES] = {0};
    
    // base 256 (1 byte) multiplication table
    for(int i = 0; i < 256; i++)
    {
        for(int j = 0; j < 256; j++)
        {
            mulTable[i][j][0] = (byte) ((i*j) % 256);   // index 0 for value
            mulTable[i][j][1] = (byte) ((i*j) / 256);   // index 1 for carry   
        }
    }
    // base 256 (1 byte) addition table
    for(int i = 0; i < 256; i++)
    {
        for(int j = 0; j < 256; j++)
        {
            addTable[i][j][0] = (byte) ((i+j) % 256);   // index 0 for value
            addTable[i][j][1] = (byte) ((i+j) / 256);   // index 1 for carry   
        }
    }
    string p_str, q_str, e_str, operation; 
    p_str = "109026109913291424366305511581086089650628117463925776754560048454991130443047109026109913291424366305511581086089650628117463925776754560048454991130443047";
    q_str = "109026109913291424366305511581086089650628117463925776754560048454991130443047109026109913291424366305511581086089600628117463925776754560048454991130443047";
    //p_str = "10000";
    //q_str = "32154";
    /*
    getline(cin, p_str);
    getline(cin, q_str);
    getline(cin, e_str);
    */
    fillArrayFromString(p_str, p, P_SIZE_IN_BYTES);
    fillArrayFromString(q_str, q, Q_SIZE_IN_BYTES);
    multiplyBase256(p, q, n, P_SIZE_IN_BYTES, Q_SIZE_IN_BYTES, N_SIZE_IN_BYTES);
    print(p, P_SIZE_IN_BYTES);
    print(q, Q_SIZE_IN_BYTES);
    print(n, N_SIZE_IN_BYTES);
    //fillArrayFromString(e_str, e, E_SIZE_IN_BYTES);
   /* 
    while(true)
    {
        getline(cin, operation);
    }*/
    return 0;
}
/* mul result
241 155 170 213 73 44 1 137 243 211 79 113 94 135 104 1 197 206 101 149 156 179 73 176 145 28 17 189 118 243 52 36 128 231 152 225 53 108 230 116 148 3 214 188 42 216 213 93 1 44 67 134 39 89 163 199 50 208 180 96 4 138 53 48 231 4 79 182 75 255 81 23 241 231 40 115 210 15 187 105 55 15 28 118 61 38 137 7 172 213 86 37 162 50 106 73 106 163 222 246 202 222 157 202 137 188 200 231 94 36 173 152 9 21 165 119 117 43 88 234 102 64 40 203 224 238 109 4
*/
void print(byte * arr, int size)
{
    for(int i = 0; i<size; i++)
    {
        cout<< +arr[i] << " ";
    }
    cout <<endl<<endl;
}

void divisionBase256(byte * a, byte * b, byte * result, int a_size, int b_size, int res_size)
{
            
}

void multiplyBase256(byte * a, byte * b, byte * result, int a_size, int b_size, int res_size)
{
    byte mul, mulCarry, addCarry1, addCarry2;
    for(int i = 0; i < a_size; i++)
    {
        byte val1 = a[i];
        for(int j = 0; j < b_size; j++)
        {
            byte val2 = b[j];
            mulCarry = mulTable[val1][val2][1];
            mul = mulTable[val1][val2][0];
            
            addCarry1 = addTable[result[i+j]][mul][1];
            addCarry2 = addTable[result[i+j+1]][addCarry1][0];
            
            result[i+j] = addTable[result[i+j]][mul][0];
            result[i+j+1] = addTable[addCarry2][mulCarry][0];
            result[i+j+2] += addTable[addCarry2][mulCarry][1];
        }
    }
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


