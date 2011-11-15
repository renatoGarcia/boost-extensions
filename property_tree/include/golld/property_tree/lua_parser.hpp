/* Copyright (C) 2011 Renato Florentino Garcia
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file BOOST_LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 * For more information, see http://www.boost.org
 */
#ifndef _GOLLD_PROPERTY_TREE_LUA_PARSER_HPP_
#define _GOLLD_PROPERTY_TREE_LUA_PARSER_HPP_

#include <boost/property_tree/ptree.hpp>
#include <golld/property_tree/detail/lua_parser_error.hpp>
#include <golld/property_tree/detail/lua_parser_read.hpp>
#include <golld/property_tree/detail/lua_parser_write.hpp>
#include <string>
#include <ostream>
#include <fstream>

namespace golld {
namespace property_tree {
namespace lua_parser {

template<typename Ptree>
void read_lua(std::basic_istream<typename Ptree::key_type::value_type> &stream,
              const std::string &rootKey,  Ptree &pt)
{
    read_lua_internal(stream, rootKey, pt, std::string());
}

template<class Ptree>
void read_lua(const std::string &filename, const std::string &rootKey, Ptree &pt,
              const std::locale &loc = std::locale())
{
    std::basic_ifstream<typename Ptree::key_type::value_type> stream(filename.c_str());
    if (!stream) {
        throw lua_parser_error("cannot open file", filename);
    }
    stream.imbue(loc);

    read_lua_internal(stream, rootKey, pt, filename);
}

template<class Ptree>
void write_lua(std::basic_ostream<typename Ptree::key_type::value_type> &stream,
               const Ptree &pt)
{
    write_lua_internal(stream, pt, std::string());
}

template<class Ptree>
void write_lua(const std::string &filename, const Ptree &pt, const std::locale &loc = std::locale())
{
    std::basic_ofstream<typename Ptree::key_type::value_type> stream(filename.c_str());
    if (!stream) {
        throw lua_parser_error("cannot open file", filename);
    }
    stream.imbue(loc);

    write_lua_internal(stream, pt, filename);
}

} // namespace lua_parser
} // namespace property_tree
} // namespace golld

namespace golld {
namespace property_tree {

using lua_parser::read_lua;
using lua_parser::write_lua;
using lua_parser::lua_parser_error;

} // namespace property_tree
} // namespace golld

#endif /* _GOLLD_PROPERTY_TREE_LUA_PARSER_HPP_ */
