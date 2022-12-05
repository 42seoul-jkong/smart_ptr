/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include <cstddef>

namespace ft
{
    struct _ptr_internal_tag
    {
    };

    template <typename T, T Value>
    struct _ptr_integral_constant
    {
        typedef T value_type;
        typedef _ptr_integral_constant<T, Value> type;
        static const T value = Value;

        operator T() const { return Value; }
    };

    typedef _ptr_integral_constant<bool, true> _ptr_true_type;
    typedef _ptr_integral_constant<bool, false> _ptr_false_type;

    template <bool B, typename T = void>
    struct _ptr_enable_if
    {
        typedef T type;
    };

    template <typename T>
    struct _ptr_enable_if<false, T>
    {
    };

    template <typename TSource, typename TDest>
    struct _ptr_convertible
    {
        typedef char (&yes)[1];
        typedef char (&no)[2];

        static yes f(TDest*);
        static no f(...);

        static const bool value = sizeof((f)(static_cast<TSource*>(0))) == sizeof(yes);
    };

    template <typename TSource, typename TDest>
    struct _ptr_convertible<TSource, TDest[]>
    {
        static const bool value = false;
    };

    template <typename TSource, typename TDest>
    struct _ptr_convertible<TSource[], TDest[]>
    {
        static const bool value = _ptr_convertible<TSource[1], TDest[1]>::value;
    };

    template <typename TSource, std::size_t N, typename TDest>
    struct _ptr_convertible<TSource[N], TDest[]>
    {
        static const bool value = _ptr_convertible<TSource[1], TDest[1]>::value;
    };

    ///

    template <typename TSource, typename TDest>
    inline void _ptr_assert_convertible() throw()
    {
        (void)sizeof(char[_ptr_convertible<TSource, TDest>::value ? 1 : -1]);
    }

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
        static const std::size_t value = 0;
    };

    template <typename T, std::size_t N>
    struct _ptr_array_extent<T[N]>
    {
        static const std::size_t value = N;
    };

    template <typename T>
    struct _ptr_is_unbounded_array : _ptr_false_type
    {
    };

    template <typename T>
    struct _ptr_is_unbounded_array<T[]> : _ptr_true_type
    {
    };

    template <typename T>
    struct _ptr_is_unbounded_array<const T[]> : _ptr_true_type
    {
    };

    template <typename T>
    struct _ptr_is_unbounded_array<volatile T[]> : _ptr_true_type
    {
    };

    template <typename T>
    struct _ptr_is_unbounded_array<const volatile T[]> : _ptr_true_type
    {
    };

    template <typename T>
    struct _ptr_is_bounded_array : _ptr_false_type
    {
    };
    template <typename T, std::size_t N>
    struct _ptr_is_bounded_array<T[N]> : _ptr_true_type
    {
    };

    template <typename T, std::size_t N>
    struct _ptr_is_bounded_array<const T[N]> : _ptr_true_type
    {
    };

    template <typename T, std::size_t N>
    struct _ptr_is_bounded_array<volatile T[N]> : _ptr_true_type
    {
    };

    template <typename T, std::size_t N>
    struct _ptr_is_bounded_array<const volatile T[N]> : _ptr_true_type
    {
    };

    template <typename T>
    struct _ptr_is_array
        : _ptr_integral_constant<
              bool,
              _ptr_is_unbounded_array<T>::value ||
                  _ptr_is_bounded_array<T>::value>
    {
    };

    template <typename T>
    struct _ptr_default_delete
    {
        _ptr_default_delete() throw() {}
        _ptr_default_delete(const _ptr_default_delete&) throw() {}

        void operator()(T* p) const
        {
            delete p;
        }
    };

    template <typename T>
    struct _ptr_default_delete<T[]>
    {
        _ptr_default_delete() throw() {}
        _ptr_default_delete(const _ptr_default_delete&) throw() {}

        void operator()(T* p) const
        {
            delete[] p;
        }
    };

    template <typename T, std::size_t N>
    struct _ptr_default_delete<T[N]> : _ptr_default_delete<T[]>
    {
    };

    template <typename T, typename TAlloc>
    struct _ptr_allocator_delete
    {
        _ptr_allocator_delete(const TAlloc& alloc) throw() : alloc(alloc) {}
        _ptr_allocator_delete(const _ptr_allocator_delete& that) throw() : alloc(that.alloc) {}

        template <typename U>
        void operator()(U* p) const
        {
            typedef typename TAlloc::template rebind<U>::other alloc_type;

            alloc_type alloc(this->alloc);

            alloc.deallocate(p, 1);
        }

    private:
        TAlloc alloc;
    };

    template <typename T, std::size_t N, typename TAlloc>
    struct _ptr_allocator_delete<T[N], TAlloc>
    {
        _ptr_allocator_delete(const TAlloc& alloc) throw() : alloc(alloc) {}
        _ptr_allocator_delete(const _ptr_allocator_delete& that) throw() : alloc(that.alloc) {}

        template <typename U>
        void operator()(U* p) const
        {
            typedef typename TAlloc::template rebind<U>::other alloc_type;

            alloc_type alloc(this->alloc);

            alloc.deallocate(p, N);
        }

    private:
        TAlloc alloc;
    };
}
