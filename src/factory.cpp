#include "factory.h"

#include "functions.h"

TFunctionFactory::TFunctionFactory() {
    Register("ident", [](const std::vector<double>& p) { return std::make_shared<TIdent>(p); });
    Register("const", [](const std::vector<double>& p) { return std::make_shared<TConst>(p); });
    Register("power", [](const std::vector<double>& p) { return std::make_shared<TPower>(p); });
    Register("exp", [](const std::vector<double>& p) { return std::make_shared<TExp>(p); });
    Register("polynomial", [](const std::vector<double>& p) { return std::make_shared<TPolynomial>(p); });
}

void TFunctionFactory::Register(const std::string& type, Creator creator) {
    if (creators.count(type)) {
        throw std::invalid_argument("function type already registered: " + type);
    }
    creators[type] = creator;
}

TFunctionPtr TFunctionFactory::Create(const std::string& type) const {
    return Create(type, std::vector<double>{});
}

TFunctionPtr TFunctionFactory::Create(const std::string& type, double param) const {
    return Create(type, std::vector<double>{param});
}

TFunctionPtr TFunctionFactory::Create(const std::string& type, std::vector<double> params) const {
    auto it = creators.find(type);
    if (it == creators.end()) {
        throw std::invalid_argument("unknown function type: " + type);
    }
    return it->second(params);
}
