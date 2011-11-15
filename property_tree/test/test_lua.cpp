/* Copyright (C) 2011 Renato Florentino Garcia
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file BOOST_LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 * For more information, see http://www.boost.org
 */
#include <golld/property_tree/assign.hpp>
#include <golld/property_tree/lua_parser.hpp>
#include <golld/property_tree/ptree_io.hpp>

#include <boost/property_tree/ptree.hpp>
#include <iostream>

namespace bpt = boost::property_tree;
namespace gpt = golld::property_tree;
using namespace golld::property_tree::assign;

int main(int argc, char *argv[])
{
    bpt::ptree pt1 =
        tree("root")
        ("one", "1")
        ("color", "blue")
        ("subtree", tree()("leave", "leave1"))
        ("array", tree()(1)(2)(3));

    bpt::ptree pt2;
    gpt::read_lua("test.lua", "root", pt2);

    gpt::lua_parser::write_lua(std::cout, pt2);
    std::cout << "pt1:\n\n" << pt1 << '\n' << std::endl;
    std::cout << "pt2:\n\n" << pt2 << '\n' << std::endl;

    return 0;
}
