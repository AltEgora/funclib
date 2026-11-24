#pragma once

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>

class TFunction {
public:
    std::function<double(double)> func;
    std::function<double(double)> deriv;
    std::string str;

    double operator()(double x) const {
        return func(x);
    }

    double GetDeriv(double x) const {
        return deriv(x);
    }

    std::string ToString() const {
        return str;
    }
};

using TFunctionPtr = std::shared_ptr<TFunction>;

std::string NumToString(double x);

TFunction operator+(const TFunction& a, const TFunction& b);
TFunction operator-(const TFunction& a, const TFunction& b);
TFunction operator*(const TFunction& a, const TFunction& b);
TFunction operator/(const TFunction& a, const TFunction& b);

template <typename T>
using NotFunction = std::enable_if_t<!std::is_base_of_v<TFunction, std::decay_t<T>>>;

template <typename T, typename = NotFunction<T>>
TFunction operator+(const TFunction&, const T&) { throw std::logic_error("unsupported type in operator +"); }

template <typename T, typename = NotFunction<T>>
TFunction operator+(const T&, const TFunction&) { throw std::logic_error("unsupported type in operator +"); }

template <typename T, typename = NotFunction<T>>
TFunction operator-(const TFunction&, const T&) { throw std::logic_error("unsupported type in operator -"); }

template <typename T, typename = NotFunction<T>>
TFunction operator-(const T&, const TFunction&) { throw std::logic_error("unsupported type in operator -"); }

template <typename T, typename = NotFunction<T>>
TFunction operator*(const TFunction&, const T&) { throw std::logic_error("unsupported type in operator *"); }

template <typename T, typename = NotFunction<T>>
TFunction operator*(const T&, const TFunction&) { throw std::logic_error("unsupported type in operator *"); }

template <typename T, typename = NotFunction<T>>
TFunction operator/(const TFunction&, const T&) { throw std::logic_error("unsupported type in operator /"); }

template <typename T, typename = NotFunction<T>>
TFunction operator/(const T&, const TFunction&) { throw std::logic_error("unsupported type in operator /"); }
