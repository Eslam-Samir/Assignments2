#include <iostream>
#include <cstdlib>
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
    vector<uint32_t> * bigNum;
    static map<int, BigNumber*> * FactorsOfTwo;

public:
    BigNumber()
    {
        bigNum = nullptr;
    }

    BigNumber(int size)
    {
        bigNum = new vector<uint32_t>(size);
        for(int i = 0; i < size; i++)
        {
            (*bigNum)[i] = 0;
        }
    }
    BigNumber(vector<uint32_t> * big, int size)
    {
        bigNum = new vector<uint32_t>(size);
        for(int i = 0; i < size; i++)
        {
            (*bigNum)[i] = (*big)[i];
        }
    }
	BigNumber(uint32_t big)
    {
        bigNum = new vector<uint32_t>(1);
        (*bigNum)[0] = big;
    }
    BigNumber(string s)
    {
        int size = ceil(s.length()/9.0);
        bigNum = new vector<uint32_t>(size);
        for(int i = 0; i < size; i++)
        {
            (*bigNum)[i] = 0;
        }
        for(int i = 0, length = s.length(); i < size; i++)
        {
            int j = length-(i+1)*9;
            int limit = j+9;
            if(j < 0)
            {
                j = 0;
            }
            for(; j < limit; j++)
            {
                (*bigNum)[i] = 10*(*bigNum)[i] + (s.at(j) - '0');
            }
        }
    }
    BigNumber(const char s[])
    {
        int size = ceil(strlen(s)/9.0);
        bigNum = new vector<uint32_t>(size);
        for(int i = 0; i < size; i++)
        {
            (*bigNum)[i] = 0;
        }
        for(int i = 0, length = strlen(s); i < size; i++)
        {
            int j = length-(i+1)*9;
            int limit = j+9;
            if(j < 0)
            {
                j = 0;
            }
            for(; j < limit; j++)
            {
                (*bigNum)[i] = 10*(*bigNum)[i] + (s[j] - '0');
            }
        }
    }
    BigNumber(BigNumber &b) {
        int size = b.getSize();
        bigNum = new vector<uint32_t>(size);
        for(int i = 0; i < size; i++)
        {
            (*bigNum)[i] = b.value(i);
        }
    }
    ~BigNumber()
    {
		if(bigNum != nullptr)
		{
			if(!bigNum->empty())
				bigNum->clear();
			delete bigNum;
		}
    }

    static void initialize()
    {
        if(FactorsOfTwo != nullptr)
            return;

        FactorsOfTwo = new map<int, BigNumber*>();
		BigNumber * initial = new BigNumber("1");
        BigNumber two("2");
        FactorsOfTwo->insert(std::pair<const int, BigNumber*>(0, initial));
        for(int i = 1; i < 1024; i++)
        {
			BigNumber * factor = new BigNumber(*(FactorsOfTwo->at(i-1)) * two);
            FactorsOfTwo->insert(std::pair<const int, BigNumber*>(i, factor));
        }
    }

	static void clear()
    {
        if(FactorsOfTwo == nullptr)
            return;

        map<int, BigNumber*>::iterator itr = FactorsOfTwo->begin();
		while(itr != FactorsOfTwo->end())
		{
			delete itr->second;
			itr++;
		}
		FactorsOfTwo->clear();
		delete FactorsOfTwo;
    }

    void operator=(BigNumber b)
    {
		if (bigNum != nullptr)
			delete bigNum;

        int size = b.getSize();
        bigNum = new vector<uint32_t>(size);
        for(int i = 0; i < size; i++)
        {
            (*bigNum)[i] = b.value(i);
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
            result.setValue(carry + num1->value(i), i);
            if(num2->getSize() > i)
            {
                result.setValue(result.value(i)+num2->value(i), i);
            }
            if(result.value(i) > 999999999)
            {
                carry = 1;
                result.setValue(result.value(i)-1000000000, i);
            }
            else
            {
                carry = 0;
            }
        }
        if(carry == 1)
        {
            result.expand(size+1);
            result.setValue(1, size);
        }
        return result;
    }

    BigNumber operator-(BigNumber& b)
    {
        this->shrink();
        b.shrink();
        BigNumber *num1, *num2;
        if(this->getSize() == b.getSize())
        {
            if(this->value(this->getSize()-1) >= b.value(b.getSize()-1))
            {
                num1 = this;
                num2 = &b;
            }
            else
            {
                num1 = &b;
                num2 = this;
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
        }
        BigNumber result(*num1);
        for(int i = 0; i < result.getSize(); i++)
        {
            if(num2->getSize() <= i)
            {
                break;
            }
            if(result.value(i) < num2->value(i))
            {
                result.setValue(result.value(i)+1000000000, i);
                for(int j = i+1; j < result.getSize(); j++)
                {
                    if(result.value(j) > 0)
                    {
                        result.setValue(result.value(j)-1, j);
                        break;
                    }
                    else
                    {
                        result.setValue(result.value(j)+999999999, j);
                    }
                }
            }
            result.setValue(result.value(i)-num2->value(i), i);
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
                mul = (uint64_t) this->value(j) * b.value(i-j);
                carry = mul / 1000000000;
                result.setValue(result.value(i)+(mul % 1000000000), i);
                result.setValue(result.value(i+1) + carry, i+1);
                if(result.value(i+1) > 999999999)
                {
                    result.setValue(result.value(i+2)+1, i+2);
                    result.setValue(result.value(i+1)-1000000000, i+1);
                }
                if(result.value(i) > 999999999)
                {
                    result.setValue(result.value(i+1)+1, i+1);
                    result.setValue(result.value(i)-1000000000, i);
                }
            }
        }
        result.shrink();
        return result;
    }
	/*
	BigNumber * operator/(BigNumber& b)
	{
		this->shrink();
		b.shrink();
		BigNumber * nom = new BigNumber(this);
		if(*nom < b)
		{
			return 0;
		}
		else if(b.getSize() == 1 && b.value(0) == 2) // division by 2
		{
			uint64_t num;
			uint32_t carry = 0;
			for(int i = nom->getSize()-1; i >= 0; i--)
			{
				num = (uint64_t) carry*1000000000 + nom->value(i);
				carry = num % 2;
				nom->setValue(num / 2, i);
			}
			nom->shrink();
			return nom;
		}
		else
		{
			BigNumber * sum = new BigNumber("0");
			do
			{
				uint64_t num = 0;
				if(nom->value(nom->getSize()-1) > b.value(b.getSize()-1))
				{
					num = nom->value(nom->getSize()-1);
				}
				else
				{
					num = (uint64_t) nom->value(nom->getSize()-1)*1000000000 + nom->value(nom->getSize()-2);
				}
				uint32_t d = num / b.value(b.getSize()-1);
				BigNumber * div = new BigNumber(d);
				BigNumber * result = *div * b;
				int size = nom->getSize()-result->getSize();
				uint32_t high = d;
				uint32_t low = d/2;
				while(high > low)
				{
					uint32_t mid = (low+high)/2;
					uint32_t mid_1 = mid+1;
					cout << endl << mid << endl;
					div = new BigNumber(mid);
					BigNumber div_1(mid_1);
					result = *div * b;
					BigNumber * result_1 = div_1 * b;
					if(*result > *nom->getPart(result->getSize()))
					{
						high = mid;
					}
					else if(*result <= *nom->getPart(result->getSize()) && *result_1 > *nom->getPart(result_1->getSize()))
					{
						div = div->append_zeros(size);
						result = result->append_zeros(size);
                        break;
					}
					else if(*result_1 <= *nom->getPart(result_1->getSize()))
					{
						div = div_1.append_zeros(size);
						result = result_1->append_zeros(size);
                        break;
					}
					else
					{
						low = mid;
					}
				}
				nom = *nom - *result;
				sum = *sum + *div;
				sum->print();
				nom->print();
				nom->shrink();
			} while(*nom > b);
			return sum;
		}
	}
	*/
	
    BigNumber operator/(BigNumber& b)
    {
        this->shrink();
        b.shrink();
        BigNumber nom(*this);
        if(this->getSize() < b.getSize())
        {
            return 0;
        }
		else if(b.getSize() == 1 && b.value(0) == 2) // division by 2
		{
			uint64_t num;
			uint32_t carry = 0;
			for(int i = nom.getSize()-1; i >= 0; i--)
			{
				num = (uint64_t) carry*1000000000 + nom.value(i);
				carry = num % 2;
				nom.setValue(num/2, i);
			}
			nom.shrink();
			return nom;
		}
        else
        {
			BigNumber * factor = new BigNumber("1");
            BigNumber two("2");
            BigNumber sum("0");
			BigNumber * result = new BigNumber(b);
            vector<BigNumber*> results;
            vector<BigNumber*> factors;
            factors.push_back(factor);
            results.push_back(result);

            while(b < nom)
            {
				int num_size = b.getSize();
				if(nom.getPart(b.getSize()) < b)
				{
					num_size++;
				}
				
				BigNumber num = nom.getPart(num_size);
				while(*result < num)
				{
					result = new BigNumber(*(result) * two);
					factor = new BigNumber(*(factor) * two);
					factors.push_back(factor);
					results.push_back(result);
				}/*
				int size = results.size();
                for(int i = size-1; i >= 0; i--)
                {
                    if(*results[i] <= *num)
                    {
                        int diff_size = nom->getSize() - num_size;
                        BigNumber * f = factors[i]->append_zeros(diff_size);
                        BigNumber * r = results[i]->append_zeros(diff_size);
                        sum = *sum + *f;
                        nom = *nom - *r;
                        delete f;
                        delete r;
                        break;
                    }
                }*/
                int low = 0;
				int high = results.size()-1;
				while(low < high)
				{
					int mid = (low+high)/2;
					if(*results[mid] > num)
					{
						high = mid;
					}
					else if(*results[mid] <= num && *results[mid+1] > num)
					{
						int diff_size = nom.getSize() - num_size;
                        BigNumber f = factors[mid]->append_zeros(diff_size);
                        BigNumber r = results[mid]->append_zeros(diff_size);
                        sum = sum + f;
                        nom = nom - r;
                        break;
					}
					else
					{
						low = mid;
					}
				}
                nom.shrink();
            }
			for(int i = results.size()-1; i >= 0; i--)
			{
				delete results[i];
				delete factors[i];
			}

            sum.shrink();
            return sum;
        }
    }
	
    BigNumber operator%(BigNumber& b)
    {
        this->shrink();
        b.shrink();
        BigNumber nom(*this);
        if(this->getSize() < b.getSize())
        {
            return nom;
        }
		else if(b.getSize() == 1 && b.value(0) == 2) // mod 2
		{
			vector<uint32_t> rem;
			rem.push_back(nom.value(0) % 2);
			BigNumber result(&rem, 1);
			return result;
		}
        else
        {
            BigNumber two("2");
			BigNumber * result = new BigNumber(b);
            vector<BigNumber*> results;
            results.push_back(result);

            while(b < nom)
            {
				int num_size = b.getSize();
				if(nom.getPart(b.getSize()) < b)
				{
					num_size++;
				}
				
				BigNumber num = nom.getPart(num_size);
				while(*result < num)
				{
					result = new BigNumber(*result * two);
					results.push_back(result);
				}
				/*
				int size = results.size();    
				for(int i = size-1; i >= 0; i--)
                {
                    if(*results[i] <= num)
                    {
                        int diff_size = nom->getSize() - num_size;
                        BigNumber r = results[i]->append_zeros(diff_size);
                        nom = *nom - r;
                        break;
                    }
                }*/
				int low = 0;
				int high = results.size()-1;
				while(low < high)
				{
					int mid = (low+high)/2;
					if(*results[mid] > num)
					{
						high = mid;
					}
					else if(*results[mid] <= num && *results[mid+1] > num)
					{
						int diff_size = nom.getSize() - num_size;
                        BigNumber r = results[mid]->append_zeros(diff_size);
                        nom = nom - r;
                        break;
					}
					else
					{
						low = mid;
					}
				}
                nom.shrink();
            }
			for(int i = results.size()-1; i >= 0; i--)
			{
				delete results[i];
			}
            return nom;
        }
    }

    BigNumber exponentiate(BigNumber &power, BigNumber &mod)
    {
        stack<int> powers;
        BigNumber test = power;

        for(int i = 1023; i >= 0; i--)
        {
            if(test >= *(FactorsOfTwo->at(i)))
            {
                test = test - *(FactorsOfTwo->at(i));
                powers.push(i);
            }
        }
        BigNumber result("1");
        BigNumber factor(*this);
        if(powers.top() == 0)
        {
            result = result * factor;
            result = result % mod;
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
                result = result * factor;
				result = result % mod;
                powers.pop();
            }
        }

        return result;
    }

	bool isPrime()
	{
		BigNumber n(*this);
		BigNumber one("1");
		BigNumber two("2");
		BigNumber n_1 = n - one;
		BigNumber q = n_1;
		BigNumber k("0");

		while(q % two != one)
		{
			q = q / two;
			k = k + one;
		}
		if(k < one)
		{
			return false; // n is even
		}
		for(int i = 2; i < 5; i++) // perform the test 3 times
		{
			BigNumber test(to_string(i));
			if(test >= n_1)
			{
				break;
			}
			bool inclusive = false;
			test = test.exponentiate(q, n);
			if(test == one || test == n_1)
			{
				inclusive = true;
				continue; // inclusive
			}
			else
			{
				BigNumber j("1");
				while(j < k)
				{
					test = test * test;
					test = test % n;
					if (test == n_1)
					{
						inclusive = true;
						break; // inclusive
					}
					j = j + one;
				}
			}
			if(!inclusive)
			{
				return false;
			}
		}
		return true;
	}
	
	BigNumber MulInverse(BigNumber &mod)
	{
		BigNumber A2("0");
		BigNumber A3(mod);
		BigNumber B2("1");
		BigNumber B3 = *this % mod;
		
		BigNumber one("1");
		BigNumber zero("0");
		bool A2_negative = false;
		while(B3 != one && B3 != zero)
		{
			BigNumber Q = A3 / B3;
			A2_negative = !A2_negative;
			BigNumber temp2 = Q * B2;
			temp2 = A2 + temp2;
			BigNumber temp3 = Q * B3;
			temp3 = A3 - temp3; 
			A2 = B2;
			A3 = B3;
			B2 = temp2;
			B3 = temp3;
		}
		if(A2_negative)
		{
			B2 = mod - B2;
		}
		return B2;
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
                if(this->value(i) < b.value(i))
                {
                    return true;
                }
                else if(this->value(i) > b.value(i))
                {
                    return false;
                }
            }
            return false;
        }
    }

    bool operator==(BigNumber &b)
    {
        if(this->getSize() < b.getSize())
        {
            return false;
        }
        else if(this->getSize() > b.getSize())
        {
            return false;
        }
        else
        {
            for(int i = this->getSize()-1; i >= 0; i--)
            {
                if(this->value(i) < b.value(i))
                {
                    return false;
                }
                else if(this->value(i) > b.value(i))
                {
                    return false;
                }
            }
            return true;
        }
    }
	bool operator!=(BigNumber &b)
    {
        return !(*this == b);
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
                if(this->value(i) < b.value(i))
                {
                    return true;
                }
                else if(this->value(i) > b.value(i))
                {
                    return false;
                }
                else if(this->value(i) == b.value(i) && i == 0)
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

    BigNumber append_zeros(int numberOfZeros)
    {
        vector<uint32_t> * tmp = new vector<uint32_t>(numberOfZeros+this->getSize());
        for(int i = 0; i < numberOfZeros; i++)
        {
            (*tmp)[i] = 0;
        }
        for(int i = numberOfZeros; i < numberOfZeros+this->getSize(); i++)
        {
            (*tmp)[i] = this->value(i-numberOfZeros);
        }
		BigNumber result(tmp, numberOfZeros+this->getSize());
		delete tmp;
        return result;
    }

    BigNumber getPart(int size)
    {
        if(size >= this->getSize())
        {
			BigNumber result(*this);
            return result;
        }
        else
        {
            vector<uint32_t> * res = new vector<uint32_t>(size);
            for(int i = this->getSize()-1; i >= this->getSize()-size; i--)
            {
                (*res)[i-(this->getSize()-size)] = this->value(i);
            }
            BigNumber result(res, size);
			delete res;
            return result;
        }
    }

    void expand(int size)
    {
        bigNum->resize(size);
    }
    void shrink()
    {
        for(int i = this->getSize()-1; i >= 0; i--)
        {
            if((*bigNum)[i] == 0)
            {
                bigNum->pop_back();
            }
            else
            {
                break;
            }
        }
    }

    int getSize()
    {
        return bigNum->size();
    }
    uint32_t value(int i)
    {
        return (*bigNum)[i];
    }
    void setValue(int v, int i)
    {
        (*bigNum)[i] = v;
    }
    void print()
    {
        cout << (*bigNum)[this->getSize()-1];
        for(int i = this->getSize()-2; i >= 0; i--)
        {
            cout<< setw(9) << setfill('0') << (*bigNum)[i];
        }
        cout << endl;
    }
};

