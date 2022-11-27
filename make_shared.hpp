/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "shared_ptr.hpp"

namespace ft
{
    template <class T>
    ft::shared_ptr<T> make_shared()
    {
        // TODO: ...
        return ft::shared_ptr<T>(new T);
    }

    // template <class T, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
    // ft::shared_ptr<T> make_shared(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9);

    // template <class T, class TAlloc, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
    // ft::shared_ptr<T> allocate_shared(const TAlloc& a, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9);
}
