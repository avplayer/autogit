//Copyright (c) 2006-2009 Emil Dotchevski and Reverge Studios, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/config.hpp>

#if !defined( BOOST_NO_EXCEPTIONS )
#   error This program requires exception handling disabled.
#endif

#include <boost/throw_exception.hpp>
#include <boost/exception/info.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/core/lightweight_test.hpp>
#include <stdlib.h>

struct
my_exception:
    boost::exception,
    std::exception
    {
    char const *
    what() const BOOST_NOEXCEPT_OR_NOTHROW
        {
        return "my_exception";
        }
    };

typedef boost::error_info<struct my_tag,int> my_int;

bool called=false;

namespace
boost
    {
    void
    throw_exception( std::exception const & x )
        {
        called=true;
        std::string s=boost::diagnostic_information(x);
        std::cout << s;
#ifndef BOOST_NO_RTTI
        BOOST_TEST(s.find("my_tag")!=std::string::npos);
#endif
        exit(boost::report_errors());
        }
    void
    throw_exception(std::exception const & x, boost::source_location const &)
        {
        throw_exception(x);
        }
    }

int
main()
    {
    BOOST_THROW_EXCEPTION( my_exception() << my_int(42) );
    BOOST_TEST(called);
    return boost::report_errors();
    }
