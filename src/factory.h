#pragma once

#include <map>
#include <string>
#include <vector>

#include "function.h"

class TFunctionFactory {
public:
    using Creator = std::function<TFunctionPtr(const std::vector<double>&)>;

    TFunctionFactory();

    void Register(const std::string& type, Creator creator);

    TFunctionPtr Create(const std::string& type) const;
    TFunctionPtr Create(const std::string& type, double param) const;
    TFunctionPtr Create(const std::string& type, std::vector<double> params) const;

private:
    std::map<std::string, Creator> creators;
};
