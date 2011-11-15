/* Copyright (C) 2011 Renato Florentino Garcia
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file BOOST_LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 * For more information, see http://www.boost.org
 */
#ifndef _GOLLD_PROPERTY_TREE_PTREE_IO_HPP_
#define _GOLLD_PROPERTY_TREE_PTREE_IO_HPP_

#include <boost/property_tree/ptree.hpp>
#include <ostream>
#include <string>

/// Prints a ptree with a possible prefix spacing if it was a subtree.
template <class charT, class traits>
void insertion(std::basic_ostream<charT,traits> &strm,
    const boost::property_tree::ptree &ptree, const std::string &prefix)
{
    strm << prefix << ptree.data() << '\n'
         << prefix << "{\n";

    boost::property_tree::ptree::const_iterator it = ptree.begin();
    const boost::property_tree::ptree::const_iterator end = ptree.end();
    for (; it != end; ++it) {
        strm << prefix << it->first << " =\n";
        insertion(strm, it->second, prefix + '\t');
        strm << '\n';
    }
    strm << prefix << '}';
}

template <class charT, class traits>
inline std::basic_ostream<charT,traits>&
operator<<(std::basic_ostream<charT,traits> &strm, const boost::property_tree::ptree &ptree)
{
    insertion(strm, ptree, "");
    return strm;
}


#endif /* _GOLLD_PROPERTY_TREE_PTREE_IO_HPP_ */
