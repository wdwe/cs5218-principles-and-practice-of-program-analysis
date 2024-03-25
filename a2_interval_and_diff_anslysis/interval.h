//
// Created by wayne on 3/20/24.
//

#ifndef A2_INTERVAL_H
#define A2_INTERVAL_H
#include <iostream>
#include <climits>
#include <string>
#include "abst.h"


class Interval {
    friend Interval operator-(const Interval &one);
    friend Interval operator+(const Interval &one, const Interval &other);
    friend Interval operator-(const Interval &one, const Interval &other);
    friend Interval operator*(const Interval &one, const Interval &other);
    friend Interval operator/(const Interval &one, const Interval &other);
    friend Interval operator%(const Interval &one, const Interval &other);
    friend bool operator==(const Interval &one, const Interval &other);
    friend bool operator!=(const Interval &one, const Interval &other);

public:
    explicit Interval(Abst c);
    Interval() = default;
    Interval(Abst l, Abst h);
    [[nodiscard]] bool isEmpty() const;
    [[nodiscard]] Interval unionWith(const Interval &other) const;
    [[nodiscard]] std::string repr() const;
    [[nodiscard]] Abst sep(const Interval& other) const;

    static Interval empty();
    static std::pair<Interval, Interval> eqTransfer(const Interval &left, const Interval &right);
    static std::pair<Interval, Interval> neTransfer(const Interval &left, const Interval &right);
    static std::pair<Interval, Interval> ltTransfer(const Interval &left, const Interval &right);
    static std::pair<Interval, Interval> leTransfer(const Interval &left, const Interval &right);
    static std::pair<Interval, Interval> gtTransfer(const Interval &left, const Interval &right);
    static std::pair<Interval, Interval> geTransfer(const Interval &left, const Interval &right);


private:
    Abst l = Abst::undef(), h = Abst::undef();
    static Interval posDivision(Interval a, Interval b);
};

std::ostream& operator<<(std::ostream &out, const Interval &interv);
llvm::raw_ostream& operator<<(llvm::raw_ostream& out, const Interval& interv);

Interval operator-(const Interval &one);
Interval operator+(const Interval &one, const Interval &other);
Interval operator-(const Interval &one, const Interval &other);
Interval operator*(const Interval &one, const Interval &other);
Interval operator/(const Interval &one, const Interval &other);
Interval operator%(const Interval &one, const Interval &other);
bool operator==(const Interval &one, const Interval &other);
bool operator!=(const Interval &one, const Interval &other);

#endif //A2_INTERVAL_H
