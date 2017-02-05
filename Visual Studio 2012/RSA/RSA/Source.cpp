#include <iostream>
#include <stdlib.h>
#include <string>
#include <cstring>
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
    bool negative;
public:
    BigNumber(int size)
    {
        bigNum = new uint32_t[size]();
        this->size = size;
        this->negative = false;
        for(int i = 0; i < this->size; i++)
        {
            bigNum[i] = 0;
        }
    }
	BigNumber(uint32_t * big, int size)
    {
        bigNum = new uint32_t[size]();
        this->size = size;
        this->negative = false;
        for(int i = 0; i < this->size; i++)
        {
            bigNum[i] = big[i];
        }
    }
    BigNumber(string & s)
    {
        this->size = ceil(s.length()/9.0);
        this->negative = false;
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
    BigNumber(const char s[])
    {
        this->size = ceil(strlen(s)/9.0);
        this->negative = false;
        bigNum = new uint32_t[this->size]();
        for(int i = 0; i < this->size; i++)
        {
            bigNum[i] = 0;
        }
        for(int i = 0, length = strlen(s); i < this->size; i++)
        {
            int j = length-(i+1)*9;
            int limit = j+9;
            if(j < 0)
            {
                j = 0;
            }
            for(; j < limit; j++)
            {
                bigNum[i] = 10*bigNum[i] + (s[j] - '0');
            }
        }
    }
    BigNumber(BigNumber *b) {
        this->size = b->getSize();
        this->negative = b->isNegative();
        this->bigNum = new uint32_t[this->size]();
        for(int i = 0; i < this->size; i++)
        {
            bigNum[i] = b->value()[i];
        }
    }

    void operator=(BigNumber b)
    {
        this->size = b.getSize();
        this->negative = b.isNegative();
        bigNum = new uint32_t[this->size]();
        for(int i = 0; i < this->size; i++)
        {
            bigNum[i] = b.value()[i];
        }
    }

    BigNumber operator+(BigNumber& b)
    {
        this->shrink();
        b.shrink();
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
    BigNumber operator-(BigNumber& b)
    {
        this->shrink();
        b.shrink();
        BigNumber *num1, *num2;
        bool negative = false;
        if(this->getSize() == b.getSize())
        {
            if(this->value()[this->getSize()-1] >= b.value()[b.getSize()-1])
            {
                num1 = this;
                num2 = &b;
            }
            else
            {
                num1 = &b;
                num2 = this;
                negative = true;
            }
        }
        else if(this->getSize() > b.getSize())
        {
            num1 = this;
            num2 = &b;
        }
        else
        {
            num1 = &b;
            num2 = this;
            negative = true;
        }
        BigNumber result(num1);
        result.setNegative(negative);
        for(int i = 0; i < result.getSize(); i++)
        {
            if(num2->getSize() <= i)
            {
                break;
            }
            if(result.value()[i] < num2->value()[i])
            {
                result.value()[i] += 1000000000;
                for(int j = i+1; j < result.getSize(); j++)
                {
                    if(result.value()[j] > 0)
                    {
                        result.value()[j]--;
                        break;
                    }
                    else
                    {
                        result.value()[j] += 999999999;
                    }
                }
            }
            result.value()[i] -= num2->value()[i];
        }
        result.shrink();
        return result;
    }

    BigNumber operator*(BigNumber& b)
    {
        this->shrink();
        b.shrink();
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

    BigNumber operator/(BigNumber& b)
    {
        this->shrink();
        b.shrink();
        BigNumber nom = *this;
        if(this->getSize() < b.getSize())
        {
            return 0;
        }
        else
        {
            BigNumber two("2");
            BigNumber sum("0");
            do
            {
				int size_diff = nom.getSize() - b.getSize();
				int count1 = nom.numberOfDigits();
				int count2 = b.numberOfDigits();
				uint32_t num1 = nom.value()[nom.getSize()-1];
				uint32_t num2 = b.value()[b.getSize()-1];
				int size = count1;
				if(nom.getSize() > 1 && b.getSize() > 1)
				{
					int i = 0;
					while((num1/100000000) == 0 && (count1 - i) > 0)
					{
						num1 = num1*10 + (nom.value()[(count1-i)/9-1] / (int) pow(10, 9-i)) % 10;
						i++;
					}
					int j = 0;
					while((num2/100000000) == 0 && (count2 - j) > 0)
					{
						int r = b.value()[(count2-j)/9];
						int a = num2*10 + (b.value()[(count2-j)/9-1] / (int) pow(10, 9-j)) % 10;
						j++;
						if(a > num1)
						{
							size--;
							break;
						}
						num2 = a;
					}
					uint32_t div = num1 / num2;
					//BigNumber initial_factor = b.append(count1-9, digits, j--);
					BigNumber initial_factor = b.append(size-count2);
					BigNumber number(&div, 1);
					number.print();
					initial_factor.print();
					initial_factor = initial_factor * number;
					nom.print();
					initial_factor.print();
					sum = sum + initial_factor;
					nom = nom - initial_factor;
					cout << endl << nom.getSize() << endl << initial_factor.getSize() << endl;
					nom.print();
					nom.shrink();
					// TODO 
				}

				/*
				cout << size_diff << endl;
				nom.print();
				BigNumber initial_factor = b.append(size_diff);
				initial_factor.print();
				uint32_t div = nom.value()[nom.getSize()-1] / initial_factor.value()[initial_factor.getSize()-1];
				BigNumber number(&div, 1);
				number.print();
				initial_factor = initial_factor * number;
				initial_factor.print();
				sum = sum + initial_factor;
				nom = nom - initial_factor;
				nom.print();
                nom.shrink();
				cout << nom.getSize() << endl;*/
            } while(nom > b);
            return sum;
        }
    }/*
    BigNumber operator%(BigNumber& b)
    {
        this->shrink();
        b.shrink();
        BigNumber nom = *this;
        if(this->getSize() < b.getSize())
        {
            return nom;
        }
        else
        {
            BigNumber ten("10");
            BigNumber two("2");
            do
            {
                int size_diff = this->getSize() - b.getSize();
				BigNumber initial_factor = b.append(size_diff);
                BigNumber factor = initial_factor * two;
                while(factor < nom)
                {
                    initial_factor = factor;
                    factor = factor * two;
                }
                nom = nom - initial_factor;
                nom.shrink();
            } while(nom > b);
            return nom;
        }
    }*/
    bool operator<(BigNumber& b)
    {
        if(this->getSize() < b.getSize())
        {
            return true;
        }
        else if(this->getSize() > b.getSize())
        {
            return false;
        }
        else
        {
            for(int i = this->getSize()-1; i >= 0; i--)
            {
                if(this->value()[i] < b.value()[i])
                {
                    return true;
                }
                else if(this->value()[i] > b.value()[i])
                {
                    return false;
                }
            }
            return false;
        }
    }

    bool operator>(BigNumber& b)
    {
        return b < *this;
    }
	BigNumber append(int numberOfZeros)
	{
		int size = numberOfZeros/9 + 1;
		cout << size << endl <<  this->getSize() << endl;
		uint32_t * tmp = new uint32_t[size+this->getSize()];
		BigNumber ten("10");
		for(int i = 0; i < size-1; i++)
		{
			tmp[i] = 0;
			cout << i << ": " << tmp[i] << endl;
		}
		for(int i = size+this->getSize()-2; i >= size-1; i--)
		{
			tmp[i] = this->value()[i-size+1];
			cout << i << ": " << tmp[i] << endl;
		}
		BigNumber result(tmp, size+this->getSize()-1);
		for(int i = 0; i < (numberOfZeros%9); i++)
		{
			result = result * ten;
		}
		return result;
	}
/*
	BigNumber append(int numberOfZeros, int digits, int j)
	{
		
		int z = ceil(numberOfZeros/9.0);
		int size = numberOfZeros + digits;
		int a = ceil(size/9.0);

		uint32_t * tmp = new uint32_t[a];
		for(int i = 0; i < z; i++)
		{
			tmp[i] = 0;
		}
		int padding = numberOfZeros % 9;
		int first = padding;
		if(digits == 8)
			first--;
		
		int number = this->value()[this->getSize()-1];
		int p = pow(10, first);
		while(number >= p || number != 0)
		{
			number /= 10;
		}
		tmp[a-1] = number;
		int remaining = digits - first;

		
		for(int i = z; i < a; i++)
		{
			int p = pow(10, padding);
			uint32_t number = 0;
			if (this->getSize() > 1)
				number = this->value()[this->getSize()-2] % p;
			int power = pow(10, 9-padding);
			number = number*power + (this->value()[i-z] / p);
			
			tmp[i] = number;
		}/*
		int multiplier = 1;
		for(int i = first; i < 9; i++)
		{
			multiplier *= 10;
		}
		tmp[zeros_size-1] =  (this->value()[this->getSize()-1-start] / multiplier ) * multiplier;
		for(int i = zeros_size; i < arr_size; i++)
		{
			tmp[i] = this->value()[i-(zeros_size-1)];
		}
		BigNumber result(tmp, a);
		return result;
	}
	*/
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
        if(i == this->getSize()-1 || this->getSize() == 1)
        {
            return;
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
	int numberOfDigits()
	{
		int count = 0;
		for(int i = 0; i <= this->getSize()-2; i++)
		{
			count += 9;
		}
		uint32_t num = this->value()[this->getSize()-1];
		while(num > 0)
		{
			num /= 10;
			count++;
		}
		return count;
	}
    bool isNegative()
    {
        return this->negative;
    }
    void setNegative(bool n)
    {
        this->negative = n;
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
    BigNumber number2("59345678910");
    BigNumber res = number1 / number2;
    res.print();
    //number1.print();
    //number2.print();

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
