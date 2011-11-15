# Copyright (C) 2011 Renato Florentino Garcia
#
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file BOOST_LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
# For more information, see http://www.boost.org

INCLUDE_DIRECTORIES(${INCLUDE_DIRS})

ADD_EXECUTABLE(test_assign test_assign.cpp)
TARGET_LINK_LIBRARIES(test_assign ${LINK_LIBS})
ADD_TEST(test_assign test_assign
  REQUIRES test_assign)

ADD_EXECUTABLE(test_conversion test_conversion.cpp)
TARGET_LINK_LIBRARIES(test_conversion ${LINK_LIBS})
ADD_TEST(test_conversion test_conversion
  REQUIRES test_conversion)

ADD_EXECUTABLE(test_lua test_lua.cpp)
TARGET_LINK_LIBRARIES(test_lua ${LINK_LIBS})
ADD_TEST(test_lua test_lua
  REQUIRES test_lua)
