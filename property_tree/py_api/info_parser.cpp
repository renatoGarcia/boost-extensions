/* Copyright (C) 2011 Renato Florentino Garcia
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file BOOST_LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 * For more information, see http://www.boost.org
 */
#include <Python.h>
#include "_ptree.hpp"
#include <boost/property_tree/info_parser.hpp>

static PyObject *info_parser_error;

static PyObject* read_info(ptree_object *self, PyObject *args, PyObject *kwds)
{
    const char *filename = NULL;
    PyObject *default_ptree = NULL;

    static char *kwlist[] = {(char*)"filename", (char*)"default_ptree", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s|O", kwlist, &filename, &default_ptree)) {
        return NULL;
    }

    boost::property_tree::ptree pt;
    try
    {
        if (default_ptree) {
            ptree_object *dpt = reinterpret_cast<ptree_object*>(default_ptree);
            boost::property_tree::read_info(filename, pt, *dpt->ptree);
        } else {
            boost::property_tree::read_info(filename, pt);
        }
    }
    catch (const boost::property_tree::info_parser_error &e) {
        PyErr_SetString(info_parser_error, e.message().c_str());
        return NULL;
    }

    return PyPtree_FromPtree(pt);
}

static PyObject* write_info(ptree_object *self, PyObject *args, PyObject *kwds)
{
    const char *filename = NULL;
    ptree_object *ptree = NULL;

    PyObject *tmp = NULL;
    static char *kwlist[] = {(char*)"filename", (char*)"ptree", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "sO", kwlist, &filename, &tmp)) {
        return NULL;
    }

    if (!PyPtree_Check(tmp)) {
        PyErr_SetString(info_parser_error, "ptree argument must be a ptree class object.");
        return NULL;
    }
    ptree = reinterpret_cast<ptree_object*>(tmp);

    try
    {
        boost::property_tree::write_info(filename, *ptree->ptree);
    }
    catch (const boost::property_tree::info_parser_error &e) {
        PyErr_SetString(info_parser_error, e.message().c_str());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyMethodDef functions[] = {
    {"read_info", (PyCFunction)read_info, METH_VARARGS | METH_KEYWORDS,
     "read_info(filename, default_ptree=None) -> ptree\n\
\n\
Read INFO from a the given file and translate it to a property tree.\n\
\n\
Parameters:\n\
filename - Name of file from which to read in the property tree.\n\
default_ptree - If parsing fails, pt is set to a copy of this tree.\n\
\n\
Throws:\n\
info_parser_error If the file cannot be read, doesn't contain valid INFO, or a\n\
conversion fails."},
    {"write_info", (PyCFunction)write_info, METH_VARARGS | METH_KEYWORDS,
     "write_info(filename, ptree) -> None\n\
\n\
Writes a tree to the file in INFO format. The tree's key type must be a string type, i.e. it must have a nested value_type typedef that is a valid parameter for basic_ofstream.\n\
\n\
Parameters:\n\
filename - The name of the file to which to write the INFO representation of the property tree.\n\
ptree - The property tree to translate to INFO and output.\n\
\n\
Throws:\n\
info_parser_error - If the file cannot be written to, or a conversion fails."},
    {NULL}
};

PyMODINIT_FUNC initinfo_parser()
{
    PyObject *pt_mod = PyImport_ImportModule("golld.property_tree._ptree");
    PyObject *file_parser_error = PyObject_GetAttrString(pt_mod, "file_parser_error");

    PyObject *m;
    m = Py_InitModule3("info_parser", functions,
                       "Python wrapper to Boost Property Tree INFO parser.");
    if (m == NULL)
        return;

    info_parser_error = PyErr_NewException((char*)"info_parser.info_parser_error", file_parser_error, NULL);

    Py_INCREF(info_parser_error);
    PyModule_AddObject(m, "info_parser_error", info_parser_error);

    if (import_ptree() < 0)
        return;
}
