
//          Copyright Nat Goodspeed 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/coroutine/all.hpp>

#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>

#include <boost/bind.hpp>
#include <boost/range.hpp>
#include <boost/shared_ptr.hpp>

struct node
{
    typedef boost::shared_ptr< node >    ptr_t;

    // Each tree node has an optional left subtree, an optional right subtree
    // and a value of its own. The value is considered to be between the left
    // subtree and the right.
    ptr_t left, right;
    std::string value;

    // construct leaf
    node(const std::string& v):
        left(), right(), value(v)
    {}
    // construct nonleaf
    node(ptr_t l, const std::string& v, ptr_t r):
        left(l), right(r), value(v)
    {}

    static ptr_t create(const std::string& v)
    {
        return ptr_t(new node(v));
    }

    static ptr_t create(ptr_t l, const std::string& v, ptr_t r)
    {
        return ptr_t(new node(l, v, r));
    }
};

node::ptr_t create_left_tree_from(const std::string& root)
{
    /* --------
         root
         / \
        b   e
       / \
      a   c
     -------- */
    return node::create(
            node::create(
                node::create("a"),
                "b",
                node::create("c")),
            root,
            node::create("e"));
}

node::ptr_t create_right_tree_from(const std::string& root)
{
    /* --------
         root
         / \
        a   d
           / \
          c   e
       -------- */
    return node::create(
            node::create("a"),
            root,
            node::create(
                node::create("c"),
                "d",
                node::create("e")));
}

// recursively walk the tree, delivering values in order
void traverse(node::ptr_t n,boost::coroutines::asymmetric_coroutine<std::string>::push_type& out)
{
    if (n->left) traverse(n->left,out);
    out(n->value);
    if (n->right) traverse(n->right,out);
}

int main()
{
    {
        node::ptr_t left_d(create_left_tree_from("d"));
        boost::coroutines::asymmetric_coroutine<std::string>::pull_type left_d_reader(
            boost::bind(traverse, left_d, _1));
        std::cout << "left tree from d:\n";
        std::copy(boost::begin(left_d_reader),
                  boost::end(left_d_reader),
                  std::ostream_iterator<std::string>(std::cout, " "));
        std::cout << std::endl;

        node::ptr_t right_b(create_right_tree_from("b"));
        boost::coroutines::asymmetric_coroutine<std::string>::pull_type right_b_reader(
            boost::bind(traverse, right_b, _1));
        std::cout << "right tree from b:\n";
        std::copy(boost::begin(right_b_reader),
                  boost::end(right_b_reader),
                  std::ostream_iterator<std::string>(std::cout, " "));
        std::cout << std::endl;

        node::ptr_t right_x(create_right_tree_from("x"));
        boost::coroutines::asymmetric_coroutine<std::string>::pull_type right_x_reader(
            boost::bind(traverse, right_x, _1));
        std::cout << "right tree from x:\n";
        std::copy(boost::begin(right_x_reader),
                  boost::end(right_x_reader),
                  std::ostream_iterator<std::string>(std::cout, " "));
        std::cout << std::endl;
    }

    {
        node::ptr_t left_d(create_left_tree_from("d"));
        boost::coroutines::asymmetric_coroutine<std::string>::pull_type left_d_reader(
            boost::bind(traverse, left_d, _1));

        node::ptr_t right_b(create_right_tree_from("b"));
        boost::coroutines::asymmetric_coroutine<std::string>::pull_type right_b_reader(
            boost::bind(traverse, right_b, _1));

        std::cout << "left tree from d == right tree from b? "
                  << std::boolalpha
                  << std::equal(boost::begin(left_d_reader),
                                boost::end(left_d_reader),
                                boost::begin(right_b_reader))
                  << std::endl;
    }

    {
        node::ptr_t left_d(create_left_tree_from("d"));
        boost::coroutines::asymmetric_coroutine<std::string>::pull_type left_d_reader(
            boost::bind(traverse, left_d, _1));

        node::ptr_t right_x(create_right_tree_from("x"));
        boost::coroutines::asymmetric_coroutine<std::string>::pull_type right_x_reader(
            boost::bind(traverse, right_x, _1));

        std::cout << "left tree from d == right tree from x? "
                  << std::boolalpha
                  << std::equal(boost::begin(left_d_reader),
                                boost::end(left_d_reader),
                                boost::begin(right_x_reader))
                  << std::endl;
    }

    std::cout << "Done" << std::endl;

    return EXIT_SUCCESS;
}
