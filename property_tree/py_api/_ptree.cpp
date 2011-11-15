/* Copyright (C) 2011 Renato Florentino Garcia
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file BOOST_LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 * For more information, see http://www.boost.org
 */
#include <Python.h>

#define PTREE_MODULE
#include "_ptree.hpp"

#include <boost/numeric/conversion/cast.hpp>
#include <golld/property_tree/ptree_io.hpp>
#include <golld/property_tree/conversion.hpp>
#include <sstream>

static PyObject *ptree_error;
static PyObject *ptree_bad_path;
static PyObject *file_parser_error;

static PyObject* makeRef(boost::shared_ptr<boost::property_tree::ptree> *real_ptree,
                         boost::property_tree::ptree *ptree);

typedef struct {
    PyObject_HEAD
    ptree_object *ptree;
    boost::property_tree::ptree::iterator *current;
    boost::property_tree::ptree::iterator *end;
} iterator_object;

typedef struct {
    PyObject_HEAD
    ptree_object *ptree;
    boost::property_tree::ptree::reverse_iterator *current;
    boost::property_tree::ptree::reverse_iterator *end;
} reverse_iterator_object;

typedef struct {
    PyObject_HEAD
    ptree_object *ptree;
    boost::property_tree::ptree::assoc_iterator *current;
    boost::property_tree::ptree::assoc_iterator *end;
} assoc_iterator_object;

PyObject* make_iterator(ptree_object *ptree);
PyObject* make_reverse_iterator(ptree_object *ptree);
PyObject* make_assoc_iterator(ptree_object *ptree);

//------------------------------------------------ PTREE

static PyObject* ptree_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
static int ptree_init(ptree_object *self, PyObject *args, PyObject *kwds);
static void ptree_dealloc(ptree_object *self);
static PyObject* ptree_str(ptree_object *self);
static PyObject* ptree_size(ptree_object *self);
static PyObject* ptree_max_size(ptree_object *self);
static PyObject* ptree_empty(ptree_object *self);
static PyObject* ptree_front(ptree_object *self);
static PyObject* ptree_back(ptree_object *self);
static PyObject* ptree_push_front(ptree_object *self, PyObject *args, PyObject *kwds);
static PyObject* ptree_push_back(ptree_object *self, PyObject *args, PyObject *kwds);
static PyObject* ptree_pop_front(ptree_object *self);
static PyObject* ptree_pop_back(ptree_object *self);
static PyObject* ptree_reverse(ptree_object *self);
static PyObject* ptree_richcompare(ptree_object *a, PyObject *b, int op);
static PyObject* ptree_count(ptree_object *self, PyObject *args, PyObject *kwds);
static PyObject* ptree_erase(ptree_object *self, PyObject *args, PyObject *kwds);
static PyObject* ptree_data(ptree_object *self);
static PyObject* ptree_clear(ptree_object *self);
static PyObject* ptree_get_child(ptree_object *self, PyObject *args, PyObject *kwds);
static PyObject* ptree_get_child_optional(ptree_object *self, PyObject *args, PyObject *kwds);
static PyObject* ptree_put_child(ptree_object *self, PyObject *args, PyObject *kwds);
static PyObject* ptree_add_child(ptree_object *self, PyObject *args, PyObject *kwds);
static PyObject* ptree_get_value(ptree_object *self);
static PyObject* ptree_put_value(ptree_object *self, PyObject *args, PyObject *kwds);
static PyObject* ptree_get(ptree_object *self, PyObject *args, PyObject *kwds);
static PyObject* ptree_put(ptree_object *self, PyObject *args, PyObject *kwds);
static PyObject* ptree_add(ptree_object *self, PyObject *args, PyObject *kwds);
static PyObject* ptree_tree_sharers_count(ptree_object *self);
static PyObject* ptree_is_root(ptree_object *self);
static PyObject* ptree_shares_tree_with(ptree_object *self, PyObject *args, PyObject *kwds);
static PyObject* ptree___iter__(ptree_object *self);
static PyObject* ptree___reversed__(ptree_object *self);
static PyObject* ptree_iterordered(ptree_object *self);

//-------------------------------------------------------------------------

