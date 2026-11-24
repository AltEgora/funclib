#include <gtest/gtest.h>

#include <cmath>
#include <vector>

#include "factory.h"

TEST(Factory, CreatesAllTypes) {
    TFunctionFactory factory;
    EXPECT_DOUBLE_EQ((*factory.Create("ident"))(3), 3);
    EXPECT_DOUBLE_EQ((*factory.Create("const", 7))(3), 7);
    EXPECT_DOUBLE_EQ((*factory.Create("power", 2))(3), 9);
    EXPECT_DOUBLE_EQ((*factory.Create("exp"))(0), 1);
    EXPECT_DOUBLE_EQ((*factory.Create("polynomial", {1, 2}))(3), 7);
}

TEST(Factory, ExampleFromTask) {
    TFunctionFactory factory;
    std::vector<TFunctionPtr> cont;
    cont.push_back(factory.Create("power", 2));
    cont.push_back(factory.Create("polynomial", {7, 0, 3, 15}));

    EXPECT_EQ(cont[0]->ToString(), "x^2");
    EXPECT_DOUBLE_EQ((*cont[0])(10), 100);
    EXPECT_EQ(cont[1]->ToString(), "7 + 3*x^2 + 15*x^3");
    EXPECT_DOUBLE_EQ((*cont[1])(10), 15307);
}

TEST(Factory, UnknownType) {
    TFunctionFactory factory;
    EXPECT_THROW(factory.Create("sin"), std::invalid_argument);
    EXPECT_THROW(factory.Create(""), std::invalid_argument);
    EXPECT_THROW(factory.Create("Power", 2), std::invalid_argument);
}

TEST(Factory, WrongParams) {
    TFunctionFactory factory;
    EXPECT_THROW(factory.Create("power"), std::invalid_argument);
    EXPECT_THROW(factory.Create("const"), std::invalid_argument);
    EXPECT_THROW(factory.Create("ident", 1), std::invalid_argument);
    EXPECT_THROW(factory.Create("polynomial", std::vector<double>{}), std::invalid_argument);
}

TEST(Factory, PolynomialFromSingleNumber) {
    TFunctionFactory factory;
    auto p = factory.Create("polynomial", 5);
    EXPECT_DOUBLE_EQ((*p)(100), 5);
    EXPECT_EQ(p->ToString(), "5");
}

TEST(Factory, RegisterExistingNameThrows) {
    TFunctionFactory factory;
    auto creator = [](const std::vector<double>&) { return std::make_shared<TFunction>(); };
    EXPECT_THROW(factory.Register("const", creator), std::invalid_argument);
    EXPECT_THROW(factory.Register("power", creator), std::invalid_argument);
    EXPECT_DOUBLE_EQ((*factory.Create("power", 2))(3), 9);
}

namespace {
class TSin : public TFunction {
public:
    explicit TSin(const std::vector<double>& params) {
        if (!params.empty()) {
            throw std::invalid_argument("sin expects 0 parameter(s)");
        }
        func = [](double x) { return std::sin(x); };
        deriv = [](double x) { return std::cos(x); };
        str = "sin(x)";
    }
};
}

TEST(Factory, RegisterNewType) {
    TFunctionFactory factory;
    factory.Register("sin", [](const std::vector<double>& p) { return std::make_shared<TSin>(p); });

    auto f = factory.Create("sin");
    EXPECT_DOUBLE_EQ((*f)(0), 0);
    EXPECT_DOUBLE_EQ(f->GetDeriv(0), 1);
    EXPECT_EQ(f->ToString(), "sin(x)");
    EXPECT_THROW(factory.Create("sin", 1), std::invalid_argument);

    auto sq = factory.Create("power", 2);
    auto g = *f * *sq;
    double v = 1.3;
    EXPECT_NEAR(g(v), std::sin(v) * v * v, 1e-12);
    EXPECT_NEAR(g.GetDeriv(v), std::cos(v) * v * v + 2 * v * std::sin(v), 1e-12);
    EXPECT_EQ(g.ToString(), "(sin(x) * x^2)");
}
