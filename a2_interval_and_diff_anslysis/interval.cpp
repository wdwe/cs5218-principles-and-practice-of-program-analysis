//
// Created by wayne on 3/20/24.
//
#include <cassert>
#include "interval.h"
#include "abst.h"
#include <algorithm>
#include <cmath>


/*
 * Interval
 */

#define EMPTY_CHECK(one, other)                         \
        if ((one).isEmpty() || (other).isEmpty())       \
        return Interval::empty()

Interval::Interval(Abst c): l(c), h(c) {}

Interval::Interval(Abst l, Abst h): l(l), h(h) {
    if (l > h) {
        std::cerr << "warning: l(" << l << ") > h" << "(" << h << ") in Interval Constructor" << std::endl;
        this->l = Abst::undef();
        this->h = Abst::undef();
    }
}

bool Interval::isEmpty() const {
    return l.isUndef() or h.isUndef();
}

Interval Interval::unionWith(const Interval &other) const {
    if (this->isEmpty())
        return other;
    if (other.isEmpty())
        return *this;
    return {std::min(this->l, other.l), std::max(this->h, other.h)};
}

Abst Interval::sep(const Interval& other) const {
    return std::max(Abst::abs(this->h - other.l), Abst::abs(other.h - this->l));
}


std::string Interval::repr() const {
    if (l.isUndef() || h.isUndef())
        return "[empty set]";

    std::string s = "[" + l.repr() + ", " + h.repr() + "]";
    return s;
}

Interval Interval::empty() {
    return {Abst::undef(), Abst::undef()};
}


std::pair<Interval, Interval> Interval::eqTransfer(const Interval &left, const Interval &right) {
    if (left.l > right.h || right.l > left.h) {
        return {Interval::empty(), Interval::empty()};
    }
    Interval resLeft = {std::max(left.l, right.l), std::min(left.h, right.h)};
    Interval resRight = resLeft;
    return {resLeft, resRight};
}

std::pair<Interval, Interval> Interval::neTransfer(const Interval &left, const Interval &right) {
    if (left.l == left.h && right.l == right.h && left.l == right.l)
        return {Interval::empty(), Interval::empty()};
    return {left, right};
}

std::pair<Interval, Interval> Interval::ltTransfer(const Interval &left, const Interval &right) {
    if (left.l >= right.h)
        return {Interval::empty(), Interval::empty()};
    Interval resLeft = {left.l, std::min(left.h, right.h - 1)};
    Interval resRight = {std::max(left.l + 1, right.l), right.h};
    return {resLeft, resRight};
}

std::pair<Interval, Interval> Interval::leTransfer(const Interval &left, const Interval &right) {
    if (left.l > right.h)
        return {Interval::empty(), Interval::empty()};
    Interval resLeft = {left.l, std::min(left.h, right.h)};
    Interval resRight = {std::max(left.l, right.l), right.h};
    return {resLeft, resRight};
}

std::pair<Interval, Interval> Interval::gtTransfer(const Interval &left, const Interval &right) {
    if (right.l >= left.h)
        return {Interval::empty(), Interval::empty()};
    Interval resLeft = {std::max(left.l, right.l + 1), left.h};
    Interval resRight = {right.l, std::min(left.h - 1, right.h)};
    return {resLeft, resRight};

}

std::pair<Interval, Interval> Interval::geTransfer(const Interval &left, const Interval &right) {
    if (right.l > left.h)
        return {Interval::empty(), Interval::empty()};
    Interval resLeft = {std::max(left.l, right.l), left.h};
    Interval resRight = {right.l, std::min(left.h, right.h)};
    return {resLeft, resRight};
}



std::ostream& operator<<(std::ostream &out, const Interval& interv) {
    out << interv.repr();
    return out;
}

llvm::raw_ostream& operator<<(llvm::raw_ostream& out, const Interval& interv) {
    out << interv.repr();
    return out;
}


Interval operator-(const Interval &one) {
    if (one.isEmpty())
        return Interval::empty();
    return {-one.h, -one.l};
}

Interval operator+(const Interval &one, const Interval &other) {
    EMPTY_CHECK(one, other);
    return {one.l + other.l, one.h + other.h};
}

Interval operator-(const Interval &one, const Interval &other) {
    EMPTY_CHECK(one, other);

    return {one.l - other.h, one.h - other.l};
}

Interval operator*(const Interval &one, const Interval &other) {
    EMPTY_CHECK(one, other);

    Abst ll = one.l * other.l, lh = one.l * other.h,
            hl = one.h * other.l, hh = one.h * other.h;
    return {std::min({ll, lh, hl, hh}), std::max({ll, lh, hl, hh})};
}

Interval Interval::posDivision(const Interval a, const Interval b)  {
    assert((other.l > 0) && "range for b must be strictly positive");
    Abst ll = a.l / b.l, lh = a.l / b.h,
            hl = a.h / b.l, hh = a.h / b.h;
    return {std::min({ll, lh, hl, hh}), std::max({ll, lh, hl, hh})};
}

Interval operator/(const Interval &one, const Interval &other) {
    EMPTY_CHECK(one, other);

    if (other.l == 0 || other.h == 0)
        return Interval::empty();

    if (other.l > 0)
        return Interval::posDivision(one, other);
    if (other.h < 0)
        return Interval::posDivision(-one, -other);
    if (other.l == 0)
        return one / Interval(1, other.h);
    if (other.h == 0)
        return one / Interval(other.l, -1);
    Interval neg = one / Interval(other.l, -1);
    Interval pos = one / Interval(1, other.h);
    return neg.unionWith(pos);
}


Interval operator%(const Interval &one, const Interval &other) {
    EMPTY_CHECK(one, other);
    Abst m = std::max(Abst::abs(other.l) - 1, Abst::abs(other.h) - 1);
    if (one.l >= 0)
        return {0, std::min(one.h, m)};
    if (one.h <= 0)
        return {-std::min(-one.l, m), 0};
    return {-std::min(-one.l, m), std::min(one.h, m)};
}

bool operator==(const Interval &one, const Interval &other) {
    if (one.l == other.l && one.h == other.h)
        return true;
    return false;
}

bool operator!=(const Interval &one, const Interval &other) {
    return !(one == other);
}