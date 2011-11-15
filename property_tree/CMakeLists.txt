# Copyright (C) 2011 Renato Florentino Garcia
#
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file BOOST_LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
# For more information, see http://www.boost.org
cmake_minimum_required(VERSION 2.8)

SET(GOLLD_PROPERTY_TREE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include
  CACHE PATH "Directory for Golld property_tree module include files.")
MARK_AS_ADVANCED(GOLLD_PROPERTY_TREE_INCLUDE_DIR)

SET(INCLUDE_DIRS ${GOLLD_BASE_INCLUDE_DIR} ${GOLLD_PROPERTY_TREE_INCLUDE_DIR})
SET(LINK_LIBS)

FIND_PACKAGE(Boost REQUIRED)
LIST(APPEND INCLUDE_DIRS ${Boost_INCLUDE_DIR})


#------------ Check for Lua 5.1
IF(WITH_LUA)
  FIND_PACKAGE(Lua51 REQUIRED)
ELSEIF(NOT DEFINED WITH_LUA)
  FIND_PACKAGE(Lua51)
ENDIF()

IF(LUA51_FOUND)
  SET(_WITH_LUA ON)
  LIST(APPEND INCLUDE_DIRS ${LUA_INCLUDE_DIR})
  LIST(APPEND LINK_LIBS ${LUA_LIBRARIES})
ELSE()
  SET(_WITH_LUA OFF)
ENDIF()
SET(WITH_LUA ${_WITH_LUA}
  CACHE BOOL "Lua language library to use in property_tree parsing.")
#-------------------------------


ADD_SUBDIRECTORY(py_api)
ADD_SUBDIRECTORY(test EXCLUDE_FROM_ALL)
