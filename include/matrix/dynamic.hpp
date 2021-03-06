#pragma once
#include "base.hpp"

#include <initializer_list>

namespace math
{

class MismatchedLength: public std::exception
{
    private:
        int left_length_;
        int right_length_;

    public:
        MismatchedLength(int left_length, int right_length)
        : left_length_(left_length), right_length_(right_length) {}

        ~MismatchedLength() override {};

        const char* what() const noexcept override
        {
            return "mismatched lengths";
        }
};

template <typename T>
class Array<T, false, 1>
{
    private:
        int length_;
        T* data_;

        void check_input(int index) const
        {
            if(index >= length_ || index < 0)
            {
                throw OutOfRange(index, length_);
            }
        }

        void check_length_matches(const Array& array) const
        {
            if(length_ != array.length())
            {
                throw MismatchedLength(length_, array.length());
            }
        }

    public:
        using InitializerList = std::initializer_list<T>;

        Array()
        : length_(0) {}

        Array(int _length)
        : length_(_length), data_(new T[length_]) {}

        Array(const Array& array)
        : length_(array.length()), data_(new T[length_])
        {
            fill(array);
        }

        Array(const InitializerList& values)
        : length_(values.size()), data_(new T[length_])
        {
            int index = 0;
            for(T value : values)
            {
                data_[index] = value;
                ++index;
            }
        }

        void allocate(int _length)
        {
            if(length_ > 0)
            {
                delete [] data_;
            }
            data_ = new T[_length];
            length_ = _length;
        }
        
        void allocate_like(const Array& array)
        {
            allocate(array.length());
        }

        ~Array()
        {
            if(length_ > 0)
            {
                delete [] data_;
            }
        }

        T operator()(int index) const
        {
            check_input(index);
            return data_[index];
        }

        T& operator()(int index)
        {
            check_input(index);
            return data_[index];
        }

        int length() const
        {
            return length_;
        }

        void fill(const InitializerList& values)
        {
            if(length_ == 0)
            {
                allocate(values.size());
            }
            if(values.size() != length_)
            {
                throw MismatchedLength(length_, values.size());
            }
            int index = 0;
            for(T value : values)
            {
                data_[index] = value;
                ++index;
            }
        }

        void fill(T value)
        {
            for(int index = 0; index < length_; ++index)
            {
                data_[index] = value;
            }
        }

        void fill(const Array& vector)
        {
            if(length_ == 0)
            {
                allocate(vector.length());
            }
            check_length_matches(vector);
            for(int index = 0; index < length_; ++index)
            {
                data_[index] = vector(index);
            }
        }

        void operator=(const Array& vector)
        {
            if(length_ == 0)
            {
                allocate(vector.length());
            }
            check_length_matches(vector);
            for(int index = 0; index < length_; ++index)
            {
                data_[index] = vector(index);
            }
        }

        Array operator()(const Array<int, false, 1>& indices) const
        {
            Array indexed(indices.length());
            for(int index = 0; index < indexed.length(); ++index)
            {
                auto data = data_[indices(index)];
                indexed(index) = data;
            }
            return indexed;
        }
};

template <typename T, int NumDims>
requires(NumDims > 1)
class Array<T, false, NumDims>
{
    public:
        using SubArray = Array<T, false, NumDims-1>;

    private:
        int length_;
        SubArray* data_;

        void check_input(int index) const
        {
            if(index >= length_ || index < 0)
            {
                throw OutOfRange(index, length_);
            }
        }


    public:
        using InitializerList = std::initializer_list<typename SubArray::InitializerList>;

        Array()
        : length_(0) {}

        template <typename ... OtherDims>
        requires(sizeof...(OtherDims) == (NumDims-1))
        Array(int _length, OtherDims... others)
        : length_(_length), data_(new SubArray[length_]) 
        {
            for(int index = 0; index < length_; ++index)
            {
                data_[index].allocate(others...);
            }
        }

        Array(const Array& array)
        : length_(array.length()), data_(new SubArray[length_])
        {
            for(int index = 0; index < length_; ++index)
            {
                data_[index].fill(array(index));
            }
        }

        Array(const InitializerList& values)
        : length_(values.size()), data_(new SubArray[length_])
        {
            int index = 0;
            for(typename SubArray::InitializerList value : values)
            {
                data_[index].fill(value);
                ++index;
            }
        }

        ~Array()
        {
            if(length_ > 0)
            {
                delete [] data_;
            }
        }

        void fill(const Array& array)
        {
            if(length_ == 0)
            {
                allocate(array.length());
            }
            else if(length_ != array.length())
            {
                throw MismatchedLength(length_, array.length());
            }
            for(int index = 0; index < length_; ++index)
            {
                data_[index].fill(array(index));
            }
        }

