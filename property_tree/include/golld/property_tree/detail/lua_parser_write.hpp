/* Copyright (C) 2011 Renato Florentino Garcia
 * Copyright (C) 2002-2006 Marcin Kalicinski
 *
 * This file is an adaptation of
 * boost/property_tree/detail/json_parser_write.hpp wrote by
 * Marcin Kalicinski
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file BOOST_LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 * For more information, see http://www.boost.org
 */
#ifndef _GOLLD_PROPERTY_TREE_DETAIL_LUA_PARSER_WRITE_HPP_
#define _GOLLD_PROPERTY_TREE_DETAIL_LUA_PARSER_WRITE_HPP_

#include <string>
#include <boost/property_tree/ptree.hpp>

namespace golld {
namespace property_tree {
namespace lua_parser {

template<class Ptree>
void write_lua_helper(std::basic_ostream<typename Ptree::key_type::value_type> &stream,
                      const Ptree &pt, int indent)
{
    typedef typename Ptree::key_type::value_type Ch;
    typedef typename std::basic_string<Ch> Str;

    if (indent == 0) {
        stream << pt.template get_value<Str>() << Ch('=');
    }

    if (pt.empty()) {
        stream << Ch('\'') << pt.template get_value<Str>() << Ch('\'');
    }
    else {
        stream << Ch('{') << Ch('\n');
        typename Ptree::const_iterator it = pt.begin();
        for (; it != pt.end(); ++it) {
            stream << Str(4 * (indent + 1), Ch(' '));
            if (!it->first.empty()) {
                stream << Str("['") << it->first << Str("'] = ");
            }
            write_lua_helper(stream, it->second, indent + 1);
            stream << Ch(',');
            stream << Ch('\n');
        }
        stream << Str(4 * indent, Ch(' '));
        stream << Ch('}');
    }
}

// Verify if ptree does not contain information that cannot be written to a Lua table
template<class Ptree>
bool verify_lua(const Ptree &pt, int depth)
{
    typedef typename Ptree::key_type::value_type Ch;
    typedef typename std::basic_string<Ch> Str;

    // Root ptree must have data
    if (depth == 0 && pt.template get_value<Str>().empty())
        return false;

    // Ptree cannot have both children and data
    if (!pt.template get_value<Str>().empty() && !pt.empty() && depth != 0)
        return false;

    // Check children
    typename Ptree::const_iterator it = pt.begin();
    for (; it != pt.end(); ++it)
        if (!verify_lua(it->second, depth + 1))
            return false;

    // Success
    return true;
}

template<class Ptree>
void write_lua_internal(std::basic_ostream<typename Ptree::key_type::value_type> &stream,
                        const Ptree &pt, const std::string &filename)
{
    if (!verify_lua(pt, 0)) {
        throw lua_parser_error("ptree contains data that cannot be represented in a Lua table", filename);
    }

    write_lua_helper(stream, pt, 0);

    stream << std::endl;
}

} // namespace lua_parser
} // namespace property_tree
} // namespace golld

#endif /* _GOLLD_PROPERTY_TREE_DETAIL_LUA_PARSER_WRITE_HPP_ */
