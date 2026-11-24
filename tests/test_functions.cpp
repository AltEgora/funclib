#include <gtest/gtest.h>

#include <cmath>
#include <locale>

#include "functions.h"

TEST(Ident, ValueAndDeriv) {
    TIdent f({});
    EXPECT_DOUBLE_EQ(f(3.5), 3.5);
    EXPECT_DOUBLE_EQ(f.GetDeriv(3.5), 1);
    EXPECT_EQ(f.ToString(), "x");
}

TEST(Const, ValueAndDeriv) {
    TConst f({5});
    EXPECT_DOUBLE_EQ(f(-100), 5);
    EXPECT_DOUBLE_EQ(f.GetDeriv(-100), 0);
    EXPECT_EQ(f.ToString(), "5");
    EXPECT_EQ(TConst({2.5}).ToString(), "2.5");
    EXPECT_EQ(TConst({1234567}).ToString(), "1234567");
    EXPECT_EQ(TConst({0.1234567}).ToString(), "0.1234567");
}

TEST(Power, ValueAndDeriv) {
    TPower f({2});
    EXPECT_DOUBLE_EQ(f(10), 100);
    EXPECT_DOUBLE_EQ(f.GetDeriv(3), 6);
    EXPECT_EQ(f.ToString(), "x^2");
}

TEST(Power, FractionalAndNegative) {
    EXPECT_DOUBLE_EQ(TPower({0.5})(9), 3);
    EXPECT_DOUBLE_EQ(TPower({-1})(4), 0.25);
    EXPECT_DOUBLE_EQ(TPower({-1}).GetDeriv(2), -0.25);
    EXPECT_EQ(TPower({0.5}).ToString(), "x^0.5");
    EXPECT_TRUE(std::isnan(TPower({0.5})(-1)));
}

TEST(Power, ZeroExponent) {
    TPower f({0});
    EXPECT_DOUBLE_EQ(f(0), 1);
    EXPECT_DOUBLE_EQ(f.GetDeriv(0), 0);
}

TEST(Exp, ValueAndDeriv) {
    TExp f({});
    EXPECT_DOUBLE_EQ(f(0), 1);
    EXPECT_DOUBLE_EQ(f(1), std::exp(1.0));
    EXPECT_DOUBLE_EQ(f.GetDeriv(2), std::exp(2.0));
    EXPECT_EQ(f.ToString(), "exp(x)");
}

TEST(Polynomial, ExampleFromTask) {
    TPolynomial f({7, 0, 3, 15});
    EXPECT_DOUBLE_EQ(f(10), 15307);
    EXPECT_DOUBLE_EQ(f(0), 7);
    EXPECT_DOUBLE_EQ(f.GetDeriv(1), 51);
    EXPECT_EQ(f.ToString(), "7 + 3*x^2 + 15*x^3");
}

TEST(Polynomial, ToStringSignsAndOnes) {
    EXPECT_EQ(TPolynomial({0, 1}).ToString(), "x");
    EXPECT_EQ(TPolynomial({-1, -1, 1}).ToString(), "-1 - x + x^2");
    EXPECT_EQ(TPolynomial({0, 0, -2.5}).ToString(), "-2.5*x^2");
    EXPECT_EQ(TPolynomial({0, 0}).ToString(), "0");
}

TEST(Polynomial, ConstantOnly) {
    TPolynomial f({4});
    EXPECT_DOUBLE_EQ(f(123), 4);
    EXPECT_DOUBLE_EQ(f.GetDeriv(123), 0);
}

TEST(BadParams, WrongCount) {
    EXPECT_THROW(TIdent({1}), std::invalid_argument);
    EXPECT_THROW(TConst({}), std::invalid_argument);
    EXPECT_THROW(TConst({1, 2}), std::invalid_argument);
    EXPECT_THROW(TPower({}), std::invalid_argument);
    EXPECT_THROW(TExp({1}), std::invalid_argument);
    EXPECT_THROW(TPolynomial({}), std::invalid_argument);
}

TEST(DerivedTypes, OperatorsWork) {
    TPower a({2});
    TConst b({1});
    auto s = a + b;
    EXPECT_DOUBLE_EQ(s(3), 10);
    EXPECT_NO_THROW(a * b);
    EXPECT_NO_THROW(b / a);
    EXPECT_THROW(a + 1.0, std::logic_error);
}

namespace {
struct CommaDecimal : std::numpunct<char> {
    char do_decimal_point() const override { return ','; }
    std::string do_grouping() const override { return "\3"; }
    char do_thousands_sep() const override { return ' '; }
};
}

TEST(NumToString, IgnoresGlobalLocale) {
    std::locale old = std::locale::global(std::locale(std::locale::classic(), new CommaDecimal));
    std::string a = TConst({2.5}).ToString();
    std::string b = TConst({1234}).ToString();
    std::locale::global(old);
    EXPECT_EQ(a, "2.5");
    EXPECT_EQ(b, "1234");
}

TEST(NumToString, NegativeAndSmall) {
    EXPECT_EQ(TConst({-5}).ToString(), "-5");
    EXPECT_EQ(TConst({1e-7}).ToString(), "1e-07");
    EXPECT_EQ(TPower({-1}).ToString(), "x^(-1)");
    EXPECT_EQ(TPower({-0.5}).ToString(), "x^(-0.5)");
}
