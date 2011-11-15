/* Copyright (C) 2011 Renato Florentino Garcia
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file BOOST_LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 * For more information, see http://www.boost.org
 */
#ifndef _PYMODULE_PTREE_HPP_
#define _PYMODULE_PTREE_HPP_

#include <boost/property_tree/ptree.hpp>
#include <boost/shared_ptr.hpp>

typedef struct {
    PyObject_HEAD
    boost::shared_ptr<boost::property_tree::ptree> *real_ptree;
    boost::property_tree::ptree *ptree;
    PyObject *attr_dict;
} ptree_object;

#define PyPtree_Check_NUM 0
#define PyPtree_Check_RETURN int
#define PyPtree_Check_PROTO (PyObject *o)

#define PyPtree_FromPtree_NUM 1
#define PyPtree_FromPtree_RETURN PyObject*
#define PyPtree_FromPtree_PROTO (const boost::property_tree::ptree &ptree)

#define PyPtree_API_pointers 2


#ifdef PTREE_MODULE

static PyPtree_Check_RETURN PyPtree_Check PyPtree_Check_PROTO;

static PyPtree_FromPtree_RETURN PyPtree_FromPtree PyPtree_FromPtree_PROTO;

#else

static void **PyPtree_API;

#define PyPtree_Check                                                   \
    (*(PyPtree_Check_RETURN (*)PyPtree_Check_PROTO) PyPtree_API[PyPtree_Check_NUM])

// Constructs a new Python ptree which has a copy of the given boost ptree
#define PyPtree_FromPtree                                               \
    (*(PyPtree_FromPtree_RETURN (*)PyPtree_FromPtree_PROTO) PyPtree_API[PyPtree_FromPtree_NUM])

static int import_ptree()
{
    PyPtree_API = (void**)PyCapsule_Import("golld.property_tree._ptree._C_API", 0);
    return (PyPtree_API != NULL) ? 0 : -1;
}

#endif /* PTREE_MODULE */

#endif /* _PYMODULE_PTREE_HPP_ */
