//Copyright (c) 2006-2009 Emil Dotchevski and Reverge Studios, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/config.hpp>

#if defined( BOOST_NO_EXCEPTIONS )
#   error This program requires exception handling.
#endif

#include "helper2.hpp"
#include <boost/throw_exception.hpp>

namespace
boost
    {
    namespace
    exception_test
        {
        inline
        derives_boost_exception::
        derives_boost_exception( int x ):
            x_(x)
            {
            }

        derives_boost_exception::
        ~derives_boost_exception() BOOST_NOEXCEPT_OR_NOTHROW
            {
            }

        inline
        derives_boost_exception_virtually::
        derives_boost_exception_virtually( int x ):
            x_(x)
            {
            }

        derives_boost_exception_virtually::
        ~derives_boost_exception_virtually() BOOST_NOEXCEPT_OR_NOTHROW
            {
            }

        inline
        derives_std_exception::
        derives_std_exception( int x ):
            x_(x)
            {
            }

        derives_std_exception::
        ~derives_std_exception() BOOST_NOEXCEPT_OR_NOTHROW
            {
            }

        template <>
        void
        throw_test_exception<derives_boost_exception>( int x )
            {
            boost::throw_exception( derives_boost_exception(x) );
            }

        template <>
        void
        throw_test_exception<derives_boost_exception_virtually>( int x )
            {
            boost::throw_exception( derives_boost_exception_virtually(x) );
            }

        template <>
        void
        throw_test_exception<derives_std_exception>( int x )
            {
            boost::throw_exception( derives_std_exception(x) );
            }
        }
    }
