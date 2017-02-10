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
    BigNumber(string & s)
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
    BigNumber(BigNumber *b) {
        int size = b->getSize();
        bigNum = new vector<uint32_t>(size);
        for(int i = 0; i < size; i++)
        {
            (*bigNum)[i] = b->value(i);
        }
    }
    ~BigNumber()
    {
		if(bigNum != nullptr && bigNum->size()>0)
		{
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
            FactorsOfTwo->insert(std::pair<const int, BigNumber*>(i, *(FactorsOfTwo->at(i-1)) * two));
        }
    }

    void operator=(BigNumber b)
    {
        int size = b.getSize();
        bigNum = new vector<uint32_t>(size);
        for(int i = 0; i < size; i++)
        {
            (*bigNum)[i] = b.value(i);
        }
    }

    BigNumber * operator+(BigNumber& b)
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
        BigNumber * result = new BigNumber(size);
        uint32_t carry = 0;
        for(int i = 0; i < size; i++)
        {
            result->setValue(carry + num1->value(i), i);
            if(num2->getSize() > i)
            {
                result->setValue(result->value(i)+num2->value(i), i);
            }
            if(result->value(i) > 999999999)
            {
                carry = 1;
                result->setValue(result->value(i)-1000000000, i);
            }
            else
            {
                carry = 0;
            }
        }
        if(carry == 1)
        {
            result->expand(size+1);
            result->setValue(1, size);
        }
        return result;
    }
    BigNumber * operator-(BigNumber& b)
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
        BigNumber * result = new BigNumber(num1);
        for(int i = 0; i < result->getSize(); i++)
        {
            if(num2->getSize() <= i)
            {
                break;
            }
            if(result->value(i) < num2->value(i))
            {
                result->setValue(result->value(i)+1000000000, i);
                for(int j = i+1; j < result->getSize(); j++)
                {
                    if(result->value(j) > 0)
                    {
                        result->setValue(result->value(j)-1, j);
                        break;
                    }
                    else
                    {
                        result->setValue(result->value(j)+999999999, j);
                    }
                }
            }
            result->setValue(result->value(i)-num2->value(i), i);
        }
        result->shrink();
        return result;
    }

    BigNumber * operator*(BigNumber& b)
    {
        this->shrink();
        b.shrink();
        int size = b.getSize() + this->getSize();
        BigNumber * result = new BigNumber(size);
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
                result->setValue(result->value(i)+(mul % 1000000000), i);
                result->setValue(result->value(i+1) + carry, i+1);
                if(result->value(i+1) > 999999999)
                {
                    result->setValue(result->value(i+2)+1, i+2);
                    result->setValue(result->value(i+1)-1000000000, i+1);
                }
                if(result->value(i) > 999999999)
                {
                    result->setValue(result->value(i+1)+1, i+1);
                    result->setValue(result->value(i)-1000000000, i);
                }
            }
        }
        result->shrink();
        return result;
    }

    BigNumber * operator/(BigNumber& b)
    {
        this->shrink();
        b.shrink();
        BigNumber * nom = new BigNumber(this);
        if(this->getSize() < b.getSize())
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
				nom->setValue(num/2, i);
			}
			nom->shrink();
			return nom;
		}
        else
        {
            BigNumber * factor = new BigNumber("1");
            BigNumber two("2");
            BigNumber * sum = new BigNumber("0");
            BigNumber * result = new BigNumber(b);
            vector<BigNumber*> results;
            vector<BigNumber*> factors;
            factors.push_back(factor);
            results.push_back(result);

            while(b < *nom)
            {
				int num_size = b.getSize();
				if(*nom->getPart(b.getSize()) < b)
				{
					num_size++;
				}
				
				BigNumber num = nom->getPart(num_size);
				while(*result < num)
				{
					result = *result * two;
					factor = *factor * two;
					factors.push_back(factor);
					results.push_back(result);
				}
				int size = results.size();
                for(int i = size-1; i >= 0; i--)
                {
                    if(*results[i] <= num)
                    {
                        int diff_size = nom->getSize() - num_size;
                        BigNumber f = factors[i]->append_zeros(diff_size);
                        BigNumber r = results[i]->append_zeros(diff_size);
                        sum = *sum + f;
                        nom = *nom - r;
                        break;
                    }
                }
                nom->shrink();
            }
			for(int i = results.size()-1; i > 0; i--)
			{
				delete results[i];
				delete factors[i];
			}
			delete factors[0];
            sum->shrink();
            return sum;
        }
    }

    BigNumber * operator%(BigNumber& b)
    {
        this->shrink();
        b.shrink();
        BigNumber * nom = new BigNumber(this);
        if(this->getSize() < b.getSize())
        {
            return nom;
        }
		else if(b.getSize() == 1 && b.value(0) == 2) // mod 2
		{
			vector<uint32_t> rem;
			rem.push_back(nom->value(0) % 2);
			BigNumber * result = new BigNumber(&rem, 1);
			return result;
		}
        else
        {
            BigNumber * factor = new BigNumber("1");
            BigNumber two("2");
            BigNumber * result = new BigNumber(b);
            vector<BigNumber*> results;
            vector<BigNumber*> factors;
            factors.push_back(factor);
            results.push_back(result);

            while(b < *nom)
            {
				int num_size = b.getSize();
				if(*nom->getPart(b.getSize()) < b)
				{
					num_size++;
				}
				
				BigNumber num = nom->getPart(num_size);
				while(*result < num)
				{
					result = *result * two;
					factor = *factor * two;
					factors.push_back(factor);
					results.push_back(result);
				}
				int size = results.size();
                for(int i = size-1; i >= 0; i--)
                {
                    if(*results[i] <= num)
                    {
                        int diff_size = nom->getSize() - num_size;
                        BigNumber f = factors[i]->append_zeros(diff_size);
                        BigNumber r = results[i]->append_zeros(diff_size);
                        nom = *nom - r;
                        break;
                    }
                }
                nom->shrink();
            }
			for(int i = results.size()-1; i > 0; i--)
			{
				delete results[i];
				delete factors[i];
			}
			delete factors[0];
            return nom;
        }
    }

    BigNumber exponentiate(BigNumber &power, BigNumber &mod)
    {
        stack<int> powers;
        vector<BigNumber*> factors;
        BigNumber test = power;
        for(int i = 1023; i >= 0; i--)
        {
            if(test >= *(FactorsOfTwo->at(i)))
            {
                test = test - *(FactorsOfTwo->at(i));
                powers.push(i);
            }
        }
        BigNumber * factor = new BigNumber(this);
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
            factor = *factor * *factor;
            factor = *factor % mod;
            if(powers.top() == i)
            {
                factors.push_back(factor);
                powers.pop();
            }
        }
        BigNumber * result = new BigNumber("1");
        for(int i = 0; i < factors.size(); i++)
        {
            result = (*result) * *factors[i];
            result = (*result) % mod;
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

    BigNumber * append_zeros(int numberOfZeros)
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
		BigNumber * result = new BigNumber(tmp, numberOfZeros+this->getSize());
        return result;
    }

    BigNumber * getPart(int size)
    {
        if(size >= this->getSize())
        {
			BigNumber * result = new BigNumber(*this);
            return result;
        }
        else
        {
            vector<uint32_t> * res = new vector<uint32_t>(size);
            for(int i = this->getSize()-1; i >= this->getSize()-size; i--)
            {
                (*res)[i-(this->getSize()-size)] = this->value(i);
            }
            BigNumber * result = new BigNumber(res, size);
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
        //cout<< (*bigNum)[this->size-1]/1000000 << " " << ((*bigNum)[this->size-1]/1000)%1000 << " " << (*bigNum)[this->size-1]%1000 << " ";
        for(int i = this->getSize()-2; i >= 0; i--)
        {
            cout<< setw(9) << setfill('0') << (*bigNum)[i];
            //cout<< (*bigNum)[i]/1000000 << " " << ((*bigNum)[i]/1000)%1000 << " " << (*bigNum)[i]%1000 << " ";
        }
        cout <<endl<<endl;
    }
};

