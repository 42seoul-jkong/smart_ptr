/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "shared_ptr.hpp"
#include "weak_ptr.hpp"

namespace ft
{
    template <typename T>
    class enable_shared_from_this
    {
    private:
        template <typename U>
        friend class shared_ptr;

    private:
        mutable ft::weak_ptr<T> weak_this;

    protected:
        enable_shared_from_this() throw() {}
        enable_shared_from_this(const enable_shared_from_this&) throw() {}
        enable_shared_from_this& operator=(const enable_shared_from_this&) throw() { return *this; }
        ~enable_shared_from_this() throw() {}

    public:
        ft::shared_ptr<T> shared_from_this() { return ft::shared_ptr<T>(this->weak_this); }
        ft::shared_ptr<const T> shared_from_this() const { return ft::shared_ptr<const T>(this->weak_this); }
        ft::weak_ptr<T> weak_from_this() throw() { return this->weak_this; }
        ft::weak_ptr<const T> weak_from_this() const throw() { return this->weak_this; }

    private:
        template <typename TAlias, typename U>
        void change_ownership(const ft::shared_ptr<TAlias>* alias, U* p) const throw()
        {
            if (this->weak_this.expired())
            {
                this->weak_this = ft::shared_ptr<T>(*alias, p);
            }
        }
    };
}
