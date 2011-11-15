/* Copyright (C) 2011 Renato Florentino Garcia
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file BOOST_LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 * For more information, see http://www.boost.org
 */
#include <Python.h>

#define ASSIGN_MODULE
#include "assign.hpp"

#include "_ptree.hpp"


static PyObject* unary_call(tree_object *self, PyObject *args, PyObject *kwds);

static PyObject* binary_call(tree_object *self, PyObject *args, PyObject *kwds);


static PyObject* tree_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    tree_object *self;

    self = (tree_object*)type->tp_alloc(type, 0);
    if (self != NULL) {
        self->tree = new golld::property_tree::assign::tree();
    }

    return (PyObject*)self;
}

static int tree_init(tree_object *self, PyObject *args, PyObject *kwds)
{
    PyObject *pyData = NULL;

    static char *kwlist[] = {(char*)"data", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &pyData)) {
        return -1;
    }

    if (pyData) {
        PyObject *pyString = PyObject_Str(pyData);
        if (pyString == NULL) {
            PyErr_SetString(PyExc_TypeError, "'data' argument must be convertible to string");
            return -1;
        }
        const char *data = PyString_AsString(pyString);
        *(self->tree) = golld::property_tree::assign::tree(data);
    }

    return 0;
}

static void tree_dealloc(tree_object *self)
{
    delete self->tree;
    self->ob_type->tp_free((PyObject*)self);
}

static PyObject* tree_call(tree_object *self, PyObject *args, PyObject *kwds)
{
    const Py_ssize_t args_size = PyTuple_Size(args);
    const Py_ssize_t kwds_size = kwds ? PyDict_Size(kwds) : 0;

    const Py_ssize_t total_size = args_size + kwds_size;

    if (total_size < 1 || total_size > 2) {
        PyErr_Format(PyExc_TypeError, "function takes 1 or 2 arguments (%d given)", total_size);
        return NULL;
    }

    if (total_size == 1) {
        return unary_call(self, args, kwds);
    } else {
        return binary_call(self, args, kwds);
    }
}

static PyObject* tree_getptree(tree_object *self, void *closure)
{
    return PyPtree_FromPtree(*self->tree);
}

static int tree_setptree(tree_object *self, PyObject *value, void *closure)
{
    PyErr_SetString(PyExc_TypeError, "Read only attribute");
    return -1;
}

static PyGetSetDef tree_getseters[] = {
    {(char*)"ptree", (getter)tree_getptree, (setter)tree_setptree,
     (char*)"Constructed ptree object", NULL},
    {NULL}
};

static PyTypeObject tree_type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /* ob_size */
    "assign.tree",             /* tp_name */
    sizeof(tree_object),       /* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)tree_dealloc,  /* tp_dealloc */
    0,                         /* tp_print */
    0,                         /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_compare */
    0,                         /* tp_repr */
    0,                         /* tp_as_number */
    0,                         /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash  */
    (ternaryfunc)tree_call,    /* tp_call */
    0,                         /* tp_str */
    0,                         /* tp_getattro */
    0,                         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,        /* tp_flags */
    "tree class",              /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    0,                         /* tp_methods */
    0,                         /* tp_members */
    tree_getseters,            /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)tree_init,       /* tp_init */
    0,                         /* tp_alloc */
    tree_new,                  /* tp_new */
};

PyMODINIT_FUNC initassign(void)
{
    PyObject *m;
    static void *PyTree_API[PyTree_API_pointers];
    PyObject *c_api_object;

    if (PyType_Ready(&tree_type) < 0)
        return;

    m = Py_InitModule3("assign", NULL, "Python wrapper to Boost ptree assign.");
    if (m == NULL)
        return;

    PyTree_API[PyTree_Check_NUM] = (void*)PyTree_Check;
    PyTree_API[PyTree_AsPtree_NUM] = (void*)PyTree_AsPtree;

    c_api_object = PyCapsule_New((void*)PyTree_API, "golld.property_tree.assign._C_API", NULL);
    if (c_api_object != NULL) {
        PyModule_AddObject(m, "_C_API", c_api_object);
    }

    Py_INCREF(&tree_type);
    PyModule_AddObject(m, "tree", (PyObject*)&tree_type);

    if (import_ptree() < 0)
        return;
}