map<int, BigNumber*> * BigNumber::FactorsOfTwo;

int main()
{
    BigNumber::initialize();
	/*
	string p_str, q_str, e_str;
    p_str = "12369571528747655798110188786567180759626910465726920556567298659370399748072366507234899432827475865189642714067836207300153035059472237275816384410077871";
    q_str = "2065420353441994803054315079370635087865508423962173447811880044936318158815802774220405304957787464676771309034463560633713497474362222775683960029689473";
	e_str = "65537";
	BigNumber p(p_str);
	BigNumber q(q_str);
	BigNumber e(e_str);

	BigNumber n = p * q;
	n.print();
	BigNumber one("1");
	BigNumber phi = (p - one) * (q - one);
	phi.print();
	BigNumber d = e.MulInverse(phi);
	d.print();
	BigNumber m("15645646");
	BigNumber c = m.exponentiate(e, n);
	c.print();
	cout << "P is prime: " << p.isPrime() << endl;
	cout << "Q is prime: " << q.isPrime() << endl;
	*/

	/*
	clock_t begin = clock();
	//BigNumber res = number2.MulInverse(number1);
	//BigNumber res = number2.exponentiate(number1, n);
	cout << endl << number1.isPrime() << endl;
	clock_t end = clock();
    double elapsed_secs = double(end - begin) / (CLOCKS_PER_SEC / 1000);
    cout << elapsed_secs << endl;
    //res.print();
	*/
	
	BigNumber p, q, e, d, n, phi, m, c;
	string s;
    while(true)
    {
        getline(cin, s);
		string name = s.substr(0,2); 
		if(name == "P=")
		{
			p = BigNumber(s.substr(2));
		}
		else if(name == "Q=")
		{
			q = BigNumber(s.substr(2));
		}
		else if(name == "E=")
		{
			e = BigNumber(s.substr(2));
		}
		else
		{
			if(s == "IsPPrime")
			{
				if(p.isPrime())
				{
					cout << "Yes" << endl;
				}
				else
				{
					cout << "No" << endl;
				}
			}
			else if(s == "IsQPrime")
			{
				if(q.isPrime())
				{
					cout << "Yes" << endl;
				}
				else
				{
					cout << "No" << endl;
				}
			}
			else if(s == "PrintN")
			{
				n = p * q;
				n.print();
			}
			else if(s == "PrintPhi")
			{
				BigNumber one("1");
				phi = (p - one) * (q - one);
				phi.print();
			}
			else if(s == "PrintD")
			{
				d = e.MulInverse(phi);
				d.print();
			}
			else if(s == "Quit")
			{
				break;
			}
			else if(s.substr(0, 14) == "EncryptPublic=")
			{
				m = BigNumber(s.substr(14));
				c = m.exponentiate(e, n);
				c.print();
			}
			else if(s.substr(0, 15) == "EncryptPrivate=")
			{
				m = BigNumber(s.substr(15));
				c = m.exponentiate(d, n);
				c.print();
			}
		}
    }
	BigNumber::clear();
    return 0;
}