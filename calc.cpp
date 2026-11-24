#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "factory.h"

TFunction Apply(const TFunction& a, char op, const TFunction& b) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return a / b;
    }
    throw std::invalid_argument(std::string("unknown operation: ") + op);
}

int main() {
    TFunctionFactory factory;
    std::map<std::string, TFunction> funcs;

    std::cout << "Команды:\n"
                 "  f = power 2              создать функцию (ident, const C, power N, exp, polynomial a0 a1 ...)\n"
                 "  h = f + g                выражение из двух уже созданных (+ - * /)\n"
                 "  h 1.5                    значение и производная в точке\n"
                 "  list                     что уже создано\n"
                 "Пустая строка или Ctrl+D для выхода.\n";

    std::string line;
    while (std::cout << "\n> " && std::getline(std::cin, line) && !line.empty()) {
        std::istringstream in(line);
        std::string name, second;
        in >> name >> second;

        try {
            if (name == "list") {
                for (const auto& [n, f] : funcs) {
                    std::cout << "  " << n << " = " << f.ToString() << "\n";
                }
            } else if (second == "=") {
                std::string first;
                in >> first;
                std::string op, other;
                if (funcs.count(first) && in >> op >> other) {
                    if (op.size() != 1 || !funcs.count(other)) {
                        throw std::invalid_argument("expected: name = a op b, where a and b already exist");
                    }
                    funcs[name] = Apply(funcs.at(first), op[0], funcs.at(other));
                } else {
                    std::vector<double> params;
                    for (double v; in >> v;) {
                        params.push_back(v);
                    }
                    if (!in.eof()) {
                        throw std::invalid_argument("parameters must be numbers");
                    }
                    funcs[name] = *factory.Create(first, params);
                }
                std::cout << name << " = " << funcs[name].ToString() << "\n";
            } else if (funcs.count(name)) {
                double x = std::stod(second);
                const TFunction& f = funcs.at(name);
                std::cout << name << "(" << x << ") = " << f(x) << ", " << name << "'(" << x << ") = " << f.GetDeriv(x)
                          << "\n";
            } else {
                throw std::invalid_argument("no function named " + name);
            }
        } catch (const std::logic_error& err) {
            std::cout << "Ошибка: " << err.what() << "\n";
        }
    }
}
