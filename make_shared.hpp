/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "shared_ptr.hpp"

namespace ft
{
    namespace _internal
    {
        // single
        template <typename T, typename TAlloc>
        struct deleter_storage
        {
            T data;
            TAlloc alloc;

        public:
            typedef T* pointer_type;
            typedef TAlloc allocate_type;

        public:
            deleter_storage(const T& data, const TAlloc& alloc) throw() : data(data), alloc(alloc) {}
            deleter_storage(const deleter_storage& that) throw() : data(that.data), alloc(that.alloc) {}
            ~deleter_storage() {}

        public:
            template <typename U>
            void operator()(U* p) const
            {
                static_cast<T*>(p)->~T();
            }

        private:
            deleter_storage& operator=(const deleter_storage&);
        };

        // array
        template <typename T, std::size_t N, typename TAlloc>
        struct deleter_storage<T[N], TAlloc>
        {
            T data[N];
            TAlloc alloc;

        public:
            typedef T* pointer_type;
            typedef TAlloc allocate_type;

        public:
            deleter_storage(const TAlloc& alloc) throw() : alloc(alloc) {}
            deleter_storage(const deleter_storage& that) throw() : alloc(that.alloc) {}
            ~deleter_storage() {}

        public:
            template <typename U>
            void operator()(U* p) const {}

        private:
            deleter_storage& operator=(const deleter_storage&);
        };
    }

    template <typename T, typename TAlloc>
    ft::shared_ptr<T> allocate_shared(const TAlloc& a)
    {
        return ft::shared_ptr<T>(_internal::internal_tag(), _internal::deleter_storage<T, TAlloc>(T(), a));
    }

    template <typename T>
    ft::shared_ptr<T> make_shared()
    {
        typedef std::allocator<T> allocate_type;
        return allocate_shared<T, allocate_type>(allocate_type());
    }

    // TODO: Forward arguments and use internal constructor

    // template <typename T, typename TAlloc, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
    // ft::shared_ptr<T> allocate_shared(const TAlloc& a, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9);

    // template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
    // ft::shared_ptr<T> make_shared(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9);
}