map<int, BigNumber*> * BigNumber::FactorsOfTwo;

int main()
{
    BigNumber::initialize();
    string p_str, q_str, e_str, operation;
    p_str = "12369571528747655798110188786567180759626910465726920556567298659370399748072366507234899432827475865189642714067836207300153035059472237275816384410077871";
    q_str = "2065420353441994803054315079370635087865508423962173447811880044936318158815802774220405304957787464676771309034463560633713497474362222775683960029689473";
    BigNumber number1(p_str);
	BigNumber number3(q_str);
    //BigNumber number2("593145345351");
    BigNumber number2("5");
    //BigNumber one("1");
    BigNumber n = number1 * number3;
    //n.print();
	BigNumber test1("25548364798832019218170326077010425733930233389897468141147917831084690989884562791601588954296621731652139141347541240725432606132471100644835778517336041031200174441223836394229943651678525471050219216183727749114047330431603023948126844573697946795476319956787513765533596926704755530772983549787878951983");
    BigNumber test("14189485321177462408318672114557330546247509303317064953027389385275897573822172257132865340641176247566481139072907283946504374015733753097985297565236283248440762546921964722811073666518084045676411531969697963714289397405456950756650291873251449912198252564556899004325611831203414257425755116609547354567");
	BigNumber res = number1.exponentiate(number3, n);
    cout << endl << (res == test) << endl;   
	cout << endl << (n == test1) << endl;   
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
