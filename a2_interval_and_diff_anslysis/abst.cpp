//
// Created by wayne on 3/22/24.
//

#include "abst.h"
/*
 * Abstract domain
 */


#define UNDEF_CHECK(one, oth)                               \
    if ((one).isUndef() || (oth).isUndef())                 \
        return Abst::undef()

long long Abst::MAX = Abst::INF - 1;
long long Abst::MIN = Abst::NEG_INF + 1;


Abst::Abst() {}

Abst::Abst(int i) {
    this->d = this->abstractToLL(i);
}

Abst::Abst(long long l) {
    this->d = this->abstractToLL(l);
}

bool Abst::isUndef() const {
    return (this->d == UNDEF);
}

bool Abst::isPosInf() const {
    return (this->d == Abst::INF);
}

bool Abst::isNegInf() const {
    return (this->d == Abst::NEG_INF);
}

bool Abst::isInf() const {
    return (this->isPosInf() || this->isNegInf());
}


long long Abst::abstractToLL(long long val) {
    if (val > Abst::MAX)
        return Abst::INF;
    if (val < Abst::MIN)
        return Abst::NEG_INF;
    return val;
}

Abst Abst::abstract(long long val) {
    Abst abst;
    abst.d = abstractToLL(val);
    return abst;

}


std::string Abst::repr() const {
    if (this->d == Abst::INF)
        return "INF";
    if (this->d == Abst::NEG_INF)
        return "-INF";
    if (this->d == UNDEF)
        return "UNDEF";
    return std::to_string(this->d);
}



Abst Abst::inf() {
    Abst abst;
    abst.d = INF;
    return abst;
}

Abst Abst::negInf() {
    Abst abst;
    abst.d = NEG_INF;
    return abst;
}

Abst Abst::undef() {
    return Abst();
}

Abst Abst::abs(const Abst &abst) {
    return abstract(std::abs(abst.d));
}

std::ostream& operator<<(std::ostream &out, const Abst &abst) {
    out << abst.repr();
    return out;
}

llvm::raw_ostream& operator<<(llvm::raw_ostream& out, const Abst& abst) {
    out << abst.repr();
    return out;
}

Abst operator-(const Abst& one) {
    if (one.isUndef())
        /* if any value is UNDEF, return UNDEF*/
        return Abst::undef();

    return Abst::abstract(-one.d);
}


Abst operator+(const Abst &one, const Abst &oth) {
    UNDEF_CHECK(one, oth);

    if (one.isInf() && !oth.isInf())
        return one;
    if (one.isInf() && !oth.isInf())
        return oth;

    long long val = one.d + oth.d;
    return Abst::abstract(val);
}

Abst operator-(const Abst &one, const Abst &oth) {
    UNDEF_CHECK(one, oth);

    if (one.isInf() && !oth.isInf())
        return one;

    if (oth.isInf() && !one.isInf())
        return -oth;

    long long val = one.d - oth.d;
    return Abst::abstract(val);
}

Abst operator*(const Abst &one, const Abst &oth) {
    UNDEF_CHECK(one, oth);

    if (oth.d == 0)
        return Abst::abstract(0);


    long long val = one.d * oth.d;
    return Abst::abstract(val);
}

Abst operator/(const Abst &one, const Abst &oth) {
    UNDEF_CHECK(one, oth);

    if (oth.d == 0)
        // if division by zero, empty is returned
        return Abst::undef();

    if (one.isInf() && !oth.isInf())
        return one;

    long long val = one.d / oth.d;
    return Abst::abstract(val);

}

Abst operator%(const Abst &one, const Abst &oth) {
    UNDEF_CHECK(one, oth);

    if (one.isInf())
        return Abst::undef();

    if (oth.d == 0)
        return Abst::undef();

    long long val = one.d % oth.d;
    return Abst::abstract(val);
}

bool operator==(const Abst &one, const Abst &oth) {
    return one.d == oth.d;
}

bool operator<(const Abst &one, const Abst &oth) {
    return one.d < oth.d;
}

bool operator>(const Abst &one, const Abst &oth) {
    return one.d > oth.d;
}

bool operator<=(const Abst &one, const Abst &oth) {
    return one < oth || one == oth;
}

bool operator>=(const Abst &one, const Abst &oth) {
    return one > oth || one == oth;
}