static PyMethodDef ptree_methods[] = {
    {"size", (PyCFunction)ptree_size, METH_NOARGS,
     "size() -> long\n\
\n\
The number of direct children of this node."},
    {"max_size", (PyCFunction)ptree_max_size, METH_NOARGS,
     "max_size() -> long\n\
\n\
The max size."},
    {"empty", (PyCFunction)ptree_empty, METH_NOARGS,
     "empty() -> bool\n\
\n\
Whether there are any direct children."},
    {"front", (PyCFunction)ptree_front, METH_NOARGS,
     "front() -> (str, ptree)\n\
\n\
A reference of the front value. A tuple with key and ptree. The returned ptree will share the real tree with this ptree."},
    {"back", (PyCFunction)ptree_back, METH_NOARGS,
     "back() -> (str, ptree)\n\
\n\
A reference of the back value. A tuple with key and ptree. The returned ptree will share the real tree with this ptree."},
    {"push_front", (PyCFunction)ptree_push_front, METH_VARARGS | METH_KEYWORDS,
     "push_front(value) -> None\n\
\n\
Insert a copy of the given tree with its key just before the begin."},
    {"push_back", (PyCFunction)ptree_push_back, METH_VARARGS | METH_KEYWORDS,
     "push_back(value) -> None\n\
\n\
Insert a copy of the given tree with its key just before the end."},
    {"pop_front", (PyCFunction)ptree_pop_front, METH_NOARGS,
     "pop_front() -> None\n\
\n\
Erases the front child."},
    {"pop_back", (PyCFunction)ptree_pop_back, METH_NOARGS,
     "pop_back() -> None\n\
\n\
Erases the back child."},
    {"reverse", (PyCFunction)ptree_reverse, METH_NOARGS,
     "reverse() -> None\n\
\n\
Reverses the order of direct children in the property tree."},
    {"count", (PyCFunction)ptree_count, METH_VARARGS | METH_KEYWORDS,
     "count(key) -> long\n\
\n\
Count the number of direct children with the given key."},
    {"erase", (PyCFunction)ptree_erase, METH_VARARGS | METH_KEYWORDS,
     "erase(key) -> long\n\
\n\
Erase all direct children with the given key and return the count."},
    {"data", (PyCFunction)ptree_data, METH_NOARGS,
     "data() -> str\n\
\n\
Returns the actual data in this node."},
    {"clear", (PyCFunction)ptree_clear, METH_NOARGS,
     "clear() -> None\n\
\n\
Clear this tree completely, of both data and children."},
    {"get_child", (PyCFunction)ptree_get_child, METH_VARARGS | METH_KEYWORDS,
     "get_child(path, defaut_value=None) -> ptree\n\
\n\
If default_value is None get the child at the given path, or throw ptree_bad_path. Else get the child at the given path, or return default_value. The returned ptree will share the real tree with this ptree.\n\
\n\
Note:\n\
Depending on the path, the result at each level may not be completely determinate, i.e. if the same key appears multiple times, which child is chosen is not specified. This can lead to the path not being resolved even though there is a descendant with this path. Example:\n\
   a -> b -> c\n\
     -> b\n\
The path \"a.b.c\" will succeed if the resolution of \"b\" chooses the first such node, but fail if it chooses the second."},
    {"get_child_optional", (PyCFunction)ptree_get_child_optional, METH_VARARGS | METH_KEYWORDS,
     "get_child_optional(path) -> ptree\n\
\n\
Get the child at the given path, or return None. The returned ptree will share the real tree with this ptree."},
    {"put_child", (PyCFunction)ptree_put_child, METH_VARARGS | METH_KEYWORDS,
     "put_child(path, value) -> ptree\n\
\n\
Set the node at the given path to the given value. Create any missing parents. If the node at the path already exists, replace it.\n\
\n\
Returns: A reference to the inserted subtree. The returned ptree will share the real tree with this ptree.\n\
\n\
Note:\n\
Because of the way paths work, it is not generally guaranteed that a node newly created can be accessed using the same path. If the path could refer to multiple nodes, it is unspecified which one gets replaced."},
    {"add_child", (PyCFunction)ptree_add_child, METH_VARARGS | METH_KEYWORDS,
     "add_child(path, value) -> ptree\n\
\n\
Add the node at the given path. Create any missing parents. If there already is a node at the path, add another one with the same key.\n\
\n\
Returns: A reference to the inserted subtree. The returned ptree will share the real tree with this ptree.\n\
\n\
Note:\n\
Because of the way paths work, it is not generally guaranteed that a node newly created can be accessed using the same path."},
    {"get_value", (PyCFunction)ptree_get_value, METH_NOARGS,
     "get_value() -> str\n\
\n\
Take the value of this node."},
    {"put_value", (PyCFunction)ptree_put_value, METH_VARARGS | METH_KEYWORDS,
     "put_value(value) -> None\n\
\n\
Replace the value at this node with the given value."},
    {"get", (PyCFunction)ptree_get, METH_VARARGS | METH_KEYWORDS,
     "get(path) -> str\n\
\n\
Shorthand for get_child(path).get_value()."},
    {"put", (PyCFunction)ptree_put, METH_VARARGS | METH_KEYWORDS,
     "put(path, value) -> ptree\n\
\n\
Set the value of the node at the given path to the supplied value. If the node doesn't exist, it is created, including all its missing parents.\n\
\n\
Returns: A reference of the node that had its value changed. The returned ptree will share the real tree with this ptree."},
    {"add", (PyCFunction)ptree_add, METH_VARARGS | METH_KEYWORDS,
     "add(path, value) -> ptree\n\
\n\
If the node identified by the path does not exist, create it, including all its missing parents. If the node already exists, add a sibling with the same key. Set the newly created node's value to the given paremeter.\n\
\n\
Returns: The node that was added. The returned ptree will share the real tree with this ptree."},
    {"tree_sharers_count", (PyCFunction)ptree_tree_sharers_count, METH_NOARGS,
     "tree_sharers_count() -> long\n\
\n\
Returns the numbers of ptrees which shares the same real tree."},
    {"is_root", (PyCFunction)ptree_is_root, METH_NOARGS,
     "is_root() -> bool\n\
\n\
Returns true if the tree root is the root of the real tree."},
    {"shares_tree_with", (PyCFunction)ptree_shares_tree_with, METH_VARARGS | METH_KEYWORDS,
     "shares_tree_with(ptree) -> bool\n\
\n\
Returns true if this ptree shares the real tree with \"ptree\"."},
    {"__reversed__", (PyCFunction)ptree___reversed__, METH_NOARGS,
     "__reversed__() -> iter\n\
\n\
Returns a reversed iterator."},
    {"iterordered", (PyCFunction)ptree_iterordered, METH_NOARGS,
     "iterordered() -> iter\n\
\n\
Returns an iterator ordered in key order."},
    {NULL}
};

