/* Copyright (C) 2011 Renato Florentino Garcia
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file BOOST_LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 * For more information, see http://www.boost.org
 */
#ifndef _GOLLD_PROPERTY_TREE_DETAIL_LUA_PARSER_ERROR_HPP_
#define _GOLLD_PROPERTY_TREE_DETAIL_LUA_PARSER_ERROR_HPP_

#include <boost/property_tree/detail/file_parser_error.hpp>
#include <string>

namespace golld {
namespace property_tree {
namespace lua_parser {

class lua_parser_error
    : public boost::property_tree::file_parser_error
{
public:
    lua_parser_error(const std::string &message, const std::string &filename)
        : file_parser_error(message, filename, 0)
    { }
};

} // namespace lua_parser
} // namespace property_tree
} // namespace golld

#endif /* _GOLLD_PROPERTY_TREE_DETAIL_LUA_PARSER_ERROR_HPP_ */
