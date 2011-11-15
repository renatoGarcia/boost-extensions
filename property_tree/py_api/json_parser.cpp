/* Copyright (C) 2011 Renato Florentino Garcia
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file BOOST_LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 * For more information, see http://www.boost.org
 */
#include <Python.h>
#include "_ptree.hpp"
#include <boost/property_tree/json_parser.hpp>

static PyObject *json_parser_error;

static PyObject* read_json(ptree_object *self, PyObject *args, PyObject *kwds)
{
    const char *filename = NULL;

    static char *kwlist[] = {(char*)"filename", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &filename)) {
        return NULL;
    }

    boost::property_tree::ptree pt;
    try
    {
        boost::property_tree::read_json(filename, pt);
    }
    catch (const boost::property_tree::json_parser_error &e) {
        PyErr_SetString(json_parser_error, e.message().c_str());
        return NULL;
    }

    return PyPtree_FromPtree(pt);
}

static PyObject* write_json(ptree_object *self, PyObject *args, PyObject *kwds)
{
    const char *filename = NULL;
    ptree_object *ptree = NULL;

    PyObject *tmp = NULL;
    static char *kwlist[] = {(char*)"filename", (char*)"ptree", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "sO", kwlist, &filename, &tmp)) {
        return NULL;
    }

    if (!PyPtree_Check(tmp)) {
        PyErr_SetString(json_parser_error, "ptree argument must be a ptree class object.");
        return NULL;
    }
    ptree = reinterpret_cast<ptree_object*>(tmp);

    try
    {
        boost::property_tree::write_json(filename, *ptree->ptree);
    }
    catch (const boost::property_tree::json_parser_error &e) {
        PyErr_SetString(json_parser_error, e.message().c_str());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyMethodDef functions[] = {
    {"read_json", (PyCFunction)read_json, METH_VARARGS | METH_KEYWORDS,
     "read_json(filename) -> ptree\n\
\n\
Read JSON from a the given file and translate it to a property tree.\n\
\n\
Parameters:\n\
filename - Name of file from which to read in the property tree.\n\
\n\
Throws:\n\
json_parser_error - In case of error deserializing the property tree."},
    {"write_json", (PyCFunction)write_json, METH_VARARGS | METH_KEYWORDS,
     "write_json(filename, ptree) -> None\n\
\n\
Translates the property tree to JSON and writes it the given file.\n\
\n\
Parameters:\n\
filename - The name of the file to which to write the JSON representation of the property tree.\n\
ptree - The property tree to translate to JSON and output.\n\
\n\
Requires:\n\
pt cannot contain keys that have both subkeys and non-empty data.\n\
\n\
Throws:\n\
json_parser_error - In case of error translating the property tree to JSON or writing to the file."},
    {NULL}
};

PyMODINIT_FUNC initjson_parser()
{
    PyObject *pt_mod = PyImport_ImportModule("golld.property_tree._ptree");
    PyObject *file_parser_error = PyObject_GetAttrString(pt_mod, "file_parser_error");

    PyObject *m;
    m = Py_InitModule3("json_parser", functions,
                       "Python wrapper to Boost Property Tree JSON parser.");
    if (m == NULL)
        return;

    json_parser_error = PyErr_NewException((char*)"json_parser.json_parser_error", file_parser_error, NULL);


    Py_INCREF(json_parser_error);
    PyModule_AddObject(m, "json_parser_error", json_parser_error);

    if (import_ptree() < 0)
        return;
}
