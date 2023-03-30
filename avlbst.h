#ifndef RBBST_H
#define RBBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"
#include <stack>

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
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
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
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
    Node<Key, Value>(key, value, parent), balance_(0)
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
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
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
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* node);
    void removeFix(AVLNode<Key,Value>* n, int8_t diff);
    void rotateRight(AVLNode<Key,Value>* n);
    void rotateLeft(AVLNode<Key, Value>* n);
    int getHeight(const AVLNode<Key, Value>* node) const;
};


/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */

template<class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value>& new_item) {
    if (this->empty()) {
        AVLNode<Key, Value>* initialNode = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
        this->root_ = initialNode;
        initialNode->setBalance(0);
    } else if (static_cast<AVLNode<Key, Value>*>(this->internalFind(new_item.first)) != nullptr) {
        AVLNode<Key, Value>* changedNode = static_cast<AVLNode<Key, Value>*>(this->internalFind(new_item.first));
        changedNode->setValue(new_item.second);
    } else {
        AVLNode<Key, Value>* temp = static_cast<AVLNode<Key, Value>*>(this->root_);
        AVLNode<Key, Value>* tempParent = nullptr;
        while (temp != nullptr) {
            tempParent = temp;
            if (temp->getKey() < new_item.first) {
                temp = temp->getRight();
            } else {
                temp = temp->getLeft();
            }
        }
        AVLNode<Key, Value>* addedNode = new AVLNode<Key, Value>(new_item.first, new_item.second, tempParent);
        addedNode->setBalance(0);
        if (tempParent->getBalance() == -1 || tempParent->getBalance() == 1) {
            if (tempParent->getBalance() == -1) {
                tempParent->setRight(addedNode);
            } else {
                tempParent->setLeft(addedNode);
            }
            tempParent->setBalance(0);
        } else {
            if (tempParent->getKey() < new_item.first) {
                tempParent->setRight(addedNode);
                tempParent->setBalance(tempParent->getBalance()+1);
                addedNode->setParent(tempParent);
            } else {
                tempParent->setLeft(addedNode);
                tempParent->setBalance(tempParent->getBalance()-1);
                addedNode->setParent(tempParent);
            }
            insertFix(tempParent, addedNode);
        }
    }
}




// done
template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* node) {
    // Exit if we've reached the root
    if (!parent || !parent->getParent()) {
        return;
    }

    // Make grandparent the parent of parent
    AVLNode<Key, Value>* grandparent = parent->getParent();

    // Handle the case where parent is the left child of grandparent
    if (grandparent->getLeft() == parent) {
        grandparent->updateBalance(-1);
        if (grandparent->getBalance() == 0) {  // Exit if balance has become 0
            return;
        } else if (grandparent->getBalance() == -1) {  // Recurse
            insertFix(grandparent, parent);
        } else if (grandparent->getBalance() == -2) {  // Right subtree is taller
            AVLNode<Key, Value>* leftChild = parent->getLeft();
            if (leftChild == node) {  // Zig-zig case
                rotateRight(grandparent);
                parent->setBalance(0);
                grandparent->setBalance(0);
            } else {  // Zig-zag case
                rotateLeft(parent);
                rotateRight(grandparent);
                if (node->getBalance() == -1) {
                    parent->setBalance(0);
                    grandparent->setBalance(1);
                    node->setBalance(0);
                } else if (node->getBalance() == 0) {
                    parent->setBalance(0);
                    grandparent->setBalance(0);
                    node->setBalance(0);
                } else if (node->getBalance() == 1) {
                    parent->setBalance(-1);
                    grandparent->setBalance(0);
                    node->setBalance(0);
                }
            }
        }
    }
    // Handle the case where parent is the right child of grandparent
    else {
        grandparent->updateBalance(1);
        if (grandparent->getBalance() == 0) {  // Exit if balance has become 0
            return;
        } else if (grandparent->getBalance() == 1) {
            insertFix(grandparent, parent);
        } else if (grandparent->getBalance() == 2) {  // Left subtree is taller
            AVLNode<Key, Value>* rightChild = parent->getRight();
            if (rightChild == node) {  // Zig-zig case
                rotateLeft(grandparent);
                parent->setBalance(0);
                grandparent->setBalance(0);
            } else {  // Zig-zag case
                rotateRight(parent);
                rotateLeft(grandparent);
                if (node->getBalance() == 1) {
                    parent->setBalance(0);
                    grandparent->setBalance(-1);
                    node->setBalance(0);
                } else if (node->getBalance() == 0) {
                    parent->setBalance(0);
                    grandparent->setBalance(0);
                    node->setBalance(0);
                } else if (node->getBalance() == -1) {
                    parent->setBalance(1);
                    grandparent->setBalance(0);
                    node->setBalance(0);
                }
            }
        }
    }
}



