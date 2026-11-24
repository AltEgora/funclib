#pragma once

#include <vector>

#include "function.h"

class TIdent : public TFunction {
public:
    explicit TIdent(const std::vector<double>& params);
};

class TConst : public TFunction {
public:
    explicit TConst(const std::vector<double>& params);
};

class TPower : public TFunction {
public:
    explicit TPower(const std::vector<double>& params);
};

class TExp : public TFunction {
public:
    explicit TExp(const std::vector<double>& params);
};

class TPolynomial : public TFunction {
public:
    explicit TPolynomial(const std::vector<double>& params);
};
