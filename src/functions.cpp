#include "functions.h"

#include <cmath>

static void CheckCount(const std::vector<double>& params, size_t count, const std::string& name) {
    if (params.size() != count) {
        throw std::invalid_argument(name + " expects " + std::to_string(count) + " parameter(s), got " +
                                    std::to_string(params.size()));
    }
}

TIdent::TIdent(const std::vector<double>& params) {
    CheckCount(params, 0, "ident");
    func = [](double x) { return x; };
    deriv = [](double) { return 1.0; };
    str = "x";
}

TConst::TConst(const std::vector<double>& params) {
    CheckCount(params, 1, "const");
    double c = params[0];
    func = [c](double) { return c; };
    deriv = [](double) { return 0.0; };
    str = NumToString(c);
}

TPower::TPower(const std::vector<double>& params) {
    CheckCount(params, 1, "power");
    double n = params[0];
    func = [n](double x) { return std::pow(x, n); };
    if (n == 0) {
        deriv = [](double) { return 0.0; };
    } else {
        deriv = [n](double x) { return n * std::pow(x, n - 1); };
    }
    str = n < 0 ? "x^(" + NumToString(n) + ")" : "x^" + NumToString(n);
}

TExp::TExp(const std::vector<double>& params) {
    CheckCount(params, 0, "exp");
    func = [](double x) { return std::exp(x); };
    deriv = [](double x) { return std::exp(x); };
    str = "exp(x)";
}

TPolynomial::TPolynomial(const std::vector<double>& params) {
    if (params.empty()) {
        throw std::invalid_argument("polynomial needs at least one coefficient");
    }

    std::vector<double> c = params;
    func = [c](double x) {
        double res = 0;
        for (size_t i = c.size(); i > 0; --i) {
            res = res * x + c[i - 1];
        }
        return res;
    };
    deriv = [c](double x) {
        double res = 0;
        for (size_t i = c.size() - 1; i > 0; --i) {
            res = res * x + static_cast<double>(i) * c[i];
        }
        return res;
    };

    str = "";
    for (size_t i = 0; i < c.size(); ++i) {
        if (c[i] == 0) {
            continue;
        }

        if (str.empty()) {
            str = c[i] < 0 ? "-" : "";
        } else {
            str += c[i] < 0 ? " - " : " + ";
        }

        double abs_c = std::abs(c[i]);
        if (i == 0) {
            str += NumToString(abs_c);
            continue;
        }
        if (abs_c != 1) {
            str += NumToString(abs_c) + "*";
        }
        str += i == 1 ? "x" : "x^" + std::to_string(i);
    }
    if (str.empty()) {
        str = "0";
    }
}
