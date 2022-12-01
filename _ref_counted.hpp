/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "__ref_counted_base_posix.hpp"

#include <exception>

namespace ft
{
    template <typename T>
    class _counted_impl : public _counted_base
    {
    private:
        T* ptr;

        _counted_impl(const _counted_impl&);
        _counted_impl& operator=(const _counted_impl&);

    public:
        explicit _counted_impl(T* ptr)
            : ptr(ptr) {}

        void dispose() throw()
        {
            delete ptr;
        }

        void destroy() throw()
        {
            delete this;
        }
    };

    template <typename TPointer, typename TDelete>
    class _counted_impl_del : public _counted_base
    {
    private:
        TPointer ptr;
        TDelete del;

        _counted_impl_del(const _counted_impl_del&);
        _counted_impl_del& operator=(const _counted_impl_del&);

    public:
        explicit _counted_impl_del(TPointer ptr, const TDelete& del)
            : ptr(ptr), del(del) {}

        void dispose() throw()
        {
            this->del(ptr);
        }

        void destroy() throw()
        {
            delete this;
        }
    };

    template <typename TPointer, typename TDelete, typename TAlloc>
    class _counted_impl_del_alloc : public _counted_base
    {
    private:
        TPointer ptr;
        TDelete del;
        TAlloc alloc;

        _counted_impl_del_alloc(const _counted_impl_del_alloc&);
        _counted_impl_del_alloc& operator=(const _counted_impl_del_alloc&);

    public:
        explicit _counted_impl_del_alloc(TPointer ptr, const TDelete& del, const TAlloc& alloc)
            : ptr(ptr), del(del), alloc(alloc) {}

        void dispose() throw()
        {
            this->del(ptr);
        }

        void destroy() throw()
        {
            typedef _counted_impl_del_alloc TCounted;
            typedef typename TAlloc::template rebind<TCounted>::other TAllocCounted;

            TAllocCounted alloc_counted(this->alloc);
            static_cast<TCounted*>(this)->~TCounted();
            alloc_counted.deallocate(this, 1);
        }
    };

    class _weak_count;

    class _shared_count
    {
    private:
        friend class _weak_count;

    private:
        _counted_base* ptr;

    public:
        _shared_count() throw()
            : ptr(NULL) {}

        explicit _shared_count(_counted_base* ptr) throw()
            : ptr(ptr) {}

        template <typename T>
        explicit _shared_count(T* p)
            : ptr(NULL)
        {
            try
            {
                this->ptr = new _counted_impl<T>(p);
            }
            catch (...)
            {
                delete p;
                throw;
            }
        }

        template <typename TPointer, typename TDelete>
        _shared_count(TPointer p, TDelete del)
        {
            try
            {
                this->ptr = new _counted_impl_del<TPointer, TDelete>(p, del);
            }
            catch (...)
            {
                del(p);
                throw;
            }
        }

        template <typename TPointer, typename TDelete, typename TAlloc>
        _shared_count(TPointer p, TDelete del, TAlloc alloc)
        {
            typedef _counted_impl_del_alloc<TPointer, TDelete, TAlloc> TCounted;
            typedef typename TAlloc::template rebind<TCounted>::other TAllocCounted;

            TAllocCounted alloc_counted(alloc);
            this->ptr = NULL;
            try
            {
                this->ptr = alloc_counted.allocate(1);
                ::new (this->ptr) TCounted(p, del, alloc);
            }
            catch (...)
            {
                if (this->ptr != NULL)
                {
                    alloc_counted.deallocate(static_cast<TCounted*>(this->ptr), 1);
                }
                del(p);
                throw;
            }
        }

        // Internal BEGIN
        template <typename T, typename TAlloc>
        _shared_count(T** pp, const TAlloc& alloc, const T& value)
        {
            // TODO: ...
            *pp = NULL;
        }
        // Internal END

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
        friend class _shared_count;

    private:
        _counted_base* ptr;

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
            throw std::bad_exception(); // bad_weak_ptr
        }
    }

    inline bool _shared_count::operator==(const _weak_count& that) const throw()
    {
        return this->ptr == that.ptr;
    }
}
