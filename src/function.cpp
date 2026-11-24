#include "function.h"

#include <locale>
#include <sstream>

std::string NumToString(double x) {
    std::ostringstream out;
    out.precision(15);
    out.imbue(std::locale::classic());
    out << x;
    return out.str();
}

static bool IsWrapped(const std::string& s) {
    int depth = 0;
    for (size_t i = 0; i + 1 < s.size(); ++i) {
        depth += s[i] == '(' ? 1 : s[i] == ')' ? -1 : 0;
        if (depth == 0) {
            return false;
        }
    }
    return s.size() > 1 && s.front() == '(' && s.back() == ')';
}

static std::string Operand(const TFunction& f) {
    std::string s = f.ToString();
    bool simple = s.find(' ') == std::string::npos && !s.starts_with('-');
    return simple || IsWrapped(s) ? s : "(" + s + ")";
}

TFunction operator+(const TFunction& a, const TFunction& b) {
    auto pa = std::make_shared<const TFunction>(a);
    auto pb = std::make_shared<const TFunction>(b);
    TFunction res;
    res.func = [pa, pb](double x) { return (*pa)(x) + (*pb)(x); };
    res.deriv = [pa, pb](double x) { return pa->GetDeriv(x) + pb->GetDeriv(x); };
    res.str = "(" + Operand(a) + " + " + Operand(b) + ")";
    return res;
}

TFunction operator-(const TFunction& a, const TFunction& b) {
    auto pa = std::make_shared<const TFunction>(a);
    auto pb = std::make_shared<const TFunction>(b);
    TFunction res;
    res.func = [pa, pb](double x) { return (*pa)(x) - (*pb)(x); };
    res.deriv = [pa, pb](double x) { return pa->GetDeriv(x) - pb->GetDeriv(x); };
    res.str = "(" + Operand(a) + " - " + Operand(b) + ")";
    return res;
}

TFunction operator*(const TFunction& a, const TFunction& b) {
    auto pa = std::make_shared<const TFunction>(a);
    auto pb = std::make_shared<const TFunction>(b);
    TFunction res;
    res.func = [pa, pb](double x) { return (*pa)(x) * (*pb)(x); };
    res.deriv = [pa, pb](double x) { return pa->GetDeriv(x) * (*pb)(x) + (*pa)(x) * pb->GetDeriv(x); };
    res.str = "(" + Operand(a) + " * " + Operand(b) + ")";
    return res;
}

TFunction operator/(const TFunction& a, const TFunction& b) {
    auto pa = std::make_shared<const TFunction>(a);
    auto pb = std::make_shared<const TFunction>(b);
    TFunction res;
    res.func = [pa, pb](double x) { return (*pa)(x) / (*pb)(x); };
    res.deriv = [pa, pb](double x) {
        double bx = (*pb)(x);
        return (pa->GetDeriv(x) * bx - (*pa)(x) * pb->GetDeriv(x)) / (bx * bx);
    };
    res.str = "(" + Operand(a) + " / " + Operand(b) + ")";
    return res;
}
