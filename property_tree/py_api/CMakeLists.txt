# Copyright (C) 2011 Renato Florentino Garcia
#
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file BOOST_LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
# For more information, see http://www.boost.org

SET(GOLLD_PROPERTY_TREE_PYAPI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
  CACHE PATH "Directory for Golld property_tree module Python API include files.")
MARK_AS_ADVANCED(GOLLD_PROPERTY_TREE_PYAPI_INCLUDE_DIR)

INCLUDE_DIRECTORIES(${INCLUDE_DIRS})

FIND_PACKAGE(Boost REQUIRED)
FIND_PACKAGE(PythonLibs)
FIND_PACKAGE(PythonInterp)

#Python must be at least 2.6
IF(PYTHONLIBS_FOUND AND PYTHONINTERP_FOUND)

  IF(PYTHON_OS_WIN)
    GET_FILENAME_COMPONENT(playercpyInstallDir ${PYTHON_EXECUTABLE} PATH)
  ELSE(PYTHON_OS_WIN)
    # Get the Python version
    EXECUTE_PROCESS(COMMAND ${PYTHON_EXECUTABLE} -V
                    ERROR_VARIABLE pythonVersionString
                    ERROR_STRIP_TRAILING_WHITESPACE)
    STRING(REGEX REPLACE "^Python ([0-9]+\\.[0-9]+).*" "\\1"
           pythonVersion ${pythonVersionString})
  ENDIF(PYTHON_OS_WIN)

  SET(PYTHON_BINDING_INSTALL_DIR lib/python${pythonVersion}/site-packages
      CACHE PATH "Python binding installation directory under $prefix")
  MARK_AS_ADVANCED (PYTHON_BINDING_INSTALL_DIR)

  INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS})

  ADD_LIBRARY(_ptree SHARED _ptree.cpp)
  SET_PROPERTY(TARGET _ptree PROPERTY PREFIX "")

  ADD_LIBRARY(info_parser SHARED info_parser.cpp)
  SET_PROPERTY(TARGET info_parser PROPERTY PREFIX "")

  ADD_LIBRARY(ini_parser SHARED ini_parser.cpp)
  SET_PROPERTY(TARGET ini_parser PROPERTY PREFIX "")

  ADD_LIBRARY(json_parser SHARED json_parser.cpp)
  SET_PROPERTY(TARGET json_parser PROPERTY PREFIX "")

  ADD_LIBRARY(xml_parser SHARED xml_parser.cpp)
  SET_PROPERTY(TARGET xml_parser PROPERTY PREFIX "")

  IF(WITH_LUA)
    ADD_LIBRARY(lua_parser SHARED lua_parser.cpp)
    SET_PROPERTY(TARGET lua_parser PROPERTY PREFIX "")
    TARGET_LINK_LIBRARIES(lua_parser ${LINK_LIBS})
  ENDIF()

  ADD_LIBRARY(assign SHARED assign.cpp)
  SET_PROPERTY(TARGET assign PROPERTY PREFIX "")

ENDIF(PYTHONLIBS_FOUND AND PYTHONINTERP_FOUND)