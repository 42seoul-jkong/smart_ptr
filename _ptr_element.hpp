/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include <cstddef>

namespace ft
{
    template <typename T>
    struct _ptr_element_type
    {
        typedef T type;
    };

    template <typename T>
    struct _ptr_element_type<T[]>
    {
        typedef T type;
    };

    template <typename T, std::size_t N>
    struct _ptr_element_type<T[N]>
    {
        typedef T type;
    };

    template <typename T>
    struct _ptr_dereference
    {
        typedef T& type;
    };

    template <>
    struct _ptr_dereference<void>
    {
        typedef void type;
    };

    template <>
    struct _ptr_dereference<const void>
    {
        typedef void type;
    };

    template <>
    struct _ptr_dereference<volatile void>
    {
        typedef void type;
    };

    template <>
    struct _ptr_dereference<const volatile void>
    {
        typedef void type;
    };

    template <typename T>
    struct _ptr_dereference<T[]>
    {
        typedef void type;
    };

    template <typename T, std::size_t N>
    struct _ptr_dereference<T[N]>
    {
        typedef void type;
    };

    template <typename T>
    struct _ptr_member_access
    {
        typedef T* type;
    };

    template <typename T>
    struct _ptr_member_access<T[]>
    {
        typedef void type;
    };

    template <typename T, std::size_t N>
    struct _ptr_member_access<T[N]>
    {
        typedef void type;
    };

    template <typename T>
    struct _ptr_array_access
    {
        typedef void type;
    };

    template <typename T>
    struct _ptr_array_access<T[]>
    {
        typedef T& type;
    };

    template <typename T, std::size_t N>
    struct _ptr_array_access<T[N]>
    {
        typedef T& type;
    };

    template <typename T>
    struct _ptr_array_extent
    {
        enum
        {
            value = 0
        };
    };

    template <typename T, std::size_t N>
    struct _ptr_array_extent<T[N]>
    {
        enum
        {
            value = N
        };
    };

    template <typename TSource, typename TDest>
    struct _ptr_convertible
    {
        typedef char (&yes)[1];
        typedef char (&no)[2];

        static yes f(TDest*);
        static no f(...);

        enum
        {
            value = sizeof((f)(static_cast<TSource*>(0))) == sizeof(yes)
        };
    };

    template <typename TSource, typename TDest>
    struct _ptr_convertible<TSource, TDest[]>
    {
        enum
        {
            value = false
        };
    };

    template <typename TSource, typename TDest>
    struct _ptr_convertible<TSource[], TDest[]>
    {
        enum
        {
            value = _ptr_convertible<TSource[1], TDest[1]>::value
        };
    };

    template <typename TSource, std::size_t N, typename TDest>
    struct _ptr_convertible<TSource[N], TDest[]>
    {
        enum
        {
            value = _ptr_convertible<TSource[1], TDest[1]>::value
        };
    };

    template <typename TSource, typename TDest>
    inline void _ptr_assert_convertible() throw()
    {
        (void)sizeof(char[_ptr_convertible<TSource, TDest>::value ? 1 : -1]);
    }
}