// done
// /*
//  * Recall: The writeup specifies that if a node has 2 children you
//  * should swap with the predecessor and then remove.
//  */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
    // Check if tree is empty
    if (this->empty())
        return;

    // Find the node from the given key
    AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if (curr == NULL)
        return;

    // Swap the node with its predecessor if it has two children
    if (curr->getLeft() != nullptr && curr->getRight() != nullptr)
    {
        AVLNode<Key, Value>* temp = static_cast<AVLNode<Key, Value>*>(this->predecessor(curr));
        nodeSwap(curr, temp);
    }

    // Get the parent and find the difference between heights of left and right subtrees
    AVLNode<Key, Value>* parent = curr->getParent();
    int diff = 0;
    if (parent != nullptr)
    {
        if (curr == parent->getLeft()) {
            diff = 1;
        } else {
            diff = -1;
        }

    }

    // Update the parent with the child of the node to be deleted and fix the tree balance
    AVLNode<Key, Value>* child = (curr->getLeft() != nullptr) ? curr->getLeft() : curr->getRight();
    if (child != nullptr)
        child->setParent(parent);
    if (parent == nullptr)
        this->root_ = child;
    else if (parent->getLeft() == curr)
        parent->setLeft(child);
    else
        parent->setRight(child);
    delete curr;
    removeFix(parent, diff);
}


// done
template <class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* node, int8_t diff) {
    if (!node) {
        return;
    }
    AVLNode<Key, Value>* parent = node->getParent();
    int8_t ndiff;
    if (parent && node == parent->getRight()) {
        ndiff = -1;
    } else {
        ndiff = 1;
    }

    int8_t balance = node->getBalance();
    if (balance == -1 && diff == -1) {
        AVLNode<Key, Value>* leftNode = node->getLeft();
        int8_t leftNodeBalance = leftNode->getBalance();
        if (leftNodeBalance <= 0) {
            rotateRight(node);
            node->setBalance(0);
            leftNode->setBalance(0);
        } else {
            AVLNode<Key, Value>* grandParent = leftNode->getRight();
            int8_t grandParentBalance = grandParent->getBalance();
            rotateLeft(leftNode);
            rotateRight(node);
            if (grandParentBalance < 0) {
                node->setBalance(1);
                leftNode->setBalance(0);
                grandParent->setBalance(0);
            } else if (grandParentBalance > 0) {
                node->setBalance(0);
                leftNode->setBalance(-1);
                grandParent->setBalance(0);
            } else {
                node->setBalance(0);
                leftNode->setBalance(0);
                grandParent->setBalance(0);
            }
        }
        removeFix(parent, ndiff);
    } else if (balance == 1 && diff == 1) {
        AVLNode<Key, Value>* rightNode = node->getRight();
        int8_t rightNodeBalance = rightNode->getBalance();
        if (rightNodeBalance >= 0) {
            rotateLeft(node);
            node->setBalance(0);
            rightNode->setBalance(0);
        } else {
            AVLNode<Key, Value>* grandParent = rightNode->getLeft();
            int8_t grandParentBalance = grandParent->getBalance();
            rotateRight(rightNode);
            rotateLeft(node);
            if (grandParentBalance > 0) {
                node->setBalance(-1);
                rightNode->setBalance(0);
                grandParent->setBalance(0);
            } else if (grandParentBalance < 0) {
                node->setBalance(0);
                rightNode->setBalance(1);
                grandParent->setBalance(0);
            } else {
                node->setBalance(0);
                rightNode->setBalance(0);
                grandParent->setBalance(0);
            }
        }
        removeFix(parent, ndiff);
     } else if (balance == 0) {
        node->setBalance(diff);
     } else if ((balance == 1 && diff == -1) || (balance == -1 && diff == 1)) {
		node->setBalance(0);
		removeFix(parent, ndiff);
     }
}

