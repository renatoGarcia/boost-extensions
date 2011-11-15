/* Copyright (C) 2011 Renato Florentino Garcia
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file BOOST_LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 * For more information, see http://www.boost.org
 */
#include <Python.h>
#include "_ptree.hpp"
#include <golld/property_tree/lua_parser.hpp>

static PyObject *lua_parser_error;

static PyObject* read_lua(ptree_object *self, PyObject *args, PyObject *kwds)
{
    const char *filename = NULL;
    const char *rootKey = NULL;

    static char *kwlist[] = {(char*)"filename", (char*)"rootKey", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "ss", kwlist, &filename, &rootKey)) {
        return NULL;
    }

    boost::property_tree::ptree pt;
    try
    {
        golld::property_tree::read_lua(filename, rootKey, pt);
    }
    catch (const golld::property_tree::lua_parser_error &e) {
        PyErr_SetString(lua_parser_error, e.message().c_str());
        return NULL;
    }

    return PyPtree_FromPtree(pt);
}

static PyObject* write_lua(ptree_object *self, PyObject *args, PyObject *kwds)
{
    const char *filename = NULL;
    ptree_object *ptree = NULL;

    PyObject *tmp = NULL;
    static char *kwlist[] = {(char*)"filename", (char*)"ptree", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "sO", kwlist, &filename, &tmp)) {
        return NULL;
    }

    if (!PyPtree_Check(tmp)) {
        PyErr_SetString(lua_parser_error, "ptree argument must be a ptree class object.");
        return NULL;
    }
    ptree = reinterpret_cast<ptree_object*>(tmp);

    try
    {
        golld::property_tree::write_lua(filename, *ptree->ptree);
    }
    catch (const golld::property_tree::lua_parser_error &e) {
        PyErr_SetString(lua_parser_error, e.message().c_str());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyMethodDef functions[] = {
    {"read_lua", (PyCFunction)read_lua, METH_VARARGS | METH_KEYWORDS,
     "read_lua(filename, rootKey) -> ptree\n\
\n\
Read a Lua table from a the given file and translate it to a property tree.\n\
\n\
Parameters:\n\
filename - Name of file from which to read in the property tree.\n\
rootKey - Name of the table to be translated to a ptree.\n\
\n\
Throws:\n\
lua_parser_error - In case of error deserializing the property tree."},
    {"write_lua", (PyCFunction)write_lua, METH_VARARGS | METH_KEYWORDS,
     "write_lua(filename, ptree) -> None\n\
\n\
Translates the property tree to a Lua table and writes it the given file.\n\
\n\
Parameters:\n\
filename - The name of the file to which to write the Lua table representation of the property tree.\n\
ptree - The property tree to translate to a Lua table and output.\n\
\n\
Throws:\n\
lua_parser_error - In case of error translating the property tree to a Lua table or writing to the file."},
    {NULL}
};

PyMODINIT_FUNC initlua_parser()
{
    PyObject *pt_mod = PyImport_ImportModule("golld.property_tree._ptree");
    PyObject *file_parser_error = PyObject_GetAttrString(pt_mod, "file_parser_error");

    PyObject *m;
    m = Py_InitModule3("lua_parser", functions,
                       "Python wrapper to Golld Property Tree Lua parser.");
    if (m == NULL)
        return;

    lua_parser_error = PyErr_NewException((char*)"lua_parser.lua_parser_error", file_parser_error, NULL);


    Py_INCREF(lua_parser_error);
    PyModule_AddObject(m, "lua_parser_error", lua_parser_error);

    if (import_ptree() < 0)
        return;
}
