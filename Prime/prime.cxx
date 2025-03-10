#include <prime.hxx>

using std::string;

typedef struct Prime {
    int x;
    Prime(int val) : x(val) {}
    
    string str(){
        return std::to_string(this->x);
    }

    string prime_or_composite(){
        std::ostringstream ss; 
        if(this->is_prime()) ss << str() << " is a prime number";
        else ss << str() << " is a composite number";
        return ss.str();
    }

    bool natural_number(){
        return this->x > 0;
    }

    bool greater_than_2(){
        return this->x > 2;
    }

    bool implication(int d){
        bool divides = (this->x % d == 0);
        bool check = (d == 1 || d == this->x);
        return (!divides) || check;
    }

    bool check_all_divisors(){
        for (int d = 1; d <= this->x; d++) {
            if(!implication(d)) return false;
        }
        return true;
    }

    bool is_prime(){
        if(!natural_number()) return false;
        if(!(greater_than_2())) return false;
        return check_all_divisors();
    }

} Prime;


int main(){
    Prime prime = Prime(3);
    std::cout << prime.str() << std::endl;
    std::cout << prime.prime_or_composite() << std::endl;
    return 0;
}