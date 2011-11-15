/* Copyright (C) 2011 Renato Florentino Garcia
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file BOOST_LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 * For more information, see http://www.boost.org
 */
%include exception.i

%header %{

#include <boost/property_tree/ptree.hpp>
#include <golld/property_tree/assign.hpp>

// Header from the Python API
#include <assign.hpp>
#include <_ptree.hpp>

%} // %header

%init %{

// Without this the import_assign will fail.
PyImport_ImportModule("golld.property_tree.assign");

if (import_ptree() < 0) {
    return;
}
if (import_assign() < 0) {
    return;
}

%}

%typemap(in) const boost::property_tree::ptree&
{
    if (PyPtree_Check($input)) {
        ptree_object *ptree = (ptree_object*)$input;
        $1 = new boost::property_tree::ptree(*ptree->ptree);
    } else if (PyTree_Check($input)) {
        tree_object *tree = (tree_object*)$input;
        $1 = new boost::property_tree::ptree(*tree->tree);
    } else {
        $1 = NULL;
        SWIG_exception(SWIG_TypeError, "Expected a ptree or a tree");
    }
}

%typemap(freearg) const boost::property_tree::ptree&
{
    if ($1 != NULL) {
        delete $1;
    }
}

// %typemap(newfree) boost::property_tree::ptree* {
//     delete $1;
// }

%typemap(typecheck) const boost::property_tree::ptree& {
    $1 = (PyPtree_Check($input) || PyTree_Check($input));
}