const char* const ptree_type_doc =
"Wrapper to a boost::property_tree::ptree class.\n\
\n\
";

static PyTypeObject ptree_type = {
    PyObject_HEAD_INIT(NULL)
    0,                                 /* ob_size */
    "property_tree.ptree",             /* tp_name */
    sizeof(ptree_object),              /* tp_basicsize */
    0,                                 /* tp_itemsize */
    (destructor)ptree_dealloc,         /* tp_dealloc */
    0,                                 /* tp_print */
    0,                                 /* tp_getattr */
    0,                                 /* tp_setattr */
    0,                                 /* tp_compare */
    0,                                 /* tp_repr */
    0,                                 /* tp_as_number */
    0,                                 /* tp_as_sequence */
    0,                                 /* tp_as_mapping */
    0,                                 /* tp_hash  */
    0,                                 /* tp_call */
    (reprfunc)ptree_str,               /* tp_str */
    PyObject_GenericGetAttr,           /* tp_getattro */
    PyObject_GenericSetAttr,           /* tp_setattro */
    0,                                 /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,                /* tp_flags */
    ptree_type_doc,                    /* tp_doc */
    0,		                       /* tp_traverse */
    0,		                       /* tp_clear */
    (richcmpfunc)ptree_richcompare,    /* tp_richcompare */
    0,		                       /* tp_weaklistoffset */
    (getiterfunc)ptree___iter__,       /* tp_iter */
    0,		                       /* tp_iternext */
    ptree_methods,                     /* tp_methods */
    0,                                 /* tp_members */
    0,                                 /* tp_getset */
    0,                                 /* tp_base */
    0,                                 /* tp_dict */
    0,                                 /* tp_descr_get */
    0,                                 /* tp_descr_set */
    offsetof(ptree_object, attr_dict), /* tp_dictoffset */
    (initproc)ptree_init,              /* tp_init */
    0,                                 /* tp_alloc */
    ptree_new,                         /* tp_new */
};

