/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include <pthread.h>

#include <cassert>

namespace ft
{
    class _counted_base
    {
    private:
        typedef signed int count_type;

        count_type shared_count;
        count_type weak_count;
        mutable pthread_mutex_t mutex;

        _counted_base(const _counted_base&);
        _counted_base& operator=(const _counted_base&);

    public:
        _counted_base()
            : shared_count(1), weak_count(1)
        {
            assert(pthread_mutex_init(&this->mutex, 0) == 0);
        }

        virtual ~_counted_base() // throw()
        {
            assert(pthread_mutex_destroy(&this->mutex) == 0);
        }

        virtual void dispose() = 0; // throw()

        virtual void destroy() // throw()
        {
            delete this;
        }

        void add_ref_copy()
        {
#ifdef OUTPUT_REF_COUNTED
            OUTPUT_REF_COUNTED << static_cast<const void*>(this) << ": " << __PRETTY_FUNCTION__ << ": ++" << this->shared_count << " (Weak=" << this->weak_count << ")" << std::endl;
#endif

            assert(pthread_mutex_lock(&this->mutex) == 0);
            ++this->shared_count;
            assert(pthread_mutex_unlock(&this->mutex) == 0);
        }

        bool add_ref_lock()
        {
#ifdef OUTPUT_REF_COUNTED
            OUTPUT_REF_COUNTED << static_cast<const void*>(this) << ": " << __PRETTY_FUNCTION__ << ": ++" << this->shared_count << " (Weak=" << this->weak_count << ")" << std::endl;
#endif

            assert(pthread_mutex_lock(&this->mutex) == 0);
            bool success = this->shared_count == 0 ? false : (++this->shared_count, true);
            assert(pthread_mutex_unlock(&this->mutex) == 0);
            return success;
        }

        void release() // throw()
        {
#ifdef OUTPUT_REF_COUNTED
            OUTPUT_REF_COUNTED << static_cast<const void*>(this) << ": " << __PRETTY_FUNCTION__ << ": --" << this->shared_count << " (Weak=" << this->weak_count << ")" << std::endl;
#endif

            assert(pthread_mutex_lock(&this->mutex) == 0);
            bool release_resource = --this->shared_count == 0;
            assert(pthread_mutex_unlock(&this->mutex) == 0);

            if (release_resource)
            {
                dispose();
                weak_release();
            }
        }

        void weak_add_ref() // throw()
        {
#ifdef OUTPUT_REF_COUNTED
            OUTPUT_REF_COUNTED << static_cast<const void*>(this) << ": " << __PRETTY_FUNCTION__ << ": " << this->shared_count << " (Weak=++" << this->weak_count << ")" << std::endl;
#endif

            assert(pthread_mutex_lock(&this->mutex) == 0);
            ++this->weak_count;
            assert(pthread_mutex_unlock(&this->mutex) == 0);
        }

        void weak_release() // throw()
        {
#ifdef OUTPUT_REF_COUNTED
            OUTPUT_REF_COUNTED << static_cast<const void*>(this) << ": " << __PRETTY_FUNCTION__ << ": " << this->shared_count << " (Weak=--" << this->weak_count << ")" << std::endl;
#endif

            assert(pthread_mutex_lock(&this->mutex) == 0);
            bool release_this = --this->weak_count == 0;
            assert(pthread_mutex_unlock(&this->mutex) == 0);

            if (release_this)
            {
                destroy();
            }
        }

        long use_count() const // throw()
        {
#ifdef OUTPUT_REF_COUNTED
            OUTPUT_REF_COUNTED << static_cast<const void*>(this) << ": " << __PRETTY_FUNCTION__ << ": " << this->shared_count << " (Weak=" << this->weak_count << ")" << std::endl;
#endif

            assert(pthread_mutex_lock(&this->mutex) == 0);
            count_type value = this->shared_count;
            assert(pthread_mutex_unlock(&this->mutex) == 0);

            return value;
        }
    };
}
