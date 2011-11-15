/* Copyright (C) 2011 Renato Florentino Garcia
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file BOOST_LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 * For more information, see http://www.boost.org
 */
#include <boost/array.hpp>
#include <boost/property_tree/ptree.hpp>
#include <vector>

#include <golld/property_tree/assign.hpp>
#include <golld/property_tree/conversion.hpp>

namespace bpt = boost::property_tree;
namespace gpt = golld::property_tree;
using namespace golld::property_tree::assign;

int main(int argc, char *argv[])
{
    {
        const bpt::ptree pt = tree()(1)(2)(3);
        std::vector<int> vec;
        vec.push_back(1);
        vec.push_back(2);
        vec.push_back(3);

        const std::vector<int> result = gpt::toSequence<std::vector<int> >(pt);

        if (result != vec) return -1;
    }

    {
        const bpt::ptree pt = tree()(1)(2)(3);
        const boost::array<int,3> arr = {1, 2, 3};

        const boost::array<int, 3> result = gpt::toArray<int, 3>(pt);

        if (result != arr) return -1;
    }

    {
        const bpt::ptree pt1 =
            tree()
            ("key1", 1)
            ("key2", 2)
            ("key3", 3)
            ("key4", 4);

        const bpt::ptree pt2 =
            tree()
            ("key1", 1)
            ("key2", 2);

        const bpt::ptree pt3 =
            tree()
            ("key3", 3)
            ("key4", 4);

        const bpt::ptree result = gpt::graphUnion(pt2, pt3);

        if (result != pt1) return -1;
    }

    return 0;
}
