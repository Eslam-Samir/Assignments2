#include <iostream>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <cstdint>
#include <ctime>
#include <cmath>
#include <ostream>
#include <iomanip>
#include <map>
#include <stack>
#include <vector>

using namespace std;

class BigNumber
{
private:
    uint32_t * bigNum;
    int size;
    bool negative;
	static map<int, BigNumber> * FactorsOfTwo;

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

	static void initialize() 
	{
		if(FactorsOfTwo != nullptr)
			return;

		FactorsOfTwo = new map<int, BigNumber>();
		BigNumber * initial = new BigNumber("1");
		BigNumber two("2");
		FactorsOfTwo->insert(std::pair<const int, BigNumber>(0, initial));
		for(int i = 1; i < 1024; i++)
		{
			FactorsOfTwo->insert(std::pair<const int, BigNumber>(i, FactorsOfTwo->at(i-1) * two));
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
        if(nom < b)
        {
            return 0;
        }
		else if(b.getSize() == 1 && b.value()[0] == 2) // division by 2
		{
			uint64_t num;
			uint32_t carry = 0;
			for(int i = nom.getSize()-1; i >= 0; i--)
			{
				num = (uint64_t) carry*1000000000 + nom.value()[i];
				carry = num % 2;
				nom.value()[i] = num / 2;
			}
			nom.shrink();
			return nom;
		}
        else
        {
            BigNumber sum("0");
            do
            {
				uint64_t num = 0;
				if(nom.value()[nom.getSize()-1] > b.value()[b.getSize()-1])
				{
					num = nom.value()[nom.getSize()-1];
				}
				else
				{
					num = (uint64_t) nom.value()[nom.getSize()-1]*1000000000 + nom.value()[nom.getSize()-2];
				}
				uint32_t d = (num / b.value()[b.getSize()-1]);
				BigNumber div(&d, 1);
				BigNumber result = div * b;
				int size = nom.getSize()-result.getSize();
				result = result.append_zeros(size);
				while(result > nom)
				{
					BigNumber diff = result - nom;
					uint32_t a= diff.value()[diff.getSize()-1], a3 = b.value()[b.getSize()-1];
					int a2 = diff.getSize();
					uint32_t diff_multiplier = ceil(1.0*diff.value()[diff.getSize()-1]/(b.value()[b.getSize()-1]));
					if(d < diff_multiplier)
						d--;
					else
						d -= diff_multiplier;
					div = BigNumber(&d, 1);
					result = div * b;
					result = result.append_zeros(size);
					uint32_t next = d;
					while(nom > result)
					{
						next++;
						div = BigNumber(&next, 1);
						result = div * b;
						result = result.append_zeros(size);
					}
					d = next-1;
					div = BigNumber(&d, 1);
					result = div * b;
					result = result.append_zeros(size);
				}
				nom = nom - result;
				sum = sum.concatenate(div);
				nom.shrink();
            } while(nom > b);
            return sum;
        }
    }
	
	BigNumber operator%(BigNumber& b)
    {
        this->shrink();
        b.shrink();
        BigNumber nom = *this;
        if(nom < b)
        {
            return nom;
        }
		else if(b.getSize() == 1 && b.value()[0] == 2) // mod 2
		{
			uint32_t rem = nom.value()[0] % 2;
			BigNumber result(&rem, 1);
			return result;
		}
        else
        {
            do
            {
				uint64_t num = 0;
				if(nom.value()[nom.getSize()-1] > b.value()[b.getSize()-1])
				{
					num = nom.value()[nom.getSize()-1];
				}
				else
				{
					num = (uint64_t) nom.value()[nom.getSize()-1]*1000000000 + nom.value()[nom.getSize()-2];
				}
				uint32_t d = (num / b.value()[b.getSize()-1]);
				BigNumber div(&d, 1);
				BigNumber result = div * b;
				int size = nom.getSize()-result.getSize();
				result = result.append_zeros(size);
				while(result > nom)
				{
					BigNumber diff = result - nom;
					uint32_t diff_multiplier = ceil(1.0*diff.value()[diff.getSize()-1]/b.value()[b.getSize()-1]);
					if(d < diff_multiplier)
						d--;
					else
						d -= diff_multiplier;
					div = BigNumber(&d, 1);
					result = div * b;
					result = result.append_zeros(size);
					uint32_t next = d;
					while(nom > result)
					{
						next++;
						div = BigNumber(&next, 1);
						result = div * b;
						result = result.append_zeros(size);
					}
					d = next-1;
					div = BigNumber(&d, 1);
					result = div * b;
					result = result.append_zeros(size);
				}
				nom = nom - result;
				nom.shrink();
            } while(nom > b);
            return nom;
        }
    }
	
	BigNumber exponentiate(BigNumber &power, BigNumber &mod)
	{
		stack<int> powers;
		vector<BigNumber> factors;
		BigNumber test = power;
		for(int i = 1023; i >= 0; i--)
		{
			if(test >= FactorsOfTwo->at(i))
			{
				test = test - FactorsOfTwo->at(i);
				powers.push(i);
			}
		}
		BigNumber factor = *this;
		if(powers.top() == 0)
		{
			factors.push_back(factor);
			powers.pop();
		}
		for(int i = 1; i < 1023; i++)
		{
			if(powers.empty())
			{
				break;
			}
			factor = factor * factor;
			factor = factor % mod;
			if(powers.top() == i)
			{
				factors.push_back(factor);
				powers.pop();
			}
		}
		BigNumber result("1");
		for(int i = 0; i < factors.size(); i++)
		{
			result = (result * factors[i]) % mod;
		}
		return result;
	}
    bool operator<(BigNumber &b)
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

	bool operator<=(BigNumber &b)
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
				else if(this->value()[i] == b.value()[i] && i == 0)
				{
					return true;
				}
            }
            return false;
        }
    }

	bool operator>=(BigNumber& b)
    {
        return b <= *this;
    }

	BigNumber concatenate(BigNumber &b)
	{
		int size = this->getSize() + b.getSize();
		uint32_t * tmp = new uint32_t[size];
		for(int i = 0; i < b.getSize(); i++)
		{
			tmp[i] = b.value()[i];
		}
		for(int i = b.getSize(); i < size; i++)
		{
			tmp[i] = this->value()[i-b.getSize()];
		}
		BigNumber result(tmp, size);
		result.shrink();
		return result;
	}
	BigNumber append_zeros(int numberOfZeros)
	{
		uint32_t * tmp = new uint32_t[numberOfZeros+this->getSize()];
		for(int i = 0; i < numberOfZeros; i++)
		{
			tmp[i] = 0;
		}
		for(int i = numberOfZeros; i < numberOfZeros+this->getSize(); i++)
		{
			tmp[i] = this->value()[i-numberOfZeros];
		}
		BigNumber result(tmp, numberOfZeros+this->getSize());
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

map<int, BigNumber> * BigNumber::FactorsOfTwo;

int main()
{
	BigNumber::initialize();
    string p_str, q_str, e_str, operation;
    p_str = "12369571528747655798110188786567180759626910465726920556567298659370399748072366507234899432827475865189642714067836207300153035059472237275816384410077871";
    q_str = "2065420353441994803054315079370635087865508423962173447811880044936318158815802774220405304957787464676771309034463560633713497474362222775683960029689473";
    BigNumber number1(p_str);
    //BigNumber number2("593145345351");
	BigNumber number2(q_str);
	BigNumber one("1");
    BigNumber n = number1 * number2;
    n.print();
    
	BigNumber res = number1.exponentiate(number2, number2);
	res.print();
	/*
	BigNumber n1("2");
	BigNumber n2("1000");
	BigNumber mod("200");
	BigNumber res = n1.exponentiate(n2, mod);
	res.print();*/

	/*
    clock_t begin = clock();
    for(int i = 0; i < 10; i++)
    {
        BigNumber res = number1 / number2;
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
