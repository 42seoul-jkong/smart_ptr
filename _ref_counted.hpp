/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "__ref_counted_base_posix.hpp"

namespace ft
{
    template <typename T>
    class _counted_impl : public _counted_base
    {
    private:
        typedef _counted_impl<T> this_type;

    private:
        T* ptr;

        _counted_impl(const _counted_impl&);
        _counted_impl& operator=(const _counted_impl&);

    public:
        explicit _counted_impl(T* ptr) : ptr(ptr) {}

        void dispose() throw()
        {
            // TODO: delete[]?
            delete ptr;
        }
    };

    class _weak_count;

    class _shared_count
    {
    private:
        _counted_base* ptr;
        friend class _weak_count;

    public:
        _shared_count() throw()
            : ptr(NULL) {}

        explicit _shared_count(_counted_base* ptr) throw()
            : ptr(ptr) {}

        template <typename U>
        explicit _shared_count(U* p) : ptr(NULL)
        {
            try
            {
                this->ptr = new _counted_impl<U>(p);
            }
            catch (...)
            {
                // TODO: ...
                delete p;
                throw;
            }
        }

        _shared_count(const _shared_count& that) throw()
            : ptr(that.ptr)
        {
            if (this->ptr != NULL)
            {
                this->ptr->add_ref_copy();
            }
        }

        explicit _shared_count(const _weak_count& that);

        ~_shared_count()
        {
            if (this->ptr != NULL)
            {
                this->ptr->release();
            }
        }

        _shared_count& operator=(const _shared_count& that) throw()
        {
            if (this->ptr != that.ptr)
            {
                _counted_base* tmp = that.ptr;
                if (that.ptr != NULL)
                {
                    that.ptr->add_ref_copy();
                }
                if (this->ptr != NULL)
                {
                    this->ptr->release();
                }
                this->ptr = tmp;
            }
            return *this;
        }

        void swap(_shared_count& that) throw()
        {
            _counted_base* tmp = that.ptr;
            that.ptr = this->ptr;
            this->ptr = tmp;
        }

        long use_count() const throw()
        {
            if (this->ptr == NULL)
            {
                return 0;
            }
            return this->ptr->use_count();
        }

        bool unique() const throw()
        {
            return this->use_count() == 1;
        }

        bool empty() const throw()
        {
            return this->ptr == NULL;
        }

        bool operator==(const _shared_count& that) const throw()
        {
            return this->ptr == that.ptr;
        }

        bool operator==(const _weak_count& that) const throw();
    };

    class _weak_count
    {
    private:
        _counted_base* ptr;
        friend class _shared_count;

    public:
        _weak_count() throw()
            : ptr(NULL) {}

        _weak_count(const _shared_count& that) throw()
            : ptr(that.ptr)
        {
            if (this->ptr != NULL)
            {
                this->ptr->weak_add_ref();
            }
        }

        _weak_count(const _weak_count& that) throw()
            : ptr(that.ptr)
        {
            if (this->ptr != NULL)
            {
                this->ptr->weak_add_ref();
            }
        }

        ~_weak_count()
        {
            if (this->ptr != NULL)
            {
                this->ptr->weak_release();
            }
        }

        _weak_count& operator=(const _shared_count& that) throw()
        {
            if (this->ptr != that.ptr)
            {
                _counted_base* tmp = that.ptr;
                if (that.ptr != NULL)
                {
                    that.ptr->weak_add_ref();
                }
                if (this->ptr != NULL)
                {
                    this->ptr->weak_release();
                }
                this->ptr = tmp;
            }
            return *this;
        }

        _weak_count& operator=(const _weak_count& that) throw()
        {
            if (this->ptr != that.ptr)
            {
                _counted_base* tmp = that.ptr;
                if (that.ptr != NULL)
                {
                    that.ptr->weak_add_ref();
                }
                if (this->ptr != NULL)
                {
                    this->ptr->weak_release();
                }
                this->ptr = tmp;
            }
            return *this;
        }

        void swap(_weak_count& that) throw()
        {
            _counted_base* tmp = that.ptr;
            that.ptr = this->ptr;
            this->ptr = tmp;
        }

        long use_count() const throw()
        {
            if (this->ptr == NULL)
            {
                return 0;
            }
            return this->ptr->use_count();
        }

        bool operator==(const _weak_count& that) const throw()
        {
            return this->ptr == that.ptr;
        }

        bool operator==(const _shared_count& that) const throw()
        {
            return this->ptr == that.ptr;
        }
    };

    inline _shared_count::_shared_count(const _weak_count& that)
        : ptr(that.ptr)
    {
        if (this->ptr == NULL || !this->ptr->add_ref_lock())
        {
            throw;
        }
    }

    inline bool _shared_count::operator==(const _weak_count& that) const throw()
    {
        return this->ptr == that.ptr;
    }
}
