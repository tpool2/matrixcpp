#pragma once

#include "base.hpp"

#include <initializer_list>

namespace math
{

template <typename T, int Dim>
requires(Dim > 0)
class Array<T, true, Dim>
{
    private:
        T data_[Dim];

        void check_input(int index) const
        {
            if(index >= Dim || index < 0)
            {
                throw OutOfRange(index, Dim);
            }
        }

        int transform_index(int index) const
        {
            int transformed_index = index;
            while(transformed_index < 0)
            {
                transformed_index += Dim;
            }
            return transformed_index;
        }

    public:
        using InitializerList = std::initializer_list<T>;

        Array()
        {
        };

        Array(T initial_value)
        {
            fill(initial_value);
        }

        Array(InitializerList values)
        {
            auto iter = values.begin();
            for(int index = 0; index < values.size(); ++index)
            {
                data_[index] = *iter;
                ++iter;
            }
        }

        Array(const Array& array)
        {
            for(int index = 0; index < array.length(); ++index)
            {
                data_[index] = array(index);
            }
        }

        void fill(T value)
        {
            for(int i = 0; i < Dim; ++i)
            {
                data_[i] = value;
            }
        }

        void fill(const Array& array)
        {
            for(int index = 0; index < Dim; ++index)
            {
                data_[index] = array(index);
            }
        }

        T& operator()(int index)
        {
            index = transform_index(index);
            check_input(index);
            return data_[index];
        }

        T operator()(int index) const
        {
            index = transform_index(index);
            check_input(index);
            return data_[index];
        }

        template <int Size>
        Array<T, true, Size> operator()(const Array<int, true, Size>& indices) const
        {
            Array<T, true, Size> indexed;
            for(int index = 0; index < Size; ++index)
            {
                indexed(index) = data_[indices(index)];
            }
            return indexed;
        }

        void operator=(const Array& array)
        {
            for(int index = 0; index < array.length(); ++index)
            {
                data_[index] = array(index);
            }
        }

        int length() const
        {
            return Dim;
        }
};

template <typename T, int FirstDim, int ... OtherDim>
requires(FirstDim > 0 && sizeof...(OtherDim) > 0)
class Array<T, true, FirstDim, OtherDim...>
{
    private:
        using SubArray = Array<T, true, OtherDim...>;
        static constexpr int NumDims = 1+sizeof...(OtherDim);
        SubArray data_[FirstDim];

        void check_input(int index) const
        {
            if(index >= FirstDim || index < 0)
            {
                throw OutOfRange(index, FirstDim);
            }
        }

        int transform_index(int index) const
        {
            int transformed_index = index;
            while(transformed_index < 0)
            {
                transformed_index += FirstDim;
            }
            return transformed_index;
        }

    public:
        using InitializerList = std::initializer_list<typename SubArray::InitializerList>;

        Array() {};

        Array(T initial_value)
        {
            fill(initial_value);
        }

        Array(InitializerList initializer_list)
        {
            int i = 0;
            for(auto iter = initializer_list.begin(); iter != initializer_list.end(); ++iter)
            {
                data_[i] = SubArray(*iter);
                ++i;
            }
        }

        Array(const Array& array)
        {
            for(int index = 0; index < array.length(); ++index)
            {
                data_[index] = array(index);
            }
        }

        void fill(T value)
        {
            for(int i = 0; i < FirstDim; ++i)
            {
                data_[i].fill(value);
            }
        }

        void fill(const Array& array)
        {
            for(int index = 0; index < FirstDim; ++index)
            {
                data_[index].fill(array(index));
            }
        }

        SubArray& operator()(int index)
        {
            index = transform_index(index);
            check_input(index);
            return data_[index];
        }

        SubArray operator()(int index) const
        {
            index = transform_index(index);
            check_input(index);
            return data_[index];
        }

        template <typename ... OtherIndices>
        requires(sizeof...(OtherIndices) > 0)
        auto operator()(int first, OtherIndices... others) const
        {
            first = transform_index(first);
            check_input(first);
            return data_[first](others...);
        }

        template <typename ... OtherIndices>
        requires(sizeof...(OtherIndices) > 0)
        auto& operator()(int first, OtherIndices... others)
        {
            first = transform_index(first);
            check_input(first);
            return data_[first](others...);
        }

        template <int Size>
        Array<T, true, Size, OtherDim...> operator()(const Array<int, true, Size>& indices) const
        {
            Array<T, true, Size, OtherDim...> indexed;
            for(int index = 0; index < Size; ++index)
            {
                int transformed_index = transform_index(indices(index));
                check_input(transformed_index);
                indexed(index) = data_[transformed_index];
            }
            return indexed;
        }

        void operator=(const Array& array)
        {
            for(int index = 0; index < array.length(); ++index)
            {
                data_[index] = array(index);
            }
        }

        int length() const
        {
            return FirstDim;
        }
};

template <typename T, int ... Shape>
using StaticArray = Array<T, true, Shape...>;

template <int ... Shape>
using StaticArrayi = StaticArray<int, Shape...>;

template <int ... Shape>
using StaticArrayf = StaticArray<float, Shape...>;

template <int ... Shape>
using StaticArrayd = StaticArray<double, Shape...>;

template <typename T, int Size>
using StaticVector = StaticArray<T, Size>;

template <int Size>
using StaticVectori = StaticVector<int, Size>;

template <int Size>
using StaticVectorf = StaticVector<float, Size>;

template <int Size>
using StaticVectord = StaticVector<double, Size>;

template <typename T, typename V=T, int ... Shape>
StaticArray<V, Shape...> empty_like(const StaticArray<T, Shape...>& array)
{
    return StaticArray<V, Shape...>();
}

template <typename T, int Length>
bool all_equal(const StaticVector<T, Length>& left, const StaticVector<T, Length>& right)
{
    for(int index = 0; index < left.length(); ++index)
    {
        if(left(index) != right(index))
        {
            return false;
        }
    }
    return true;
}

template <typename T, int ... Lengths>
requires(sizeof...(Lengths) > 1)
bool all_equal(const StaticArray<T, Lengths ...>& left, const StaticArray<T, Lengths ...>& right)
{
    for(int index = 0; index < left.length(); ++index)
    {
        if(!all_equal(left(index), right(index)))
        {
            return false;
        }
    }
    return true;
}

template <int Length>
bool all(const StaticVector<bool, Length>& vector)
{
    for(int index = 0; index < vector.length(); ++index)
    {
        if(!vector(index))
        {
            return false;
        }
    }
    return true;
}

template <int FirstLength, int SecondLength, int ... Shape>
bool all(const StaticArray<bool, FirstLength, SecondLength, Shape ...>& array)
{
    for(int index = 0; index < array.length(); ++index)
    {
        if(!all(array(index)))
        {
            return false;
        }
    }
    return true;
}

template <typename T, int N>
StaticArray<T, N, N> Identity()
{
    StaticArray<T, N, N> matrix;
    T identity_element = static_cast<T>(1);
    T zero_element = static_cast<T>(0);
    for(int row = 0; row < N; ++row)
    {
        for(int column = 0; column < N; ++column)
        {
            matrix(row,column) = (row==column ? identity_element : zero_element);
        }
    }
    return matrix;
}

template <int N>
StaticVectori<N> ARange()
{
    StaticVectori<N> range;
    for(int index = 0; index < N; ++index)
    {
        range(index) = index;
    }
    return range;
}
}