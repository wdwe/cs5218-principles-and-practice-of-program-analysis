#include "abst.h"
#include <iostream>
#include <climits>
#include <algorithm>
#include <vector>
#include <utility>

int main() {
//    Abst::MIN = -20;
//    Abst::MAX = 20;


    std::vector<std::pair<Abst, Abst>> cases = {
            {1, 2}, {15, 20}, {15, 21},
            {-1, -2}, {-15, -20}, {-15, -21},
            {-1, 2}, {-15, 20}, {-15, 21},
            {10, 2}, {150, 20}, {150, 21},
            {-10, -2}, {-150, -20}, {-150, -21},
            {-10, 2}, {-150, 20}, {-150, 21},
            {Abst::inf(), 15}, {15, Abst::negInf()},
            {Abst::negInf(), Abst::inf()}, {Abst::inf(), Abst::negInf()},
            {14, 0}, {0, 14}, {Abst::negInf(), 0},
            {Abst::inf(), 0}, {0, Abst::negInf()}, {Abst::undef(), 0},
            {Abst::negInf(), Abst::undef()}
    };
    std::cout << "=== negation ===" << std::endl;
    for (const auto &[a, b]: cases) {
        std::cout << "-" << a << " is " << -a << std::endl;
        std::cout << "-" << b << " is " << -b << std::endl;
    }

    std::cout << std::endl;
    std::cout << "=== addition ===" << std::endl;
    for (const auto &[a, b]: cases) {
        std::cout << a << " + " << b << " is " << a + b << std::endl;
    }

    std::cout << std::endl;
    std::cout << "=== subtraction ===" << std::endl;
    for (const auto &[a, b]: cases) {
        std::cout << a << " - " << b << " is " << a - b << std::endl;
    }

    std::cout << std::endl;
    std::cout << "=== multiplication ===" << std::endl;
    for (const auto &[a, b]: cases) {
        std::cout << a << " * " << b << " is " << a * b << std::endl;
    }

    std::cout << std::endl;
    std::cout << "=== division ===" << std::endl;
    for (const auto &[a, b]: cases) {
        std::cout << a << " / " << b << " is " << a / b << std::endl;
    }

    std::cout << std::endl;
    std::cout << "=== modulo ===" << std::endl;
    for (const auto &[a, b]: cases) {
        std::cout << a << " % " << b << " is " << a % b << std::endl;
    }

    return 0;
}