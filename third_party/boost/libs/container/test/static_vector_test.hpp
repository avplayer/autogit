// Boost.Container static_vector
// Unit Test

// Copyright (c) 2012-2013 Adam Wulkiewicz, Lodz, Poland.
// Copyright (c) 2012-2013 Andrew Hundt.

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_CONTAINER_TEST_STATIC_VECTOR_TEST_HPP
#define BOOST_CONTAINER_TEST_STATIC_VECTOR_TEST_HPP

#include <boost/container/static_vector.hpp>

#include "movable_int.hpp"

using namespace boost::container;

class value_ndc
{
public:
    explicit value_ndc(int a) : aa(a) {}
    ~value_ndc() {}
    bool operator==(value_ndc const& v) const { return aa == v.aa; }
    bool operator<(value_ndc const& v) const { return aa < v.aa; }
private:
    value_ndc(value_ndc const&) {}
    value_ndc & operator=(value_ndc const&) { return *this; }
    int aa;
};

class value_nd
{
public:
    explicit value_nd(int a) : aa(a) {}
    ~value_nd() {}
    bool operator==(value_nd const& v) const { return aa == v.aa; }
    bool operator<(value_nd const& v) const { return aa < v.aa; }
private:
    int aa;
};

class value_nc
{
public:
    explicit value_nc(int a = 0) : aa(a) {}
    ~value_nc() {}
    value_nc & operator=(int a){  aa = a; return *this;  }
    bool operator==(value_nc const& v) const { return aa == v.aa; }
    bool operator<(value_nc const& v) const { return aa < v.aa; }
private:
    value_nc(value_nc const&) {}
    value_nc & operator=(value_nc const&) { return *this; }
    int aa;
};

class counting_value
{
    BOOST_COPYABLE_AND_MOVABLE(counting_value)

public:
    explicit counting_value(int a = 0, int b = 0) : aa(a), bb(b) { ++c(); }
    counting_value(counting_value const& v) : aa(v.aa), bb(v.bb) { ++c(); }
    counting_value(BOOST_RV_REF(counting_value) p) : aa(p.aa), bb(p.bb) { p.aa = 0; p.bb = 0; ++c(); }                      // Move constructor
    counting_value& operator=(BOOST_RV_REF(counting_value) p) { aa = p.aa; p.aa = 0; bb = p.bb; p.bb = 0; return *this; }   // Move assignment
    counting_value& operator=(BOOST_COPY_ASSIGN_REF(counting_value) p) { aa = p.aa; bb = p.bb; return *this; }              // Copy assignment
    counting_value& operator=(int a) { aa =a; return *this; }              // Copy assignment
    ~counting_value() { --c(); }
    bool operator==(counting_value const& v) const { return aa == v.aa && bb == v.bb; }
    bool operator<(counting_value const& v) const { return aa < v.aa || ( aa == v.aa && bb < v.bb ); }
    static size_t count() { return c(); }

private:
    static size_t & c() { static size_t co = 0; return co; }
    int aa, bb;
};

namespace boost {

template <class T>
struct has_nothrow_move;

template <>
struct has_nothrow_move<counting_value>
{
    static const bool value = true;
};

}

#endif // BOOST_CONTAINER_TEST_STATIC_VECTOR_TEST_HPP
