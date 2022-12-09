/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "shared_ptr.hpp"

#include <cstddef>

namespace ft
{
    namespace _internal
    {
        // single
        template <typename T, typename TAlloc>
        struct deleter_storage
        {
            typedef T* pointer_type;
            typedef TAlloc allocate_type;

            T data;
            TAlloc alloc;

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
            typedef T* pointer_type;
            typedef TAlloc allocate_type;

            T data[N];
            TAlloc alloc;

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

        template <typename T, typename TAlloc>
        struct allocator_delete
        {
            TAlloc alloc;

        public:
            allocator_delete(const TAlloc& alloc) throw() : alloc(alloc) {}
            allocator_delete(const allocator_delete& that) throw() : alloc(that.alloc) {}
            ~allocator_delete() {}

            template <typename U>
            void operator()(U* p) const
            {
                typedef typename TAlloc::template rebind<U>::other alloc_type;

                alloc_type alloc(this->alloc);

                alloc.deallocate(p, 1);
            }

        private:
            allocator_delete& operator=(const allocator_delete&);
        };

        template <typename T, typename TAlloc>
        struct allocator_delete<T[], TAlloc>
        {
            TAlloc alloc;
            std::size_t n;

        public:
            allocator_delete(const TAlloc& alloc, std::size_t n) throw() : alloc(alloc), n(n) {}
            allocator_delete(const allocator_delete& that) throw() : alloc(that.alloc), n(that.n) {}

            template <typename U>
            void operator()(U* p) const
            {
                typedef typename TAlloc::template rebind<U>::other alloc_type;

                alloc_type alloc(this->alloc);

                alloc.deallocate(p, this->n);
            }

        private:
            allocator_delete& operator=(const allocator_delete&);
        };

        template <typename T, std::size_t N, typename TAlloc>
        struct allocator_delete<T[N], TAlloc>
        {
            TAlloc alloc;

        public:
            allocator_delete(const TAlloc& alloc) throw() : alloc(alloc) {}
            allocator_delete(const allocator_delete& that) throw() : alloc(that.alloc) {}

            template <typename U>
            void operator()(U* p) const
            {
                typedef typename TAlloc::template rebind<U>::other alloc_type;

                alloc_type alloc(this->alloc);

                alloc.deallocate(p, N);
            }

        private:
            allocator_delete& operator=(const allocator_delete&);
        };
    }

    template <typename T, typename TAlloc>
    typename _internal::enable_if<!_internal::is_array<T>::value, ft::shared_ptr<T> >::type allocate_shared(const TAlloc& a)
    {
        return ft::shared_ptr<T>(_internal::internal_tag(), _internal::deleter_storage<T, TAlloc>(T(), a));
    }

    template <typename T, typename TAlloc>
    typename _internal::enable_if<_internal::is_bounded_array<T>::value, ft::shared_ptr<T> >::type allocate_shared(const TAlloc& a)
    {
        // TODO:
        return ft::shared_ptr<T>(_internal::internal_tag(), _internal::deleter_storage<T, TAlloc>(a));
    }

    template <typename T, typename TAlloc>
    typename _internal::enable_if<_internal::is_unbounded_array<T>::value, ft::shared_ptr<T> >::type allocate_shared(const TAlloc& a, std::size_t n)
    {
        // TODO:
        _internal::allocate_guard<TAlloc> alloc(a, n);
        ft::shared_ptr<T> result = ft::shared_ptr<T>(alloc.get(), _internal::allocator_delete<T, TAlloc>(a, n), a);
        alloc.reset();
        return result;
    }

    // TODO: array element, but the elements of the array are initialized from the default value.

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
