#ifndef RBBST_H
#define RBBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the height as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int getHeight () const;
    void setHeight (int height);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int height_;
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor and setting
* the color to red since every new node will be red when it is first inserted.
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), height_(1)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the height of a AVLNode.
*/
template<class Key, class Value>
int AVLNode<Key, Value>::getHeight() const
{
    return height_;
}

/**
* A setter for the height of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setHeight(int height)
{
    height_ = height;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); 
    virtual void remove(const Key& key); 
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2); // swaps given nodes
    virtual void rotateLeft(AVLNode<Key, Value>* n); // rotates subtree of root n to the left
    virtual void rotateRight(AVLNode<Key, Value>* n); // rotates subtree of root n to the right
    virtual void balance(AVLNode<Key, Value>* n); // fixes tree balance using rotations
    virtual void rotateUpdate(AVLNode<Key, Value>* a, AVLNode<Key, Value>* b); // update height after rotations
    virtual void insertUpdate(AVLNode<Key, Value>* n); // updates heights and balances after insert
    virtual void removeUpdate(AVLNode<Key, Value>* n); //updates heights and balances after remove
};

template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    AVLNode<Key, Value>* prev;
    if (BinarySearchTree<Key, Value>::root_ == NULL) {
        BinarySearchTree<Key, Value>::root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, NULL);
        return;
    } else {
        AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(this->root_);
        // go down through the tree until you find the node with the same key or find a null node
        while (curr != NULL) {
            prev = curr;
            if (new_item.first == curr->getKey()) {
                break;
            } else if (new_item.first < curr->getKey()) {
                curr = curr->getLeft();
            } else {
                curr = curr->getRight();
            }
        }
        // if you find an identical node, update the value
        if (prev == curr) {
            curr->setValue(new_item.second);
            // in this case we dont need to balance, so break
            return;
        } else if (new_item.first < prev->getKey()) {
            // if hit null, set the corrrect predecessor
            prev->setLeft(new AVLNode<Key, Value>(new_item.first, new_item.second, prev));
        } else {
            prev->setRight(new AVLNode<Key, Value>(new_item.first, new_item.second, prev));
        }
    }
    if (prev->getHeight() == 2) {
        return;
    }
    insertUpdate(prev);
}

template<class Key, class Value>
void AVLTree<Key, Value>::remove (const Key& key)
{
    AVLNode<Key, Value>* temp = static_cast<AVLNode<Key, Value>*>(BinarySearchTree<Key, Value>::internalFind(key));
    if (temp == nullptr) // if node is not in tree
    {
        return;
    }
    else if (this->root_ == nullptr) //empty tree
    {
        return;
    }
    else if (temp->getLeft() == nullptr && temp->getRight() == nullptr) // no child nodes
    {
        if (temp == this->root_)
        {
            delete temp;
            this->root_ = nullptr;
        }
        else
        {
            if (temp->getParent()->getKey() > key)
            {
                temp->getParent()->setLeft(nullptr);
            }
            else
            {
                temp->getParent()->setRight(nullptr);
            }
            temp->setParent(nullptr);
            temp->setLeft(nullptr);
			temp->setRight(nullptr);
            AVLNode<Key, Value>* parent = temp->getParent();
            delete temp;
            removeUpdate(parent);
        }
    }
    else if (temp->getLeft() != NULL && temp->getRight() == NULL) // left child node
    {
        nodeSwap(temp, temp->getLeft());
        remove(temp->getKey());
    }
    else if (temp->getLeft() == NULL && temp->getRight() != NULL) // right child node
    {
        nodeSwap(temp, temp->getRight());
        remove(temp->getKey());
    }
    else // two child nodes 
    {
        nodeSwap(static_cast<AVLNode<Key, Value>*>(BinarySearchTree<Key, Value>::predecessor(temp)), temp);
        remove(temp->getKey());
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int tempH = n1->getHeight();
    n1->setHeight(n2->getHeight());
    n2->setHeight(tempH);
}

// updates heights and balances after insert
template<class Key, class Value>
void AVLTree<Key,Value>::insertUpdate(AVLNode<Key, Value>* n)
{
    if (n == NULL) // empty tree
    {
        return;
    }
    int leftHeight = 0;
    int rightHeight = 0;
    if (n->getLeft() != NULL)
    {
        leftHeight = n->getLeft()->getHeight();
    }
    if (n->getRight() != NULL)
    {
        rightHeight = n->getRight()->getHeight();
    }
    int height = std::max(leftHeight, rightHeight) + 1; // calculate height of ecisting node
    if (n->getHeight() != height) // update if necessary
    {  
        n->setHeight(height);
    }
    else
    {
        return;
    }
    if (abs(leftHeight - rightHeight) <= 1) // continues to update height along ancestor chain if necessary
    {
        if (n->getParent() != NULL)
        {
            insertUpdate(n->getParent());
        }     
        return;
    }
    balance(n); // balances if necessary
}

// updates heights and balances after remove
template<class Key, class Value>
void AVLTree<Key,Value>::removeUpdate(AVLNode<Key, Value>* n)
{
    if (n == NULL) // empty tree
    {
        return;
    }
    int leftHeight = 0;
    int rightHeight = 0;
    if (n->getLeft() != NULL)
    {
        leftHeight = n->getLeft()->getHeight();
    }
    if (n->getRight() != NULL)
    {
        rightHeight = n->getRight()->getHeight();
    }
    int height = std::max(leftHeight, rightHeight) + 1; // calculate height of ecisting node
    if (n->getHeight() != height) // update if necessary
    {  
        n->setHeight(height);
    }
    else
    {
        return;
    }
    if (abs(leftHeight - rightHeight) <= 1) // continues to update height along ancestor chain if necessary
    {
        if (n->getParent() != NULL)
        {
            removeUpdate(n->getParent());
        }     
        return;
    }
    balance(n); // balances if necessary
    removeUpdate(n->getParent()); //continues up ancestor chain
}

// rotates subtree with root n to the left
template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* a) {
    AVLNode<Key, Value>* b = a->getRight();
    AVLNode<Key, Value>* c = b->getLeft();
    // first fix a
    if (c) {
        a->setRight(c); // c becomes a's right child
        c->setParent(a);
    } else {
        a->setRight(NULL);
    }
    // check if a has a parent, if so we need to set it to be b's parent
    if (a->getParent()) {
        AVLNode<Key, Value>* p = a->getParent();
        if (p->getLeft() == a) {
            p->setLeft(b);
        } else {
            p->setRight(b);
        }
        b->setParent(p);
        // now that y is in the correct place below the parent we need to sort out a
        b->setLeft(a);
        a->setParent(b);

    } else {
        // a is the root set b to the root
        this->root_ = b;
        b->setParent(NULL);
        b->setLeft(a);
        a->setParent(b);
    }
    rotateUpdate(a, b);
}
// rotates subtree with root n to the right
template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* a) {
    AVLNode<Key, Value>* b = a->getLeft();
    AVLNode<Key, Value>* c = b->getRight();
    if (c != NULL) {
        a->setLeft(c); // c becomes a's left child
        c->setParent(a);
    } else {
        a->setLeft(NULL);
    }
    if (a->getParent()) // a's parent becomes b's parent, b becomes a's parent
    { 
        AVLNode<Key, Value>* p = a->getParent();
        if (p->getLeft() == a) 
        {
            p->setLeft(b);
        } else {
            p->setRight(b);
        }
        b->setParent(p);
        b->setRight(a);
        a->setParent(b);
    } else {
        // a is currently root
        this->root_ = b;
        b->setParent(NULL);
        b->setRight(a);
        a->setParent(b);
    }
    rotateUpdate(a, b);
}

// fixes tree balance using rotations
template<class Key, class Value>
void AVLTree<Key,Value>::balance(AVLNode<Key, Value>* node)
{
    if (node == NULL) // empty tree
    {
        return;
    }
    int leftHeight = 0;
    int rightHeight = 0;
    if (node->getLeft() != NULL) // finds which subtree is larger
    {
        leftHeight = node->getLeft()->getHeight();
    }
    if (node->getRight() != NULL)
    {
        rightHeight = node->getRight()->getHeight();
    }
    AVLNode<Key, Value>* parent; // will become node's parent after rotations
    if (leftHeight > rightHeight)
    {
        parent = node->getLeft();
        if (parent == NULL) 
        {
            return;
        }
        leftHeight = 0;
        rightHeight = 0;
        if (parent->getLeft() != NULL)
        {
            leftHeight = parent->getLeft()->getHeight();
        }
        if (parent->getRight() != NULL)
        {
            rightHeight = parent->getRight()->getHeight();
        }
        if (leftHeight > rightHeight) // left left
        {
            rotateRight(node);
        }
        else if (leftHeight < rightHeight) // left right
        {
            rotateLeft(parent);
            rotateRight(node);
        }
    }
    else if (leftHeight < rightHeight)
    {
        leftHeight = 0;
        rightHeight = 0;
        parent = node->getRight();
        if (parent == NULL)
        {
            return;
        }
        if (parent->getLeft() != NULL)
        {
            leftHeight = parent->getLeft()->getHeight();
        }
        if (parent->getRight() != NULL)
        {
            rightHeight = parent->getRight()->getHeight();
        }
        if (leftHeight > rightHeight) // right left
        {
            rotateRight(parent);
            rotateLeft(node);
        }
        else if (leftHeight < rightHeight) // right right
        {
            rotateLeft(node);
        }
    }
}

// update height after rotations
template<class Key, class Value>
void AVLTree<Key, Value>::rotateUpdate(AVLNode<Key, Value>* a, AVLNode<Key, Value>* b)
{
    int leftHeight = 0;
    int rightHeight = 0;
    if (a->getLeft() != NULL) 
    {
        leftHeight = a->getLeft()->getHeight();
    }
    if (a->getRight() != NULL)
    {
        rightHeight = a->getRight()->getHeight();
    }
    a->setHeight(std::max(leftHeight, rightHeight) + 1);
    leftHeight = 0;
    rightHeight = 0;
    if (b->getLeft() != NULL) 
    {
        leftHeight = b->getLeft()->getHeight();
    }
    if (b->getRight() != NULL)
    {
        rightHeight = b->getRight()->getHeight();
    }
    b->setHeight(std::max(leftHeight, rightHeight) + 1);
}
#endif