//------------------------------------------------ ptree class

static PyObject* ptree_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    ptree_object *self;

    self = (ptree_object*)type->tp_alloc(type, 0);
    if (self != NULL) {
        self->real_ptree = new boost::shared_ptr<boost::property_tree::ptree>(new boost::property_tree::ptree());
        self->ptree = self->real_ptree->get();
        self->attr_dict = PyDict_New();
    }

    return (PyObject*)self;
}

static int ptree_init(ptree_object *self, PyObject *args, PyObject *kwds)
{
    const char *data = NULL;

    static char *kwlist[] = {(char*)"data", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &data)) {
        return -1;
    }

    if (data) {
        *(self->ptree) = boost::property_tree::ptree(data);
    }

    return 0;
}

static void ptree_dealloc(ptree_object *self)
{
    delete self->real_ptree;
    self->ob_type->tp_free((PyObject*)self);
}

static PyObject* ptree_str(ptree_object *self)
{
    std::ostringstream oss;
    oss << *self->ptree;

    const std::string str = oss.str();
    return PyString_FromString(str.c_str());
}

static PyObject* ptree_size(ptree_object *self)
{
    const boost::property_tree::ptree::size_type size_b = self->ptree->size();
    size_t size_c = 0;
    try
    {
        size_c = boost::numeric_cast<size_t>(size_b);
    }
    catch(const boost::bad_numeric_cast& e) {
        PyErr_SetString(PyExc_OverflowError, e.what());
        return NULL;
    }

    return PyLong_FromSize_t(size_c);
}

static PyObject* ptree_max_size(ptree_object *self)
{
    const boost::property_tree::ptree::size_type size_b = self->ptree->max_size();
    size_t size_c = 0;
    try
    {
        size_c = boost::numeric_cast<size_t>(size_b);
    }
    catch(const boost::bad_numeric_cast& e) {
        PyErr_SetString(PyExc_OverflowError, e.what());
        return NULL;
    }

    return PyLong_FromSize_t(size_c);
}

static PyObject* ptree_empty(ptree_object *self)
{
    if (self->ptree->empty()) {
        Py_RETURN_TRUE;
    } else {
        Py_RETURN_FALSE;
    }
}

static PyObject* ptree_front(ptree_object *self)
{
    boost::property_tree::ptree::value_type &value = self->ptree->front();
    const char * const key = value.first.c_str();
    PyObject *tree = makeRef(self->real_ptree, &value.second);

    return Py_BuildValue("(sN)", key, tree);
}

static PyObject* ptree_back(ptree_object *self)
{
    boost::property_tree::ptree::value_type &value = self->ptree->back();
    const char * const key = value.first.c_str();
    PyObject *tree = makeRef(self->real_ptree, &value.second);

    return Py_BuildValue("(sN)", key, tree);
}

static PyObject* ptree_push_front(ptree_object *self, PyObject *args, PyObject *kwds)
{
    const char *key = NULL;
    ptree_object *ptree = NULL;

    static char *kwlist[] = {(char*)"value", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "(sO!)", kwlist, &key, &ptree_type, &ptree)) {
        return NULL;
    }

    self->ptree->push_front(std::make_pair(std::string(key), *ptree->ptree));

    Py_RETURN_NONE;
}

static PyObject* ptree_push_back(ptree_object *self, PyObject *args, PyObject *kwds)
{
    const char *key = NULL;
    ptree_object *ptree = NULL;

    static char *kwlist[] = {(char*)"value", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "(sO!)", kwlist, &key, &ptree_type, &ptree)) {
        return NULL;
    }

    self->ptree->push_back(std::make_pair(std::string(key), *ptree->ptree));

    Py_RETURN_NONE;
}

static PyObject* ptree_pop_front(ptree_object *self)
{
    self->ptree->pop_front();

    Py_RETURN_NONE;
}

static PyObject* ptree_pop_back(ptree_object *self)
{
    self->ptree->pop_back();

    Py_RETURN_NONE;
}

static PyObject* ptree_reverse(ptree_object *self)
{
    self->ptree->reverse();

    Py_RETURN_NONE;
}

