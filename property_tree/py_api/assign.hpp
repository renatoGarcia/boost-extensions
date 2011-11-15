/* Copyright (C) 2011 Renato Florentino Garcia
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file BOOST_LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 * For more information, see http://www.boost.org
 */
#ifndef _PYMODULE_ASSIGN_HPP_
#define _PYMODULE_ASSIGN_HPP_

#include <boost/property_tree/ptree.hpp>
#include <golld/property_tree/assign.hpp>

typedef struct {
    PyObject_HEAD
    golld::property_tree::assign::tree *tree;
} tree_object;


#define PyTree_Check_NUM 0
#define PyTree_Check_RETURN int
#define PyTree_Check_PROTO (PyObject *o)

#define PyTree_AsPtree_NUM 1
#define PyTree_AsPtree_RETURN boost::property_tree::ptree&
#define PyTree_AsPtree_PROTO (PyObject *o)

#define PyTree_API_pointers 2


#ifdef ASSIGN_MODULE

static PyTree_Check_RETURN PyTree_Check PyTree_Check_PROTO;

static PyTree_AsPtree_RETURN PyTree_AsPtree PyTree_AsPtree_PROTO;

#else

static void **PyTree_API;

#define PyTree_Check                                                    \
    (*(PyTree_Check_RETURN (*)PyTree_Check_PROTO) PyTree_API[PyTree_Check_NUM])

#define PyTree_AsPtree                                                  \
    (*(PyTree_AsPtree_RETURN (*)PyTree_AsPtree_PROTO) PyTree_API[PyTree_AsPtree_NUM])

static int import_assign()
{
    PyTree_API = (void**)PyCapsule_Import("golld.property_tree.assign._C_API", 0);
 
    return (PyTree_API != NULL) ? 0 : -1;
}

#endif /* ASSIGN_MODULE */

#endif /* _PYMODULE_ASSIGN_HPP_ */
