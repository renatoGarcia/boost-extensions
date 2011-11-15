/* Copyright (C) 2011 Renato Florentino Garcia
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file BOOST_LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 * For more information, see http://www.boost.org
 */
#ifndef _GOLLD_PROPERTY_TREE_CONVERSION_HPP_
#define _GOLLD_PROPERTY_TREE_CONVERSION_HPP_

#include <boost/array.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <algorithm>
#include <stdexcept>

namespace golld {
namespace property_tree {

namespace detail {

template <class T, class Ptree>
struct pair2data
    : public std::unary_function<const typename Ptree::value_type, T>
{
    T operator()(const typename Ptree::value_type &pair) const
    {
        return pair.second.template get_value<T>();
    }
};

}

template <class Sequence>
Sequence toSequence(const boost::property_tree::ptree &ptree)
{
    namespace bpt = boost::property_tree;
    typedef detail::pair2data<typename Sequence::value_type, bpt::ptree> Concrete_pair2data;
    typedef boost::transform_iterator<Concrete_pair2data, bpt::ptree::const_iterator> Iterator;

    const Iterator begin(ptree.begin(), Concrete_pair2data());
    const Iterator end(ptree.end(), Concrete_pair2data());

    return Sequence(begin, end);
}

template <class T, std::size_t N>
boost::array<T, N> toArray(const boost::property_tree::ptree &ptree)
{
    namespace bpt = boost::property_tree;
    typedef detail::pair2data<T, bpt::ptree> Concrete_pair2data;
    typedef boost::transform_iterator<Concrete_pair2data, bpt::ptree::const_iterator> Iterator;

    if (ptree.size() != N) {
        throw std::range_error("Array size error.");
    }

    const Iterator begin(ptree.begin(), Concrete_pair2data());
    const Iterator end(ptree.end(), Concrete_pair2data());

    boost::array<T, N> tmpArray;
    std::copy(begin, end, tmpArray.begin());

    return tmpArray;
}

template<class Ptree>
Ptree graphUnion(const Ptree& pt1, const Ptree& pt2)
{
    Ptree result;

    BOOST_FOREACH(const typename Ptree::value_type &v, pt1) {
        result.add_child(v.first, v.second);
    }

    BOOST_FOREACH(const typename Ptree::value_type &v, pt2) {
        result.add_child(v.first, v.second);
    }

    return result;
}

} // namespace property_tree
} // namespace golld

#endif /* _GOLLD_PROPERTY_TREE_CONVERSION_HPP_ */
