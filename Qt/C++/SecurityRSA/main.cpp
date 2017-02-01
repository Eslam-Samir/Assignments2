#include <iostream>
#include <stdlib.h>
#include <string>
#include <cstdint>
#include <cmath>
#include <ctime>
#include <ostream>
#include <iomanip>

using namespace std;

class BigNumber
{
private:
    uint32_t * bigNum;
    int size;
public:
    BigNumber(int size)
    {
        bigNum = new uint32_t[size]();
        this->size = size;
        for(int i = 0; i < this->size; i++)
        {
            bigNum[i] = 0;
        }
    }
    BigNumber(string & s)
    {
        this->size = ceil(s.length()/9.0);

        bigNum = new uint32_t[this->size]();
        for(int i = 0; i < this->size; i++)
        {
            bigNum[i] = 0;
        }
        for(int i = 0, length = s.length(); i < this->size; i++)
        {
            int j = length-(i+1)*9;
            int limit = j+9;
            if(j < 0)
            {
                j = 0;
            }
            for(; j < limit; j++)
            {
                bigNum[i] = 10*bigNum[i] + (s.at(j) - '0');
            }
        }
    }
    BigNumber operator=(BigNumber& b)
    {
        delete[] bigNum;
        this->size = b.getSize();
        bigNum = new uint32_t[this->size]();
        for(int i = 0; i < this->size; i++)
        {
            bigNum[i] = b.value()[i];
        }
    }

    BigNumber operator+(BigNumber& b)
    {
        int size;
        BigNumber *num1, *num2;
        if(b.getSize() > this->getSize())
        {
            size = b.getSize();
            num1 = &b;
            num2 = this;
        }
        else
        {
            size = this->getSize();
            num1 = this;
            num2 = &b;
        }
        BigNumber result(size);
        uint32_t carry = 0;
        for(int i = 0; i < size; i++)
        {
            result.value()[i] = carry + num1->value()[i];
            if(num2->getSize() > i)
            {
                result.value()[i] += num2->value()[i];
            }
            if(result.value()[i] > 999999999)
            {
                carry = 1;
                result.value()[i] -= 1000000000;
            }
            else
            {
                carry = 0;
            }
        }
        if(carry == 1)
        {
            result.expand(size+1);
            result.value()[size] = 1;
        }
        return result;
    }

    BigNumber operator*(BigNumber& b)
    {
        int size = b.getSize() + this->getSize();
        BigNumber result(size);
        uint64_t mul;
        uint32_t carry = 0;
        for(int i = 0; i < size; i++)
        {
            for(int j = 0; j <= i; j++)
            {
                if(j >= this->getSize())
                {
                    break;
                }
                if((i-j) >= b.getSize())
                {
                    continue;
                }
                mul = (uint64_t) this->value()[j] * b.value()[i-j];
                carry = mul / 1000000000;
                result.value()[i] += mul % 1000000000;
                result.value()[i+1] += carry;
                if(result.value()[i+1] > 999999999)
                {
                    result.value()[i+2] += 1;
                    result.value()[i+1] -= 1000000000;
                }
                if(result.value()[i] > 999999999)
                {
                    result.value()[i+1] += 1;
                    result.value()[i] -= 1000000000;
                }
            }
        }
        result.shrink();
        return result;
    }
    void expand(int size)
    {
        if(size <= this->size)
        {
            return;
        }
        uint32_t * temp = bigNum;
        bigNum = new uint32_t[size]();
        for(int i = 0; i < this->size; i++)
        {
            bigNum[i] = temp[i];
        }
        for(int i = this->size; i < size; i++)
        {
            bigNum[i] = 0;
        }
        this->size = size;
        delete[] temp;
    }
    void shrink()
    {
        int i = this->getSize()-1;
        for(; i > 0; i--)
        {
            if(this->value()[i] != 0)
            {
                break;
            }
        }
        uint32_t * temp = bigNum;
        this->size = i+1;
        bigNum = new uint32_t[this->size]();
        for(int i = 0; i < this->size; i++)
        {
            bigNum[i] = temp[i];
        }
        delete[] temp;
    }

    int getSize()
    {
        return this->size;
    }
    uint32_t * value()
    {
        return bigNum;
    }

    void print()
    {
        //cout << bigNum[this->size-1];
        cout<< bigNum[this->size-1]/1000000 << " " << (bigNum[this->size-1]/1000)%1000 << " " << bigNum[this->size-1]%1000 << " ";
        for(int i = this->size-2; i >= 0; i--)
        {
            //cout<< setw(9) << setfill('0') << bigNum[i];
            cout<< bigNum[i]/1000000 << " " << (bigNum[i]/1000)%1000 << " " << bigNum[i]%1000 << " ";
        }
        cout <<endl<<endl;
    }
};

int main()
{
    string p_str, q_str, e_str, operation;
    p_str = "12369571528747655798110188786567180759626910465726920556567298659370399748072366507234899432827475865189642714067836207300153035059472237275816384410077871";
    q_str = "2065420353441994803054315079370635087865508423962173447811880044936318158815802774220405304957787464676771309034463560633713497474362222775683960029689473";
    BigNumber number1(p_str);
    BigNumber number2(q_str);
    //number1.print();
    //number2.print();
    //BigNumber res = number1 * number2;
    //res.print();
/*
    clock_t begin = clock();
    for(int i = 0; i < 1000; i++)
    {
        BigNumber res = number1 * number2;
    }
    clock_t end = clock();
    double elapsed_secs = double(end - begin) / (CLOCKS_PER_SEC / 1000);
    cout << elapsed_secs << endl;
*/
    //p_str = "10000";
    //q_str = "32154";
    /*
    getline(cin, p_str);
    getline(cin, q_str);
    getline(cin, e_str);
    */
   /*
    while(true)
    {
        getline(cin, operation);
    }*/
    return 0;
}