static PyObject* ptree_richcompare(ptree_object *a, PyObject *b, int op)
{
    if ((op != Py_EQ) && (op != Py_NE)) {
        PyErr_SetString(PyExc_TypeError, "can only compare to equal or not equal");
        return NULL;
    }

    if (!PyPtree_Check(b)) {
        if (op == Py_EQ) Py_RETURN_FALSE;
        if (op == Py_NE) Py_RETURN_TRUE;
    }

    if (op == Py_EQ) {
        if (*a->ptree == *((ptree_object*)b)->ptree) Py_RETURN_TRUE;
        else Py_RETURN_FALSE;
    } else { //op == Py_NE
        if (*a->ptree != *((ptree_object*)b)->ptree) Py_RETURN_TRUE;
        else Py_RETURN_FALSE;
    }
}

static PyObject* ptree_count(ptree_object *self, PyObject *args, PyObject *kwds)
{
    const char *key = NULL;

    static char *kwlist[] = {(char*)"key", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &key)) {
        return NULL;
    }

    const boost::property_tree::ptree::size_type count_b = self->ptree->count(key);
    size_t count_c = 0;
    try
    {
        count_c = boost::numeric_cast<size_t>(count_b);
    }
    catch(const boost::bad_numeric_cast& e) {
        PyErr_SetString(PyExc_OverflowError, e.what());
        return NULL;
    }

    return PyLong_FromSize_t(count_c);
}

static PyObject* ptree_erase(ptree_object *self, PyObject *args, PyObject *kwds)
{
    const char *key = NULL;

    static char *kwlist[] = {(char*)"key", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &key)) {
        return NULL;
    }

    const boost::property_tree::ptree::size_type erase_b = self->ptree->erase(key);
    size_t erase_c = 0;
    try
    {
        erase_c = boost::numeric_cast<size_t>(erase_b);
    }
    catch(const boost::bad_numeric_cast& e) {
        PyErr_SetString(PyExc_OverflowError, e.what());
        return NULL;
    }

    return PyLong_FromSize_t(erase_c);
}

static PyObject* ptree_data(ptree_object *self)
{
    std::string data = self->ptree->data();
    return PyString_FromString(data.c_str());
}

static PyObject* ptree_clear(ptree_object *self)
{
    self->ptree->clear();
    Py_RETURN_NONE;
}

static PyObject* ptree_get_child(ptree_object *self, PyObject *args, PyObject *kwds)
{
    const char *path = NULL;
    ptree_object *default_value = NULL;

    static char *kwlist[] = {(char*)"path", (char*)"default_value", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s|O!", kwlist, &path, &ptree_type, &default_value)) {
        return NULL;
    }

    boost::property_tree::ptree *pt;
    if (default_value == NULL) {
        try{
            pt = &self->ptree->get_child(std::string(path));
        }
        catch (const boost::property_tree::ptree_bad_path &e) {
            PyErr_SetString(ptree_bad_path, e.what());
            return NULL;
        }
    } else {
        pt = &self->ptree->get_child(std::string(path), *default_value->ptree);
    }

    return makeRef(self->real_ptree, pt);
}

static PyObject* ptree_get_child_optional(ptree_object *self, PyObject *args, PyObject *kwds)
{
    const char *path = NULL;

    static char *kwlist[] = {(char*)"path", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &path)) {
        return NULL;
    }

    boost::optional<boost::property_tree::ptree&> opt = self->ptree->get_child_optional(path);

    if (!opt) {
        Py_RETURN_NONE;
    } else {
        return makeRef(self->real_ptree, &(*opt));
    }
}

static PyObject* ptree_put_child(ptree_object *self, PyObject *args, PyObject *kwds)
{
    const char *path = NULL;
    ptree_object *value = NULL;

    static char *kwlist[] = {(char*)"path", (char*)"value", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "sO!", kwlist, &path, &ptree_type, &value)) {
        return NULL;
    }

    boost::property_tree::ptree &pt = self->ptree->put_child(path, *value->ptree);

    return makeRef(self->real_ptree, &pt);
}

static PyObject* ptree_add_child(ptree_object *self, PyObject *args, PyObject *kwds)
{
    const char *path = NULL;
    ptree_object *value = NULL;

    static char *kwlist[] = {(char*)"path", (char*)"value", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "sO!", kwlist, &path, &ptree_type, &value)) {
        return NULL;
    }

    boost::property_tree::ptree &pt = self->ptree->add_child(path, *value->ptree);

    return makeRef(self->real_ptree, &pt);
}

