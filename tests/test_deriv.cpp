#include <gtest/gtest.h>

#include <cmath>

#include "factory.h"

class Deriv : public ::testing::Test {
protected:
    TFunctionFactory factory;
    TFunctionPtr x = factory.Create("ident");
};

TEST_F(Deriv, ExampleFromTask) {
    auto f = factory.Create("power", 2);
    auto g = factory.Create("polynomial", {7, 0, 3, 15});
    auto p = *f + *g;
    EXPECT_DOUBLE_EQ(p.GetDeriv(1), 53);
    EXPECT_DOUBLE_EQ(f->GetDeriv(3), 6);
    EXPECT_DOUBLE_EQ(g->GetDeriv(3), 423);
}

TEST_F(Deriv, BasicFunctions) {
    EXPECT_DOUBLE_EQ(x->GetDeriv(-4), 1);
    EXPECT_DOUBLE_EQ(factory.Create("const", 8)->GetDeriv(2), 0);
    EXPECT_DOUBLE_EQ(factory.Create("power", 3)->GetDeriv(2), 12);
    EXPECT_DOUBLE_EQ(factory.Create("exp")->GetDeriv(1), std::exp(1.0));
}

TEST_F(Deriv, ProductAndQuotientRules) {
    auto e = factory.Create("exp");
    auto sq = factory.Create("power", 2);

    EXPECT_NEAR((*sq * *e).GetDeriv(2), 8 * std::exp(2.0), 1e-12);
    EXPECT_NEAR((*e / *x).GetDeriv(2), std::exp(2.0) / 4, 1e-12);
    EXPECT_DOUBLE_EQ((*sq - *x).GetDeriv(5), 9);
}

TEST_F(Deriv, MatchesNumericOnNestedExpression) {
    auto c = factory.Create("const", 3);
    auto p = factory.Create("polynomial", {1, -2, 0, 0.5});
    auto e = factory.Create("exp");
    auto expr = (*p * *e - *c) / (*x + *c);

    for (double v : {-2.0, 0.0, 0.7, 2.5}) {
        double h = 1e-6;
        double numeric = (expr(v + h) - expr(v - h)) / (2 * h);
        EXPECT_NEAR(expr.GetDeriv(v), numeric, 1e-5) << "x = " << v;
    }
}

TEST_F(Deriv, SingularPoints) {
    EXPECT_TRUE(std::isinf(factory.Create("power", 0.5)->GetDeriv(0)));
    auto c = factory.Create("const", 1);
    EXPECT_FALSE(std::isfinite((*c / *x).GetDeriv(0)));
}

TEST_F(Deriv, LongExpressionIsCheap) {
    TFunction sum = *x;
    for (int i = 1; i < 1000; ++i) {
        sum = sum + *x;
    }
    EXPECT_DOUBLE_EQ(sum(2), 2000);
    EXPECT_DOUBLE_EQ(sum.GetDeriv(2), 1000);
}
