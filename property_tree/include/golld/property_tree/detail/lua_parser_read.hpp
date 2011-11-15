/* Copyright (C) 2011 Renato Florentino Garcia
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file BOOST_LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 * For more information, see http://www.boost.org
 */
#ifndef _GOLLD_PROPERTY_TREE_DETAIL_LUA_PARSER_READ_HPP_
#define _GOLLD_PROPERTY_TREE_DETAIL_LUA_PARSER_READ_HPP_

#include <boost/property_tree/ptree.hpp>
#include <string>

#include "lua_parser_error.hpp"

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

namespace golld {
namespace property_tree {
namespace lua_parser {

void read_data(lua_State *L, boost::property_tree::ptree &pt)
{
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        int key_type = lua_type(L, -2);
        int value_type = lua_type(L, -1);

        if (key_type != LUA_TSTRING && key_type != LUA_TNUMBER) {
            lua_pop(L, 1);
            throw lua_parser_error("Support only string or number keys", "");
        }
        lua_pushvalue(L, -2);
        std::string key_string(lua_tostring(L, -1));
        lua_pop(L, 1);

        boost::property_tree::ptree tmp;
        switch (value_type) {
        case LUA_TTABLE:
            read_data(L, tmp);
            break;
        case LUA_TSTRING:
            {
                std::string value(lua_tostring(L, -1));
                tmp.put_value(value);
                break;
            }
        case LUA_TNUMBER:
            {
                lua_Number value = lua_tonumber(L, -1);
                tmp.put_value(value);
                break;
            }
        case LUA_TBOOLEAN:
            {
                bool value = lua_toboolean(L, -1);
                tmp.put_value(value);
                break;
            }
        default:
            lua_pop(L, 1);
            throw lua_parser_error("The value type must be number or boolean or table or string", "");
        }

        pt.push_back(std::make_pair(key_string, tmp));
        lua_pop(L, 1);
    }
}

template<class Ptree>
void read_lua_internal(std::basic_istream<typename Ptree::key_type::value_type> &stream,
                       const std::string &rootKey, Ptree &pt, const std::string &filename)
{
    typedef typename Ptree::key_type::value_type Ch;
    typedef typename std::basic_string<Ch> Str;

    pt.clear();

    std::basic_string<Ch> s(std::istreambuf_iterator<Ch>(stream.rdbuf()),
                            std::istreambuf_iterator<Ch>());
    if (!stream.good()) {
        throw lua_parser_error("read error", filename);
    }

    lua_State *L = NULL;

    L = luaL_newstate();
    if (L == NULL) {
        throw lua_parser_error("Error on creating Lua environment", filename);
    }

    luaL_openlibs(L);

    if (luaL_dostring(L, s.c_str())) {
        std::string desc(lua_tostring(L, -1));
        lua_close(L);
        throw lua_parser_error(desc, filename);
    }

    lua_getglobal(L, rootKey.c_str());
    if (lua_isnil(L, -1)) {
        lua_close(L);
        throw lua_parser_error("Root key not found", filename);
    }

    pt.put_value(rootKey);

    try {
        read_data(L, pt);
    }
    catch (const lua_parser_error &e) {
        lua_close(L);
        throw lua_parser_error(e.message(), filename);
    }

    lua_pop(L, 1);
    lua_close(L);
}

} // namespace lua_parser
} // namespace property_tree
} // namespace golld

#endif /* _GOLLD_PROPERTY_TREE_DETAIL_LUA_PARSER_READ_HPP_ */
