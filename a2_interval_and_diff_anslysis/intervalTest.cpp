#include "interval.h"
#include "abst.h"
#include <iostream>
#include <climits>
#include <vector>
#include <utility>

int main() {
    Abst::MIN = -2000;
    Abst::MAX = 2000;
    std::vector<std::pair<Interval, Interval>> cases = {
            {{5, 10}, {10, 15}}, {{5, 10}, {10, 199}},
            {{-5, -10}, {-15, -10}}, {{-10, -5}, {-199, -10}},
            {{-5, 10}, {2, 3}}, {{2, 3}, {-5, 10}},
            {{Abst::undef(), 2}, {4, 5}}, {{4, 5}, {Abst::undef(), 2}},
            {{30, Abst::inf()}, {4, 5}}, {{-Abst::inf(), 20}, {4, 5}},
            {{-20, 3}, {-13, 5}}, {{-199, -10}, {-10, -5}}
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

    std::cout << std::endl;
    std::cout << "=== union ===" << std::endl;
    for (const auto &[a, b]: cases) {
        std::cout << a << " union " << b << " is " << a.unionWith(b) << std::endl;
    }

    return 0;

    return 0;
}