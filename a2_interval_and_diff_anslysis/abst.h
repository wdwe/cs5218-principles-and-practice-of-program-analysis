//
// Created by wayne on 3/22/24.
//

#ifndef A2_ABST_H
#define A2_ABST_H

#include <iostream>
#include <climits>
#include <cmath>

#include "llvm/Support/raw_ostream.h"

class Abst{
    friend Abst operator-(const Abst& one);
    friend Abst operator+(const Abst& one, const Abst& oth);
    friend Abst operator-(const Abst& one, const Abst& oth);
    friend Abst operator*(const Abst& one, const Abst& oth);
    friend Abst operator/(const Abst& one, const Abst& oth);
    friend Abst operator%(const Abst& one, const Abst& oth);
    friend bool operator<(const Abst& one, const Abst& oth);
    friend bool operator>(const Abst& one, const Abst& oth);
    friend bool operator==(const Abst& one, const Abst& oth);
    friend bool operator>=(const Abst& one, const Abst& oth);
    friend bool operator<=(const Abst& one, const Abst& oth);
public:
    Abst();
    Abst(int i); // allow implicit conversion
    Abst(long long l);

    bool isUndef() const;
    bool isPosInf() const;
    bool isNegInf() const;
    bool isInf() const;
    [[nodiscard]] std::string repr() const;
    static long long MAX;
    static long long MIN;
    const static long long INF = static_cast<long long>(INT_MAX) + 2;
    const static long long NEG_INF = static_cast<long long>(INT_MIN) - 1;
    const static long long UNDEF = NEG_INF - 1;
    static Abst inf();
    static Abst negInf();
    static Abst undef();
    static Abst abs(const Abst&);
private:
    long long d = UNDEF;
    static Abst abstract(long long val);
    static long long abstractToLL(long long val);
};

std::ostream& operator<<(std::ostream &out, const Abst& abst);
llvm::raw_ostream& operator<<(llvm::raw_ostream &out, const Abst& abst);


Abst operator-(const Abst& one);
Abst operator+(const Abst& one, const Abst& oth);
Abst operator-(const Abst& one, const Abst& oth);
Abst operator*(const Abst& one, const Abst& oth);
Abst operator/(const Abst& one, const Abst& oth);
Abst operator%(const Abst& one, const Abst& oth);
bool operator<(const Abst& one, const Abst& oth);
bool operator>(const Abst& one, const Abst& oth);
bool operator==(const Abst& one, const Abst& oth);
bool operator>=(const Abst& one, const Abst& oth);
bool operator<=(const Abst& one, const Abst& oth);



#endif //A2_ABST_H
