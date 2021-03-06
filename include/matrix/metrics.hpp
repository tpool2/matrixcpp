#pragma once

#include "products.hpp"

namespace math
{

template <typename T>
double norm(const DynamicVector<T>& vector)
{
    return sqrt(dot(vector,vector));
}

template <typename T, int N>
double norm(const StaticVector<T,N>& vector)
{
    return sqrt(static_cast<double>(dot(vector,vector)));
}
}