/**cases to handle for rotate:
 * when n->getLeft() has left and right node, need to move it's right to become y's top's left
 * update root if rotating one that is root and/or parent is nullptr
 * 
*/
template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key,Value>* n)
{
    if (n == nullptr)
    {
        return;
    }

    AVLNode<Key,Value>* nChild = n->getLeft();//if is nullptr, means n has no children to rotate up
    if (nChild == nullptr) //means nothing to move up
    {
        return;
    }

    AVLNode<Key,Value>* nChildRight = nChild->getRight(); //will be nullptr if no right child of left child, just means will assign nullptr as left child later
    AVLNode<Key,Value>* nParent = n->getParent();//if is nullptr, n is root and so child will end up being root anad need to switch

    //moves over below node's right child to be n's left child, will be nullptr if nChildRight does not exist
    //also make it's parent it's former child
    n->setLeft(nChildRight);
    n->setParent(nChild);

    if (nChildRight != nullptr)
    {
        nChildRight->setParent(n);
    }

    //sort out nChild
    //for child that is rotated up where n was, set it's  parent to n's parent
    //set it's right to n
    nChild->setParent(nParent);
    nChild->setRight(n);

    //sort out nParent
    //then connect the parent to the child correctly but checking to see what side it is
    if (nParent == nullptr) //means is root as nothing above
    {
        this->root_ = nChild;

    }
    else if (nParent->getRight() == n)
    {
        nParent->setRight(nChild);
    }
    else if (nParent->getLeft() == n)
    {
        nParent->setLeft(nChild);
    }
}


/**cases to handle for rotate:
 * when n->getLeft() has left and right node, need to move it's right to become y's top's left
 * update root if rotating one that is root and/or parent is nullptr
 * 
*/
template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key,Value>* n)
{
    if (n == nullptr)
    {
        return;
    }

    AVLNode<Key,Value>* nChild = n->getRight();//if is nullptr, means n has no children to rotate up
    if (nChild == nullptr) //means nothing to move up
    {
        return;
    }

    AVLNode<Key,Value>* nChildLeft = nChild->getLeft(); //will be nullptr if no right child of left child, just means will assign nullptr as left child later
    AVLNode<Key,Value>* nParent = n->getParent();//if is nullptr, n is root and so child will end up being root anad need to switch

    //moves over below node's right child to be n's left child, will be nullptr if nChildRight does not exist
    //also make it's parent it's former child
    n->setRight(nChildLeft);
    n->setParent(nChild);

    if (nChildLeft != nullptr)
    {
        nChildLeft->setParent(n);
    }

    //sort out nChild
    //for child that is rotated up where n was, set it's  parent to n's parent
    //set it's right to n
    nChild->setParent(nParent);
    nChild->setLeft(n);

    //sort out nParent
    //then connect the parent to the child correctly but checking to see what side it is
    if (nParent == nullptr) //means is root as nothing above
    {
        this->root_ = nChild;

    }
    else if (nParent->getRight() == n)
    {
        nParent->setRight(nChild);
    }
    else if (nParent->getLeft() == n)
    {
        nParent->setLeft(nChild);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

#endif
