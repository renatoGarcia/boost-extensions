#!/usr/bin/env python2
#
# Copyright (C) 2011 Renato Florentino Garcia
#
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file BOOST_LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
# For more information, see http://www.boost.org

import golld.property_tree
from golld.property_tree.assign import tree

pt = (tree()
         ('key1', 1)
         ('key2', 2)
         ('subtree', tree()(1)(2)(3))
     ).ptree

pt.add('aaa', 'bbb')

print pt
