/* Copyright (C) 2011 Renato Florentino Garcia
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file BOOST_LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 * For more information, see http://www.boost.org
 */
#ifndef _GOLLD_PROPERTY_TREE_ASSIGN_HPP_
#define _GOLLD_PROPERTY_TREE_ASSIGN_HPP_

#include <boost/property_tree/ptree.hpp>
#include <string>

namespace golld {
namespace property_tree {
namespace assign {

/**
 * @brief An assign boost::property_tree::basic_ptree class.
 *
 * This class provides a more concise and elegant alternative to creates a
 * boost::property_tree::basic_ptree. For instance, a property_tree like:
 \verbatim
 key1 value1
 key2
 {
     key3 12345
     {
         key4 "value4 with spaces"
     }
     key5 value5
     "" 10
     "" 11
     "" 12
 }\endverbatim
 * Can be created:
 * \code
 * tree()
 * ("key1", "value1")
 * ("key2", tree()
 *     ("key3", 12345)
 *     (tree()
 *         ("key4", "value4 with spaces")
 *     )
 *     ("key5", "value5")
 *     (10)(11)(12)
 * );
 * \endcode
 */
template <class Ptree>
class basic_tree
{
public:
    /**
     * @brief Constructs an empty basic_ptree.
     *
     * The constructed basic_ptree has not a data or a subtree.
     */
    basic_tree()
        : ptree_()
    { }

    /**
     * @brief Construts a basic_ptree with internal data @c data.
     *
     * @param data The data to put in the constructed tree.
     */
    basic_tree(const typename Ptree::data_type &data)
        : ptree_(data)
    { }

    /**
     * @brief Adds to this basic_tree a subtree @c t under the path @c path.
     *
     * @param path The path of the added subtree.
     * @param t The subtree to be added.
     *
     * @return A reference to this basic_tree.
     */
    basic_tree & operator()(const typename Ptree::path_type &path, const basic_tree &t)
    {
        this->ptree_.add_child(path, t.ptree_);
        return *this;
    }

    /**
     * @brief Creates and adds to this basic_tree a subtree with data @c data under the
     * path @c path.
     *
     * @param path The path of the added subtree.
     * @param data The data of the just created tree.
     *
     * @return A reference to this basic_tree.
     */
    template <class Type>
    basic_tree & operator()(const typename Ptree::path_type &path, const Type &data)
    {
        this->ptree_.add(path, data);
        return *this;
    }

    /**
     * @brief Adds to this basic_tree a subtree @c t under an empty path.
     *
     * @param t The subtree to be added.
     *
     * @return A reference to this basic_tree.
     */
    basic_tree & operator()(const basic_tree &t)
    {
        this->ptree_.push_back(std::make_pair(typename Ptree::key_type(),
                                              static_cast<const Ptree&>(t)));
        return *this;
    }

    /**
     * @brief Creates and adds to this basic_tree a subtree with data @c data under an
     * empty path.
     *
     * @param data The data of the just created basic_tree.
     *
     * @return A reference to this basic_tree.
     */
    template <class Type>
    basic_tree & operator()(const Type &data)
    {
        Ptree tmp;
        tmp.put_value(data);
        this->ptree_.push_back(std::make_pair(typename Ptree::key_type(), tmp));
        return *this;
    }

    /**
     * @brief Converts this basic_tree to a Ptree.
     *
     * @return A Ptree equivalent to this tree.
     */
    operator const Ptree&() const
    {
        return this->ptree_;
    }

    /**
     * @brief Converts this basic_tree to a Ptree.
     *
     * @return A Ptree equivalent to this tree.
     */
    operator Ptree&()
    {
        return this->ptree_;
    }

private:
    Ptree ptree_;
};

typedef basic_tree<boost::property_tree::ptree> tree;

} // namespace assign
} // namespace property_tree
} // namespace golld

#endif /* _GOLLD_PROPERTY_TREE_ASSIGN_HPP_ */
