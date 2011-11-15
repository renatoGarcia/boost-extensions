/* Copyright (C) 2011 Renato Florentino Garcia
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file BOOST_LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 * For more information, see http://www.boost.org
 */
#include <boost/property_tree/ptree.hpp>

#include <golld/property_tree/assign.hpp>

namespace bpt = boost::property_tree;
using namespace golld::property_tree::assign;

int main(int argc, char *argv[])
{
    bpt::ptree pt1;
    pt1.add("key1", "value1");

    bpt::ptree tmpPt1;
    tmpPt1.add("key3", 12345);

    bpt::ptree tmpPt2;
    tmpPt2.add("key4", "value4 with spaces");

    tmpPt1.push_back(std::make_pair("", tmpPt2));
    tmpPt1.add("key5", "value5");

    bpt::ptree tmpPt3;
    tmpPt3.put_value(10);
    tmpPt1.push_back(std::make_pair("", tmpPt3));

    bpt::ptree tmpPt4;
    tmpPt4.put_value(11);
    tmpPt1.push_back(std::make_pair("", tmpPt4));

    bpt::ptree tmpPt5;
    tmpPt5.put_value(12);
    tmpPt1.push_back(std::make_pair("", tmpPt5));

    pt1.add_child("key2", tmpPt1);

    bpt::ptree pt2 =
        tree()
        ("key1", "value1")
        ("key2", tree()
            ("key3", 12345)
            (tree()
                ("key4", "value4 with spaces"))
            ("key5", "value5")
            (10)(11)(12));

    if (pt1 == pt2) return 0;
    else return 1;
}
