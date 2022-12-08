/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "shared_ptr.hpp"

namespace ft
{
    template <typename T>
    ft::shared_ptr<T> make_shared()
    {
        T* p = new T;
        return ft::shared_ptr<T>(p);
    }

    template <typename T, typename TAlloc>
    ft::shared_ptr<T> allocate_shared(const TAlloc& a)
    {
        typedef typename TAlloc::template rebind<T>::other alloc_type;

        alloc_type alloc(a);

        T* p = alloc.allocate(1);
        new (p) T;
        return ft::shared_ptr<T>(p, _internal::allocator_delete<T, alloc_type>(alloc), alloc);
    }

    // TODO: Forward arguments and use internal constructor

    // template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
    // ft::shared_ptr<T> make_shared(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9);

    // template <typename T, typename TAlloc, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
    // ft::shared_ptr<T> allocate_shared(const TAlloc& a, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9);
}