static PyObject* ptree_get_value(ptree_object *self)
{
    std::string data = self->ptree->get_value<std::string>();
    return PyString_FromString(data.c_str());
}

static PyObject* ptree_put_value(ptree_object *self, PyObject *args, PyObject *kwds)
{
    const char *value = NULL;

    static char *kwlist[] = {(char*)"value", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &value)) {
        return NULL;
    }

    self->ptree->put_value(value);

    Py_RETURN_NONE;
}

static PyObject* ptree_get(ptree_object *self, PyObject *args, PyObject *kwds)
{
    const char *path = NULL;

    static char *kwlist[] = {(char*)"path", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &path)) {
        return NULL;
    }

    std::string str = self->ptree->get<std::string>(path);

    return PyString_FromString(str.c_str());
}

static PyObject* ptree_put(ptree_object *self, PyObject *args, PyObject *kwds)
{
    const char *path = NULL;
    const char *value = NULL;

    static char *kwlist[] = {(char*)"path", (char*)"value", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "ss", kwlist, &path, &value)) {
        return NULL;
    }

    boost::property_tree::ptree &pt = self->ptree->put(path, value);

    return makeRef(self->real_ptree, &pt);
}

static PyObject* ptree_add(ptree_object *self, PyObject *args, PyObject *kwds)
{
    const char *path = NULL;
    const char *value = NULL;

    static char *kwlist[] = {(char*)"path", (char*)"value", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "ss", kwlist, &path, &value)) {
        return NULL;
    }

    boost::property_tree::ptree &pt = self->ptree->add(path, value);

    return makeRef(self->real_ptree, &pt);
}

static PyObject* ptree_tree_sharers_count(ptree_object *self)
{
    const long count = self->real_ptree->use_count();
    return PyLong_FromLong(count);
}

static PyObject* ptree_is_root(ptree_object *self)
{
    if (self->real_ptree->get() == self->ptree) {
        Py_RETURN_TRUE;
    } else {
        Py_RETURN_FALSE;
    }
}

static PyObject* ptree_shares_tree_with(ptree_object *self, PyObject *args, PyObject *kwds)
{
    ptree_object *ptree = NULL;

    static char *kwlist[] = {(char*)"ptree", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!", kwlist, &ptree_type, &ptree)) {
        return NULL;
    }

    if (self->real_ptree->get() == ptree->real_ptree->get()) {
        Py_RETURN_TRUE;
    } else {
        Py_RETURN_FALSE;
    }
}

static PyObject* ptree___iter__(ptree_object *self)
{
    return make_iterator(self);
}

static PyObject* ptree___reversed__(ptree_object *self)
{
    return make_reverse_iterator(self);
}

static PyObject* ptree_iterordered(ptree_object *self)
{
    return make_assoc_iterator(self);
}

//------------------------------------------------ ITERATOR

template<class IT>
static void T_iterator_dealloc(IT *self)
{
    Py_DECREF(self->ptree);
    delete self->current;
    delete self->end;
    self->ob_type->tp_free((PyObject*)self);
}

static PyObject* generic_iterator___iter__(PyObject *self)
{
    Py_INCREF(self);
    return self;
}

template<class IT>
static PyObject* T_iterator_next(IT *self)
{
    if (*self->current == *self->end) {
        PyErr_SetNone(PyExc_StopIteration);
        return NULL;
    }

    const char * const key = (*self->current)->first.c_str();
    PyObject *tree = makeRef(self->ptree->real_ptree, &(*self->current)->second);

    ++(*self->current);

    return Py_BuildValue("(sN)", key, tree);
}

template static void T_iterator_dealloc<iterator_object>(iterator_object*);
template static PyObject* T_iterator_next<iterator_object>(iterator_object*);

template static void T_iterator_dealloc<reverse_iterator_object>(reverse_iterator_object*);
template static PyObject* T_iterator_next<reverse_iterator_object>(reverse_iterator_object*);

template static void T_iterator_dealloc<assoc_iterator_object>(assoc_iterator_object*);
template static PyObject* T_iterator_next<assoc_iterator_object>(assoc_iterator_object*);

