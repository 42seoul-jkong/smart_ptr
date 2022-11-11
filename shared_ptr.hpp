/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "_ptr_element.hpp"
#include "_ref_counted.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>

namespace ft
{
    template <typename T>
    class weak_ptr;

    template <typename T>
    class shared_ptr
    {
    public:
        typedef typename _ptr_element_type<T>::type element_type;

    private:
        typedef shared_ptr<T> this_type;

        template <class U>
        friend class weak_ptr;
        template <class U>
        friend class shared_ptr;

    private:
        element_type* ptr;
        _shared_count ref;

    public:
        shared_ptr() throw()
            : ptr(NULL), ref() {}

        template <typename U>
        explicit shared_ptr(U* p)
            : ptr(p), ref()
        {
            _shared_count(p).swap(ref);
            // TODO: enable_shared_from_this
        }

        shared_ptr(const shared_ptr& that) throw()
            : ptr(that.ptr), ref(that.ref) {}

        template <typename U>
        shared_ptr(const shared_ptr<U>& that) throw()
            : ptr(that.ptr), ref(that.ref)
        {
            _ptr_assert_convertible<U, T>();
        }

        template <typename U>
        shared_ptr(const shared_ptr<U>& that, element_type* p) throw()
            : ptr(p), ref(that.ref) {}

        template <typename U>
        explicit shared_ptr(const weak_ptr<U>& that)
            : ref(that.ref)
        {
            _ptr_assert_convertible<U, T>();

            this->ptr = that.ptr;
        }

        ~shared_ptr() throw() {}

        shared_ptr& operator=(const shared_ptr& that) throw()
        {
            this_type(that).swap(*this);
            return *this;
        }

        template <typename U>
        shared_ptr& operator=(const shared_ptr<U>& that) throw()
        {
            _ptr_assert_convertible<U, T>();

            this_type(that).swap(*this);
            return *this;
        }

        void reset() throw()
        {
            this_type().swap(*this);
        }

        template <typename U>
        void reset(U* p)
        {
            assert(p == NULL || p != this->ptr);

            this_type(p).swap(*this);
            return *this;
        }

        template <typename U>
        void reset(const shared_ptr<U>& that, element_type* p) throw()
        {
            this_type(that, p).swap(*this);
            return *this;
        }

        typename _ptr_dereference<T>::type operator*() const throw()
        {
            assert(this->ptr != NULL);

            return *this->ptr;
        }

        typename _ptr_member_access<T>::type operator->() const throw()
        {
            assert(this->ptr != NULL);

            return this->ptr;
        }

        typename _ptr_array_access<T>::type operator[](std::ptrdiff_t i) const throw()
        {
            assert(this->ptr != NULL);
            assert(i >= 0 && (i < _ptr_array_extent<T>::value || _ptr_array_extent<T>::value == 0));

            return this->ptr[i];
        }

        element_type* get() const throw()
        {
            return this->ptr;
        }

        bool unique() const throw()
        {
            return this->ref.unique();
        }

        long use_count() const throw()
        {
            return this->ref.use_count();
        }

        operator bool() const throw()
        {
            return this->ptr != NULL;
        }

        void swap(shared_ptr& that) throw()
        {
            std::swap(this->ptr, that.ptr);
            this->ref.swap(that.ref);
        }
    };

    template <typename T, typename U>
    bool operator==(const shared_ptr<T>& lhs, const shared_ptr<U>& rhs) throw()
    {
        return lhs.get() == rhs.get();
    }

    template <typename T, typename U>
    bool operator!=(const shared_ptr<T>& lhs, const shared_ptr<U>& rhs) throw()
    {
        return lhs.get() != rhs.get();
    }

    template <typename T>
    void swap(shared_ptr<T>& lhs, shared_ptr<T>& rhs) throw()
    {
        lhs.swap(rhs);
    }

    template <typename T>
    typename shared_ptr<T>::element_type* get_pointer(const shared_ptr<T>& p) throw()
    {
        return p.get();
    }

    template <typename T, typename TSource>
    shared_ptr<T> static_pointer_cast(const shared_ptr<TSource>& that) throw()
    {
        // Compile-time test
        (void)static_cast<T*>(static_cast<TSource*>(NULL));

        return shared_ptr<T>(that, static_cast<typename shared_ptr<T>::element_type*>(that.get()));
    }

    template <typename T, typename TSource>
    shared_ptr<T> const_pointer_cast(const shared_ptr<TSource>& that) throw()
    {
        // Compile-time test
        (void)const_cast<T*>(const_cast<TSource*>(NULL));

        return shared_ptr<T>(that, const_cast<typename shared_ptr<T>::element_type*>(that.get()));
    }

    template <typename T, typename TSource>
    shared_ptr<T> dynamic_pointer_cast(const shared_ptr<TSource>& that) throw()
    {
        // Compile-time test
        (void)dynamic_cast<T*>(dynamic_cast<TSource*>(NULL));

        // Run-time test
        typename shared_ptr<T>::element_type* p = dynamic_cast<typename shared_ptr<T>::element_type*>(that.get());
        if (!p)
        {
            return shared_ptr<T>();
        }

        return shared_ptr<T>(that, p);
    }

    template <typename T, typename TSource>
    shared_ptr<T> reinterpret_pointer_cast(const shared_ptr<TSource>& that) throw()
    {
        // Compile-time test
        (void)reinterpret_cast<T*>(reinterpret_cast<TSource*>(NULL));

        return shared_ptr<T>(that, reinterpret_cast<typename shared_ptr<T>::element_type*>(that.get()));
    }
}
