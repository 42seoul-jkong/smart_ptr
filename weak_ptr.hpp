/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "_ptr_element.hpp"
#include "_ref_counted.hpp"
#include "shared_ptr.hpp"

#include <algorithm>
#include <cstddef>

namespace ft
{
    template <typename T>
    class weak_ptr
    {
    public:
        typedef typename _ptr_element_type<T>::type element_type;

    private:
        typedef weak_ptr<T> this_type;

        template <class U>
        friend class weak_ptr;
        template <class U>
        friend class shared_ptr;

    private:
        element_type* ptr;
        _weak_count ref;

    public:
        weak_ptr() throw()
            : ptr(NULL), ref() {}

        template <typename U>
        weak_ptr(const shared_ptr<U>& that) throw()
            : ptr(that.ptr), ref(that.ref)
        {
            _ptr_assert_convertible<U, T>();
        }

        weak_ptr(const weak_ptr& that) throw()
            : ptr(that.lock().get()), ref(that.ref) {}

        template <typename U>
        weak_ptr(const weak_ptr<U>& that) throw()
            : ptr(that.ptr), ref(that.ref)
        {
            _ptr_assert_convertible<U, T>();
        }

        template <typename U>
        weak_ptr(const shared_ptr<U>& that, element_type* p) throw()
            : ptr(p), ref(that.ref) {}

        template <typename U>
        weak_ptr(const weak_ptr<U>& that, element_type* p) throw()
            : ptr(p), ref(that.ref) {}

        ~weak_ptr() throw() {}

        weak_ptr& operator=(const weak_ptr& that) throw()
        {
            this->ptr = that.lock().get();
            this->ref = that.ref;
            return *this;
        }

        template <typename U>
        weak_ptr& operator=(const weak_ptr<U>& that) throw()
        {
            _ptr_assert_convertible<U, T>();

            this->ptr = that.lock().get();
            this->ref = that.ref;
            return *this;
        }

        template <typename U>
        weak_ptr& operator=(const shared_ptr<U>& that) throw()
        {
            _ptr_assert_convertible<U, T>();

            this->ptr = that.ptr;
            this->ref = that.ref;
            return *this;
        }

        long use_count() const throw()
        {
            return this->ref.use_count();
        }

        bool expired() const throw()
        {
            return this->ref.use_count() == 0;
        }

        shared_ptr<T> lock() const throw()
        {
            return shared_ptr<T>(*this);
        }

        void reset() throw()
        {
            this_type().swap(*this);
        }

        void swap(weak_ptr<T>& that) throw()
        {
            std::swap(ptr, that.ptr);
            ref.swap(that.ref);
        }
    };

    template <typename T>
    void swap(weak_ptr<T>& lhs, weak_ptr<T>& rhs) throw()
    {
        lhs.swap(rhs);
    }
}