        SubArray operator()(int index) const
        {
            check_input(index);
            return data_[index];
        }

        SubArray& operator()(int index)
        {
            check_input(index);
            return data_[index];
        }

        template <typename ... OtherIndices>
        requires(sizeof...(OtherIndices) > 0)
        auto operator()(int index, OtherIndices... others) const
        {
            check_input(index);
            return data_[index](others...);
        }

        template <typename ... OtherIndices>
        requires(sizeof...(OtherIndices) > 0)
        auto& operator()(int index, OtherIndices... others)
        {
            check_input(index);
            return data_[index](others...);
        }

        int length() const
        {
            return length_;
        }

        void fill(T value)
        {
            for(int index = 0; index < length_; ++index)
            {
                data_[index].fill(value);
            }
        }

        void fill(const InitializerList& values)
        {
            if(length_ == 0)
            {
                allocate(values.size());
            }
            if(values.size() != length_)
            {
                throw MismatchedLength(length_, values.size());
            }
            int index = 0;
            for(typename SubArray::InitializerList value : values)
            {
                data_[index].fill(value);
                ++index;
            }
        } 

        void allocate(int _length)
        {
            if(length_ > 0)
            {
                delete [] data_;
            }
            data_ = new SubArray[_length];
            length_ = _length;
        }

        template <typename ... OtherLengths>
        requires(sizeof...(OtherLengths) == (NumDims-1))
        void allocate(int _length, OtherLengths... others)
        {
            allocate(_length);
            for(int index = 0; index < length_; ++index)
            {
                data_[index].allocate(others...);
            }
        }

        void allocate_like(const Array& array)
        {
            allocate(array.length());
            for(int index = 0; index < length_; ++index)
            {
                data_[index].allocate_like(array(index));
            }
        }

        void operator=(const Array& array)
        {
            fill(array);
        }

        Array operator()(const Array<int, false, 1>& indices) const
        {
            Array indexed;
            indexed.allocate(indices.length());
            for(int index = 0; index < indices.length(); ++index)
            {
                indexed(index) = data_[indices(index)];
            }
            return indexed;
        }
};


template <typename T, int NumDims>
using DynamicArray = Array<T, false, NumDims>;

template <int NumDims>
using DynamicArrayi = DynamicArray<int, NumDims>;

template <int NumDims>
using DynamicArrayf = DynamicArray<float, NumDims>;

template <int NumDims>
using DynamicArrayd = DynamicArray<double, NumDims>;

template <typename T>
using DynamicVector = DynamicArray<T, 1>;

using DynamicVectori = DynamicVector<int>;

using DynamicVectorf = DynamicVector<float>;

using DynamicVectord = DynamicVector<double>;

template <typename T>
using DynamicMatrix = DynamicArray<T, 2>;

using DynamicMatrixi = DynamicMatrix<int>;

using DynamicMatrixf = DynamicMatrix<float>;

using DynamicMatrixd = DynamicMatrix<double>;

template <typename T>
DynamicVector<T> empty_like(const DynamicVector<T>& vector)
{
    DynamicVector<T> empty_vector(vector.length());
    return empty_vector;
};

template <typename T, int NumDims>
requires(NumDims > 1)
DynamicArray<T, NumDims> empty_like(const DynamicArray<T, NumDims>& array)
{
    DynamicArray<T, NumDims> empty_array;
    empty_array.allocate_like(array);
    return empty_array;
};

template <typename T>
bool all_equal(const DynamicVector<T>& left, const DynamicVector<T>& right)
{
    if(left.length() != right.length())
    {
        throw MismatchedLength(left.length(), right.length());
    }

    for(int index = 0; index < left.length(); ++index)
    {
        if(left(index) != right(index))
        {
            return false;
        }
    }
    return true;
};

template <typename T, int NumDims>
requires(NumDims > 1)
bool all_equal(const DynamicArray<T, NumDims>& left, const DynamicArray<T, NumDims>& right)
{
    if(left.length()!=right.length())
    {
        throw MismatchedLength(left.length(), right.length());
    }

    for(int index = 0; index < left.length(); ++index)
    {
        if(!all_equal(left(index), right(index)))
        {
            return false;
        }
    } 
    return true;
};

template <typename T>
DynamicMatrix<T> Identity(int N)
{
    DynamicMatrix<T> matrix(N,N);
    auto identity_element = static_cast<T>(1);
    auto zero_element = static_cast<T>(0);
    for(int row = 0; row < N; ++row)
    {
        for(int column = 0; column < N; ++column)
        {
            matrix(row,column) = (row == column ? identity_element : zero_element);
        }
    }
    return matrix;
};

DynamicVectori ARange(int N);
    
} // namespace math
