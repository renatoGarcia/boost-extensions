/* Copyright (C) 2011 Renato Florentino Garcia
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file BOOST_LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 * For more information, see http://www.boost.org
 */
#include <Python.h>
#include "_ptree.hpp"
#include <boost/property_tree/ini_parser.hpp>

static PyObject *ini_parser_error;

static PyObject* read_ini(ptree_object *self, PyObject *args, PyObject *kwds)
{
    const char *filename = NULL;

    static char *kwlist[] = {(char*)"filename", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &filename)) {
        return NULL;
    }

    boost::property_tree::ptree pt;
    try
    {
        boost::property_tree::read_ini(filename, pt);
    }
    catch (const boost::property_tree::ini_parser_error &e) {
        PyErr_SetString(ini_parser_error, e.message().c_str());
        return NULL;
    }

    return PyPtree_FromPtree(pt);
}

static PyObject* write_ini(ptree_object *self, PyObject *args, PyObject *kwds)
{
    const char *filename = NULL;
    ptree_object *ptree = NULL;

    PyObject *tmp = NULL;
    static char *kwlist[] = {(char*)"filename", (char*)"ptree", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "sO", kwlist, &filename, &tmp)) {
        return NULL;
    }

    if (!PyPtree_Check(tmp)) {
        PyErr_SetString(ini_parser_error, "ptree argument must be a ptree class object.");
        return NULL;
    }
    ptree = reinterpret_cast<ptree_object*>(tmp);

    try
    {
        boost::property_tree::write_ini(filename, *ptree->ptree);
    }
    catch (const boost::property_tree::ini_parser_error &e) {
        PyErr_SetString(ini_parser_error, e.message().c_str());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyMethodDef functions[] = {
    {"read_ini", (PyCFunction)read_ini, METH_VARARGS | METH_KEYWORDS,
     "read_ini(filename) -> ptree\n\
\n\
Read INI from a the given file and translate it to a property tree.\n\
\n\
Parameters:\n\
filename - Name of file from which to read in the property tree.\n\
\n\
Throws:\n\
ini_parser_error - In case of error deserializing the property tree."},
    {"write_ini", (PyCFunction)write_ini, METH_VARARGS | METH_KEYWORDS,
     "write_ini(filename, ptree) -> None\n\
\n\
Translates the property tree to INI and writes it the given file.\n\
\n\
Parameters:\n\
filename - The name of the file to which to write the INI representation of the property tree.\n\
ptree - The property tree to tranlsate to INI and output.\n\
\n\
Requires:\n\
pt cannot have data in its root.\n\
pt cannot have keys both data and children.\n\
pt cannot be deeper than two levels.\n\
There cannot be duplicate keys on any given level of pt.\n\
\n\
Throws:\n\
info_parser_error - In case of error translating the property tree to INI or writing to the file."},
    {NULL}
};

PyMODINIT_FUNC initini_parser()
{
    PyObject *pt_mod = PyImport_ImportModule("golld.property_tree._ptree");
    PyObject *file_parser_error = PyObject_GetAttrString(pt_mod, "file_parser_error");

    PyObject *m;
    m = Py_InitModule3("ini_parser", functions,
                       "Python wrapper to Boost Property Tree INI parser.");
    if (m == NULL)
        return;

    ini_parser_error = PyErr_NewException((char*)"ini_parser.ini_parser_error", file_parser_error, NULL);

    Py_INCREF(ini_parser_error);
    PyModule_AddObject(m, "ini_parser_error", ini_parser_error);

    if (import_ptree() < 0)
        return;
}
