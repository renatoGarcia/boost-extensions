/* Copyright (C) 2011 Renato Florentino Garcia
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file BOOST_LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 * For more information, see http://www.boost.org
 */
#include <Python.h>
#include "_ptree.hpp"
#include <boost/property_tree/xml_parser.hpp>

static PyObject *xml_parser_error;

static PyObject* read_xml(ptree_object *self, PyObject *args, PyObject *kwds)
{
    const char *filename = NULL;
    int flags = 0;

    static char *kwlist[] = {(char*)"filename", (char*)"flags", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s|i", kwlist, &filename, &flags)) {
        return NULL;
    }

    boost::property_tree::ptree pt;
    try
    {
        boost::property_tree::read_xml(filename, pt, flags);
    }
    catch (const boost::property_tree::xml_parser_error &e) {
        PyErr_SetString(xml_parser_error, e.message().c_str());
        return NULL;
    }

    return PyPtree_FromPtree(pt);
}

static PyObject* write_xml(ptree_object *self, PyObject *args, PyObject *kwds)
{
    const char *filename = NULL;
    ptree_object *ptree = NULL;

    PyObject *tmp = NULL;
    static char *kwlist[] = {(char*)"filename", (char*)"ptree", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "sO", kwlist, &filename, &tmp)) {
        return NULL;
    }

    if (!PyPtree_Check(tmp)) {
        PyErr_SetString(xml_parser_error, "ptree argument must be a ptree class object.");
        return NULL;
    }
    ptree = reinterpret_cast<ptree_object*>(tmp);

    try
    {
        boost::property_tree::write_xml(filename, *ptree->ptree);
    }
    catch (const boost::property_tree::xml_parser_error &e) {
        PyErr_SetString(xml_parser_error, e.message().c_str());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyMethodDef functions[] = {
    {"read_xml", (PyCFunction)read_xml, METH_VARARGS | METH_KEYWORDS,
     "read_xml(filename, flags=None) -> ptree\n\
\n\
Read XML from a the given file and translate it to a property tree. XML attributes are \
placed under keys named <xmlattr>.\n\
\n\
Parameters:\n\
filename - Name of file from which to read in the property tree.\n\
flags - Flags controlling the bahviour of the parser. The following flags are \
supported:\n\
\tno_concat_text -- Prevents concatenation of text nodes into datastring of property \
tree. Puts them in separate <xmltext> strings instead.\n\
\tno_comments -- Skip XML comments.\n\
\n\
Throws:\n\
xml_parser_error - In case of error deserializing the property tree."},
    {"write_xml", (PyCFunction)write_xml, METH_VARARGS | METH_KEYWORDS,
     "write_xml(filename, ptree) -> None\n\
\n\
Translates the property tree to XML and writes it the given file.\n\
\n\
Parameters:\n\
filename - The file to which to write the XML representation of the property tree.\n\
ptree - The property tree to tranlsate to XML and output.\n\
\n\
Throws:\n\
info_parser_error - xml_parser_error In case of error translating the property tree to XML or writing to the output stream."},
    {NULL}
};

PyMODINIT_FUNC initxml_parser(void)
{
    PyObject *pt_mod = PyImport_ImportModule("golld.property_tree._ptree");
    PyObject *file_parser_error = PyObject_GetAttrString(pt_mod, "file_parser_error");

    PyObject *m;

    m = Py_InitModule3("xml_parser", functions,
                       "Python wrapper to Boost Property Tree XML parser.");
    if (m == NULL)
        return;

    xml_parser_error = PyErr_NewException((char*)"xml_parser.xml_parser_error", file_parser_error, NULL);

    Py_INCREF(xml_parser_error);
    PyModule_AddObject(m, "xml_parser_error", xml_parser_error);

    PyModule_AddIntConstant(m, "no_concat_text", boost::property_tree::xml_parser::no_concat_text);
    PyModule_AddIntConstant(m, "no_comments", boost::property_tree::xml_parser::no_comments);
    PyModule_AddIntConstant(m, "trim_whitespace", boost::property_tree::xml_parser::trim_whitespace);

    if (import_ptree() < 0)
        return;
}
