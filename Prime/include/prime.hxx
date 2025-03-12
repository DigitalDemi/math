#ifndef PRIME_HXX
#define PRIME_HXX

#include <iostream>
#include <string>
#include <sstream>

typedef struct Prime {
    int x;
    Prime(int val);
    
    std::string str();
    std::string prime_or_composite();
    bool natural_number();
    bool greater_than_2();
    bool implication(int d);
    bool check_all_divisors();
    bool is_prime();
} Prime;

#endif // PRIME_HXX