static PyTypeObject iterator_type = {
    PyObject_HEAD_INIT(NULL)
    0,                                      /* ob_size */
    "property_tree.iterator",               /* tp_name */
    sizeof(iterator_object),                /* tp_basicsize */
    0,                                      /* tp_itemsize */
    (destructor)T_iterator_dealloc<iterator_object>,   /* tp_dealloc */
    0,                                      /* tp_print */
    0,                                      /* tp_getattr */
    0,                                      /* tp_setattr */
    0,                                      /* tp_compare */
    0,                                      /* tp_repr */
    0,                                      /* tp_as_number */
    0,                                      /* tp_as_sequence */
    0,                                      /* tp_as_mapping */
    0,                                      /* tp_hash  */
    0,                                      /* tp_call */
    0,                                      /* tp_str */
    0,                                      /* tp_getattro */
    0,                                      /* tp_setattro */
    0,                                      /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,                     /* tp_flags */
    0,                                      /* tp_doc */
    0,		                            /* tp_traverse */
    0,		                            /* tp_clear */
    0,                                      /* tp_richcompare */
    0,		                            /* tp_weaklistoffset */
    (getiterfunc)generic_iterator___iter__, /* tp_iter */
    (iternextfunc)T_iterator_next<iterator_object>,    /* tp_iternext */
};

static PyTypeObject reverse_iterator_type = {
    PyObject_HEAD_INIT(NULL)
    0,                                      /* ob_size */
    "property_tree.reverse_iterator",       /* tp_name */
    sizeof(reverse_iterator_object),        /* tp_basicsize */
    0,                                      /* tp_itemsize */
    (destructor)T_iterator_dealloc<reverse_iterator_object>,   /* tp_dealloc */
    0,                                      /* tp_print */
    0,                                      /* tp_getattr */
    0,                                      /* tp_setattr */
    0,                                      /* tp_compare */
    0,                                      /* tp_repr */
    0,                                      /* tp_as_number */
    0,                                      /* tp_as_sequence */
    0,                                      /* tp_as_mapping */
    0,                                      /* tp_hash  */
    0,                                      /* tp_call */
    0,                                      /* tp_str */
    0,                                      /* tp_getattro */
    0,                                      /* tp_setattro */
    0,                                      /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,                     /* tp_flags */
    0,                                      /* tp_doc */
    0,		                            /* tp_traverse */
    0,		                            /* tp_clear */
    0,                                      /* tp_richcompare */
    0,		                            /* tp_weaklistoffset */
    (getiterfunc)generic_iterator___iter__, /* tp_iter */
    (iternextfunc)T_iterator_next<reverse_iterator_object>,    /* tp_iternext */
};

static PyTypeObject assoc_iterator_type = {
    PyObject_HEAD_INIT(NULL)
    0,                                      /* ob_size */
    "property_tree.assoc_iterator",         /* tp_name */
    sizeof(assoc_iterator_object),          /* tp_basicsize */
    0,                                      /* tp_itemsize */
    (destructor)T_iterator_dealloc<assoc_iterator_object>,   /* tp_dealloc */
    0,                                      /* tp_print */
    0,                                      /* tp_getattr */
    0,                                      /* tp_setattr */
    0,                                      /* tp_compare */
    0,                                      /* tp_repr */
    0,                                      /* tp_as_number */
    0,                                      /* tp_as_sequence */
    0,                                      /* tp_as_mapping */
    0,                                      /* tp_hash  */
    0,                                      /* tp_call */
    0,                                      /* tp_str */
    0,                                      /* tp_getattro */
    0,                                      /* tp_setattro */
    0,                                      /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,                     /* tp_flags */
    0,                                      /* tp_doc */
    0,		                            /* tp_traverse */
    0,		                            /* tp_clear */
    0,                                      /* tp_richcompare */
    0,		                            /* tp_weaklistoffset */
    (getiterfunc)generic_iterator___iter__, /* tp_iter */
    (iternextfunc)T_iterator_next<assoc_iterator_object>,    /* tp_iternext */
};

PyObject* make_iterator(ptree_object *ptree)
{
    Py_INCREF(ptree);

    iterator_object *self;
    self = (iterator_object*)iterator_type.tp_alloc(&iterator_type, 0);
    self->ptree = ptree;
    self->current = new boost::property_tree::ptree::iterator(ptree->ptree->begin());
    self->end = new boost::property_tree::ptree::iterator(ptree->ptree->end());

    return (PyObject*)self;
}

