/*
Copyright 2019 Glen Joseph Fernandes
(glenjofe@gmail.com)

Distributed under the Boost Software License, Version 1.0.
(http://www.boost.org/LICENSE_1_0.txt)
*/
#ifndef BOOST_CORE_DEFAULT_ALLOCATOR_HPP
#define BOOST_CORE_DEFAULT_ALLOCATOR_HPP

#include <boost/config.hpp>
#include <new>

namespace boost {

#if defined(BOOST_NO_EXCEPTIONS)
void throw_exception(const std::exception&);
#endif

namespace default_ {

struct true_type {
    typedef bool value_type;
    typedef true_type type;

    BOOST_STATIC_CONSTANT(bool, value = true);

    BOOST_CONSTEXPR operator bool() const BOOST_NOEXCEPT {
        return true;
    }

    BOOST_CONSTEXPR bool operator()() const BOOST_NOEXCEPT {
        return true;
    }
};

template<class T>
struct default_allocator {
    typedef T value_type;
    typedef T* pointer;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef true_type propagate_on_container_move_assignment;
    typedef true_type is_always_equal;

    template<class U>
    struct rebind {
        typedef default_allocator<U> other;
    };

#if !defined(BOOST_NO_CXX11_DEFAULTED_FUNCTIONS)
    default_allocator() = default;
#else
    BOOST_CONSTEXPR default_allocator() BOOST_NOEXCEPT { }
#endif

    template<class U>
    BOOST_CONSTEXPR default_allocator(const default_allocator<U>&)
        BOOST_NOEXCEPT { }

    BOOST_CONSTEXPR std::size_t max_size() const BOOST_NOEXCEPT {
        return ~static_cast<std::size_t>(0) / sizeof(T);
    }

#if !defined(BOOST_NO_EXCEPTIONS)
    T* allocate(std::size_t n) {
        if (n > max_size()) {
            throw std::bad_alloc();
        }
        return static_cast<T*>(::operator new(sizeof(T) * n));
    }

    void deallocate(T* p, std::size_t) {
        ::operator delete(p);
    }
#else
    T* allocate(std::size_t n) {
        if (n > max_size()) {
            boost::throw_exception(std::bad_alloc());
        }
        void* p = ::operator new(sizeof(T) * n, std::nothrow);
        if (!p) {
            boost::throw_exception(std::bad_alloc());
        }
        return static_cast<T*>(p);
    }

    void deallocate(T* p, std::size_t) {
        ::operator delete(p, std::nothrow);
    }
#endif
};

template<class T, class U>
BOOST_CONSTEXPR inline bool
operator==(const default_allocator<T>&,
    const default_allocator<U>&) BOOST_NOEXCEPT
{
    return true;
}

template<class T, class U>
BOOST_CONSTEXPR inline bool
operator!=(const default_allocator<T>&,
    const default_allocator<U>&) BOOST_NOEXCEPT
{
    return false;
}

} /* default_ */

using default_::default_allocator;

} /* boost */

#endif