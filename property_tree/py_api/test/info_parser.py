#!/usr/bin/env python2
#
# Copyright (C) 2011 Renato Florentino Garcia
#
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file BOOST_LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
# For more information, see http://www.boost.org

import golld.property_tree.info_parser as gpi

t = gpi.read_info('tree.info')
gpi.write_info('output.info', t)
print(t)
