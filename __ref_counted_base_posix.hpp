/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include <pthread.h>

#include <cassert>
#include <cstdint>

namespace ft
{
    class _counted_base
    {
    private:
        std::int_least32_t shared_count;
        std::int_least32_t weak_count;
        mutable pthread_mutex_t mutex;

        _counted_base(const _counted_base&);
        _counted_base& operator=(const _counted_base&);

    public:
        _counted_base()
            : shared_count(1), weak_count(1)
        {
            assert(pthread_mutex_init(&mutex, 0) == 0);
        }

        virtual ~_counted_base() // throw()
        {
            assert(pthread_mutex_destroy(&mutex) == 0);
        }

        virtual void dispose() = 0; // throw()

        virtual void destroy() // throw()
        {
            delete this;
        }

        void add_ref_copy()
        {
            assert(pthread_mutex_lock(&mutex) == 0);
            ++this->shared_count;
            assert(pthread_mutex_unlock(&mutex) == 0);
        }

        bool add_ref_lock()
        {
            assert(pthread_mutex_lock(&mutex) == 0);
            bool success = this->shared_count == 0 ? false : (++this->shared_count, true);
            assert(pthread_mutex_unlock(&mutex) == 0);
            return success;
        }

        void release() // throw()
        {
            assert(pthread_mutex_lock(&mutex) == 0);
            bool release_resource = --this->shared_count == 0;
            assert(pthread_mutex_unlock(&mutex) == 0);

            if (release_resource)
            {
                dispose();
                weak_release();
            }
        }

        void weak_add_ref() // throw()
        {
            assert(pthread_mutex_lock(&mutex) == 0);
            ++this->weak_count;
            assert(pthread_mutex_unlock(&mutex) == 0);
        }

        void weak_release() // throw()
        {
            assert(pthread_mutex_lock(&mutex) == 0);
            bool release_this = --this->weak_count == 0;
            assert(pthread_mutex_unlock(&mutex) == 0);

            if (release_this)
            {
                destroy();
            }
        }

        long use_count() const // throw()
        {
            assert(pthread_mutex_lock(&mutex) == 0);
            std::int_least32_t value = this->shared_count;
            assert(pthread_mutex_unlock(&mutex) == 0);

            return value;
        }
    };
}
