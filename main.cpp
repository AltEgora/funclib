#include <iostream>

#include "factory.h"

int main() {
    TFunctionFactory factory;

    auto x = factory.Create("ident");                    // x
    auto c = factory.Create("const", 2);                 // 2
    auto f = factory.Create("power", 2);                 // x^2
    auto e = factory.Create("exp");                      // e^x
    auto g = factory.Create("polynomial", {7, 0, 3, 15}); // 7 + 3x^2 + 15x^3

    std::cout << "Основные функции в точке 1:\n";
    for (const auto& fn : {x, c, f, e, g}) {
        std::cout << "  " << fn->ToString() << ": значение " << (*fn)(1)
                  << ", производная " << fn->GetDeriv(1) << "\n";
    }

    auto sum = *f + *g;
    auto diff = *e - *x;
    auto prod = *c * *f;
    auto quot = *g / *x;
    auto big = (*f + *g) * *e - *c / *x;

    std::cout << "\nВыражения в точке 1:\n";
    for (const auto& expr : {sum, diff, prod, quot, big}) {
        std::cout << "  " << expr.ToString() << ": значение " << expr(1)
                  << ", производная " << expr.GetDeriv(1) << "\n";
    }

}