PyObject* make_reverse_iterator(ptree_object *ptree)
{
    Py_INCREF(ptree);

    reverse_iterator_object *self;
    self = (reverse_iterator_object*)reverse_iterator_type.tp_alloc(&reverse_iterator_type, 0);
    self->ptree = ptree;
    self->current = new boost::property_tree::ptree::reverse_iterator(ptree->ptree->rbegin());
    self->end = new boost::property_tree::ptree::reverse_iterator(ptree->ptree->rend());

    return (PyObject*)self;
}

PyObject* make_assoc_iterator(ptree_object *ptree)
{
    Py_INCREF(ptree);

    assoc_iterator_object *self;
    self = (assoc_iterator_object*)assoc_iterator_type.tp_alloc(&assoc_iterator_type, 0);
    self->ptree = ptree;
    self->current = new boost::property_tree::ptree::assoc_iterator(ptree->ptree->ordered_begin());
    self->end = new boost::property_tree::ptree::assoc_iterator(ptree->ptree->not_found());

    return (PyObject*)self;
}

//------------------------------------------------ Functions

PyObject* graphUnion(PyObject *self, PyObject *args)
{
    ptree_object *pt1;
    ptree_object *pt2;

    if (!PyArg_ParseTuple(args, "O!O!", &ptree_type, &pt1, &ptree_type, &pt2)) {
        return NULL;
    }

    boost::property_tree::ptree result = golld::property_tree::graphUnion(*pt1->ptree, *pt2->ptree);

    return PyPtree_FromPtree(result);
}

static PyMethodDef property_tree_functions[] = {
    {"graphUnion", graphUnion, METH_VARARGS,
     "graphUnion(ptree1, ptree2) -> ptree\n\
\n\
Return the union of ptree1 and ptree2."},
   {NULL}
};

//-----------------------------------------------

static int PyPtree_Check(PyObject *o)
{
    return PyObject_TypeCheck(o, &ptree_type);
}

static PyObject* PyPtree_FromPtree(const boost::property_tree::ptree &ptree)
{
    ptree_object *self = (ptree_object*)PyObject_CallObject((PyObject*)&ptree_type, NULL);

    if (self != NULL) {
        *self->ptree = ptree;
    }

    return (PyObject*)self;
}

PyMODINIT_FUNC init_ptree(void)
{
    PyObject *m;
    static void *PyPtree_API[PyPtree_API_pointers];
    PyObject *c_api_object;

    if (PyType_Ready(&ptree_type) < 0)
        return;
    if (PyType_Ready(&iterator_type) < 0)
        return;
    if (PyType_Ready(&reverse_iterator_type) < 0)
        return;
    if (PyType_Ready(&assoc_iterator_type) < 0)
        return;

    m = Py_InitModule3("_ptree", property_tree_functions,
                       "Python wrapper to Boost Property Tree library.");
    if (m == NULL)
        return;

    PyPtree_API[PyPtree_Check_NUM] = (void*)PyPtree_Check;
    PyPtree_API[PyPtree_FromPtree_NUM] = (void*)PyPtree_FromPtree;

    c_api_object = PyCapsule_New((void*)PyPtree_API, "golld.property_tree._ptree._C_API", NULL);
    if (c_api_object != NULL) {
        PyModule_AddObject(m, "_C_API", c_api_object);
    }

    ptree_error = PyErr_NewException((char*)"property_tree.ptree_error", PyExc_Exception, NULL);
    ptree_bad_path = PyErr_NewException((char*)"property_tree.ptree_bad_path", ptree_error, NULL);
    file_parser_error = PyErr_NewException((char*)"property_tree.file_parser_error", ptree_error, NULL);

    Py_INCREF(ptree_error);
    Py_INCREF(ptree_bad_path);
    Py_INCREF(file_parser_error);
    PyModule_AddObject(m, "ptree_error", ptree_error);
    PyModule_AddObject(m, "ptree_bad_path", ptree_bad_path);
    PyModule_AddObject(m, "file_parser_error", file_parser_error);

    Py_INCREF(&ptree_type);
    PyModule_AddObject(m, "ptree", (PyObject*)&ptree_type);
}


static PyObject* makeRef(boost::shared_ptr<boost::property_tree::ptree> *real_ptree,
                         boost::property_tree::ptree *ptree)
{
    ptree_object *self = (ptree_object*)PyObject_CallObject((PyObject*)&ptree_type, NULL);
    *self->real_ptree = *real_ptree;
    self->ptree = ptree;

    return (PyObject*)self;
}