static PyObject* unary_call(tree_object *self, PyObject *args, PyObject *kwds)
{
    const Py_ssize_t args_size = PyTuple_Size(args);

    PyObject *pyData;
    if (args_size == 1) {
        pyData = PyTuple_GetItem(args, 0);
    } else {
        pyData = PyDict_GetItemString(kwds, "data");
    }

    if (pyData == NULL) {
        PyErr_SetString(PyExc_TypeError, "Required argument 'data' (unique argument) not found");
        return NULL;
    }

    PyObject *pyString = PyObject_Str(pyData);
    if (pyString == NULL) {
        PyErr_SetString(PyExc_TypeError, "'data' argument must be convertible to string");
        return NULL;
    }

    const char * const data = PyString_AsString(pyString);
    if (data == NULL) {
        return NULL;
    }

    (*self->tree)(data);

    Py_INCREF(self);
    return (PyObject*)self;
}

static PyObject* binary_call(tree_object *self, PyObject *args, PyObject *kwds)
{
    const Py_ssize_t args_size = PyTuple_Size(args);

    //------ Get the first argument (path)
    PyObject * const pyString = (args_size >= 1) ?
        PyTuple_GetItem(args, 0) : PyDict_GetItemString(kwds, "path");
    if (pyString == NULL) {
        PyErr_SetString(PyExc_TypeError, "Required argument 'path' (first argument) not found");
        return NULL;
    }
    const char * const path = PyString_AsString(pyString);
    if (path == NULL) {
        return NULL;
    }

    //------ Get the second argument (tree or data)
    const tree_object *tree = NULL;
    const char *data = NULL;

    if (args_size == 2) { // If it is in args
        PyObject * const sec = PyTuple_GetItem(args, 1);
        PyObject *secString = NULL;

        if (PyTree_Check(sec)) {
            tree = (tree_object*)sec;
        } else if (secString = PyObject_Str(sec)){
            data = PyString_AsString(secString);
            Py_DECREF(secString);
        } else {
            PyErr_SetString(PyExc_TypeError, "Second argument must be a tree or a type convertible to a string");
            return NULL;
        }
    } else { // If it is in kwds
        PyObject *sec = NULL;
        PyObject *secString = NULL;

        if (sec = PyDict_GetItemString(kwds, "tree")) {
            if (!PyTree_Check(sec)) {
                PyErr_SetString(PyExc_TypeError, "Named argument tree must be of type tree");
                return NULL;
            }
            tree = (tree_object*)sec;
        } else if (sec = PyDict_GetItemString(kwds, "data")) {
            if ((secString = PyObject_Str(sec)) == NULL) {
                PyErr_SetString(PyExc_TypeError, "Named argument data must be a string");
                return NULL;
            }
            data = PyString_AsString(secString);
            Py_DECREF(secString);
        } else {
            PyErr_SetString(PyExc_TypeError, "Second argument must be named tree or data");
            return NULL;
        }
    }

    if (tree) {
        (*self->tree)(path, *tree->tree);
    } else {
        (*self->tree)(path, data);
    }

    Py_INCREF(self);
    return (PyObject*)self;
}

static int PyTree_Check(PyObject *o)
{
    return PyObject_TypeCheck(o, &tree_type);
}

static boost::property_tree::ptree& PyTree_AsPtree(PyObject *o)
{
    if (!PyTree_Check(o)) {
        PyErr_SetString(PyExc_TypeError, "Type must be a tree");
    }

    tree_object *tree = (tree_object*)o;
    return (boost::property_tree::ptree&)(*tree->tree);
}
