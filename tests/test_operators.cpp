#include <gtest/gtest.h>

#include <cmath>
#include <string>
#include <vector>

#include "factory.h"

class Operators : public ::testing::Test {
protected:
    TFunctionFactory factory;
    TFunctionPtr f = factory.Create("power", 2);
    TFunctionPtr g = factory.Create("polynomial", {7, 0, 3, 15});
    TFunctionPtr x = factory.Create("ident");
    TFunctionPtr c = factory.Create("const", 3);
};

TEST_F(Operators, ExampleFromTask) {
    auto p = *f + *g;
    EXPECT_DOUBLE_EQ(p(1), 26);
    EXPECT_DOUBLE_EQ(p.GetDeriv(1), 53);
}

TEST_F(Operators, Sub) {
    auto p = *g - *f;
    EXPECT_DOUBLE_EQ(p(2), 7 + 12 + 120 - 4);
    EXPECT_DOUBLE_EQ(p.GetDeriv(2), 12 + 180 - 4);
}

TEST_F(Operators, Mul) {
    auto p = *f * *c;
    EXPECT_DOUBLE_EQ(p(2), 12);
    EXPECT_DOUBLE_EQ(p.GetDeriv(2), 12);

    auto q = *x * *x;
    EXPECT_DOUBLE_EQ(q.GetDeriv(5), 10);
}

TEST_F(Operators, Div) {
    auto p = *c / *x;
    EXPECT_DOUBLE_EQ(p(2), 1.5);
    EXPECT_DOUBLE_EQ(p.GetDeriv(2), -0.75);

    TFunction q = p;
    EXPECT_DOUBLE_EQ(q(2), 1.5);
    EXPECT_DOUBLE_EQ(q.GetDeriv(2), -0.75);
}

TEST_F(Operators, DivByZero) {
    auto p = *c / *x;
    EXPECT_TRUE(std::isinf(p(0)));
}

TEST_F(Operators, ToString) {
    EXPECT_EQ((*f + *g).ToString(), "(x^2 + (7 + 3*x^2 + 15*x^3))");
    EXPECT_EQ((*f - *c).ToString(), "(x^2 - 3)");
    EXPECT_EQ(((*x + *c) * *f).ToString(), "((x + 3) * x^2)");
    EXPECT_EQ((*c / *x).ToString(), "(3 / x)");
}

TEST_F(Operators, ToStringKeepsPolynomialTogether) {
    EXPECT_EQ((*f * *g).ToString(), "(x^2 * (7 + 3*x^2 + 15*x^3))");
    EXPECT_EQ((*c - *g).ToString(), "(3 - (7 + 3*x^2 + 15*x^3))");
    EXPECT_EQ((*g / *g).ToString(), "((7 + 3*x^2 + 15*x^3) / (7 + 3*x^2 + 15*x^3))");
}

TEST_F(Operators, ToStringNestedNotDoubleWrapped) {
    auto s = *f + *c;
    EXPECT_EQ((s * s).ToString(), "((x^2 + 3) * (x^2 + 3))");
    EXPECT_EQ(((s * s) - *x).ToString(), "(((x^2 + 3) * (x^2 + 3)) - x)");
}

TEST_F(Operators, NestedExpression) {
    auto e = factory.Create("exp");
    auto p = (*f + *c) * *e / *x;
    double v = 1.5;
    EXPECT_NEAR(p(v), (v * v + 3) * std::exp(v) / v, 1e-12);

    double h = 1e-6;
    double numeric = (p(v + h) - p(v - h)) / (2 * h);
    EXPECT_NEAR(p.GetDeriv(v), numeric, 1e-5);
}

TEST_F(Operators, ExpressionOutlivesOperands) {
    auto p = *f + *c;
    f.reset();
    c.reset();
    EXPECT_DOUBLE_EQ(p(2), 7);
}

TEST_F(Operators, ExpressionDoesNotSeeOperandChanges) {
    auto p = *f + *c;
    *f = *factory.Create("power", 3);
    c->func = [](double) { return 100.0; };
    c->deriv = [](double) { return 1.0; };
    c->str = "100";

    EXPECT_DOUBLE_EQ((*f)(2), 8);
    EXPECT_DOUBLE_EQ((*c)(2), 100);
    EXPECT_DOUBLE_EQ(p(2), 7);
    EXPECT_DOUBLE_EQ(p.GetDeriv(2), 4);
    EXPECT_EQ(p.ToString(), "(x^2 + 3)");
}

TEST_F(Operators, CopyAndAssign) {
    auto p = *f + *c;
    TFunction q = p;
    p = p * *x;
    EXPECT_DOUBLE_EQ(q(2), 7);
    EXPECT_DOUBLE_EQ(p(2), 14);
    EXPECT_EQ(q.ToString(), "(x^2 + 3)");
    EXPECT_EQ(p.ToString(), "((x^2 + 3) * x)");
}

TEST_F(Operators, UnsupportedTypes) {
    EXPECT_THROW(*f + "abc", std::logic_error);
    EXPECT_THROW("abc" + *f, std::logic_error);
    EXPECT_THROW(*f - 2, std::logic_error);
    EXPECT_THROW(2.0 - *f, std::logic_error);
    EXPECT_THROW(*f * 2.0, std::logic_error);
    EXPECT_THROW(*f / std::string("x"), std::logic_error);
    EXPECT_THROW(*f + g, std::logic_error);
}

TEST_F(Operators, UnsupportedTypesBothSides) {
    EXPECT_THROW(*f - "abc", std::logic_error);
    EXPECT_THROW("abc" - *f, std::logic_error);
    EXPECT_THROW(*f * "abc", std::logic_error);
    EXPECT_THROW(2.0 * *f, std::logic_error);
    EXPECT_THROW(*f / 2, std::logic_error);
    EXPECT_THROW("x" / *f, std::logic_error);
    EXPECT_THROW(*f + nullptr, std::logic_error);
    EXPECT_THROW((std::vector<double>{1, 2} * *f), std::logic_error);
}

TEST_F(Operators, NegativeConstInString) {
    auto m = factory.Create("const", -5);
    EXPECT_EQ((*f - *m).ToString(), "(x^2 - (-5))");
    EXPECT_EQ((*m * *x).ToString(), "((-5) * x)");
    auto neg = factory.Create("polynomial", {0, -1});
    EXPECT_EQ(neg->ToString(), "-x");
    EXPECT_EQ((*c + *neg).ToString(), "(3 + (-x))");
    EXPECT_DOUBLE_EQ((*f - *m)(1), 6);
}
