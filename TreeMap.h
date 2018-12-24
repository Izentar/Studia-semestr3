#ifndef AISDI_MAPS_TREEMAP_H
#define AISDI_MAPS_TREEMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <iostream>

namespace aisdi
{
template <typename KeyType, typename ValueType>
class TreeMap
{
    class Node;
private:
    std::size_t size;
    Node *guard;   // end,    indicates only the left child

public:
    using key_type = KeyType;
    using mapped_type = ValueType;
    using value_type = std::pair<const key_type, mapped_type>;
    using size_type = std::size_t;
    using reference = value_type&;
    using const_reference = const value_type&;

    class ConstIterator;
    class Iterator;
    using iterator = Iterator;
    using const_iterator = ConstIterator;

private:

    Node* findFirstLeaf() const // for methods begin()
    {
        Node *tmp=guard;
        while(tmp->goLeft()!=nullptr)
        {
            tmp=tmp->goLeft();
        }
        return tmp;
    }

    inline Node* findLastLeaf() const
    {
        return guard;
    }

    void deleteTreeMap(Node *node)
    {
        if(node==nullptr) return;

        deleteTreeMap(node->goLeft());
        deleteTreeMap(node->goRight());
        delete node;
        // do not set the pointer in guard at nullptr, because this method is used only in specific places
    }

    // rotations, only when tree is not balanced
    Node* rotationLeft(Node *subtree)
    {
        Node *father=subtree->goBack(), *subright=subtree->goRight(), *leaf;
        if(subright==nullptr) throw std::out_of_range("rotationLeft(Node *subtree) -> no right child. TreeMap.h");
        leaf=subright->goLeft();

        if(subtree->isRight())
        {
            father->changeRight(subright);
        }
        else
        {
            father->changeLeft(subright);
        }

        subtree->changeRight(leaf);
        subtree->changeBack(subright);
        subright->changeLeft(subtree);
        subright->changeBack(father);
        if(leaf!=nullptr) leaf->changeBack(subtree);

        return subright;

    }

    void rotationLeftAdding(Node *subtree)
    {
        Node *subright=rotationLeft(subtree);

        if(subright->getBalanceFactor()==-1)
        {
            subright->getBalanceFactor()=0;
            subtree->getBalanceFactor()=0;
        }
        else
        {
            subright->getBalanceFactor()=1;
            subtree->getBalanceFactor()=-1;
        }
    }

    void rotationLeftDeleting(Node *subtree)
    {
        Node *subright=rotationLeft(subtree);

        if(subright->getBalanceFactor()==0)
        {
            subright->getBalanceFactor()=1;
            subtree->getBalanceFactor()=-1;
        }
        else
        {
            subright->getBalanceFactor()=0;
            subtree->getBalanceFactor()=0;
        }

    }

    Node* rotationRight(Node *subtree)
    {
        Node *father=subtree->goBack(), *subleft=subtree->goLeft(), *leaf;
        if(subleft==nullptr) throw std::out_of_range("rotationRight(Node *subtree) -> no left child. TreeMap.h");
        leaf=subleft->goRight();

        if(subtree->isRight())
        {
            father->changeRight(subleft);
        }
        else
        {
            father->changeLeft(subleft);
        }
        subtree->changeBack(subleft);
        subtree->changeLeft(leaf);
        subleft->changeBack(father);
        subleft->changeRight(subtree);
        if(leaf!=nullptr) leaf->changeBack(subtree);

        return subleft;
    }

    void rotationRightAdding(Node *subtree)
    {
        Node *subleft=rotationRight(subtree);

        if(subleft->getBalanceFactor()==1)
        {
            subleft->getBalanceFactor()=0;
            subtree->getBalanceFactor()=0;
        }
        else
        {
            subleft->getBalanceFactor()=-1;
            subtree->getBalanceFactor()=1;
        }
    }

    void rotationRightDeleting(Node *subtree)
    {
        Node *subleft=rotationRight(subtree);

        if(subleft->getBalanceFactor()==0)
        {
            subleft->getBalanceFactor()=-1;
            subtree->getBalanceFactor()=0;
        }
        else
        {
            subleft->getBalanceFactor()=0;
            subtree->getBalanceFactor()=0;
        }


    }

    Node* rotationLeftRight(Node *subtree, Node *subchild)
    {
        Node *father=subtree->goBack(), *subchild2, *leaf, *leaf2;

        if(subchild==nullptr) throw std::out_of_range("rotationLeftRight(Node *subtree) -> no left child. TreeMap.h");
        subchild2=subchild->goRight();

        if(subchild2==nullptr) throw std::out_of_range("rotationLeftRight(Node *subtree) -> no left->right child. TreeMap.h");
        leaf=subchild2->goLeft();
        leaf2=subchild2->goRight();

        if(subtree->isRight())
        {
            father->changeRight(subchild2);
        }
        else
        {
            father->changeLeft(subchild2);
        }

        subchild2->changeBack(father);
        subchild2->changeLeft(subchild);
        subchild2->changeRight(subtree);
        subchild->changeBack(subchild2);
        subchild->changeRight(leaf);
        subtree->changeBack(subchild2);
        subtree->changeLeft(leaf2);
        if(leaf!=nullptr) leaf->changeBack(subchild);
        if(leaf2!=nullptr) leaf->changeBack(subtree);

        return subchild2;
    }

    void rotationLeftRightAdding(Node *subtree)
    {
        Node  *subchild=subtree->goLeft(), *subchild2=rotationLeftRight(subtree, subchild);

        if(subchild2->getBalanceFactor()==-1)
        {
            subtree->getBalanceFactor()=0;
            subchild->getBalanceFactor()=1;
        }
        else
        if(subchild2->getBalanceFactor()==0)
        {
            subtree->getBalanceFactor()=0;
            subchild->getBalanceFactor()=0;
        }
        else    // ==1
        {
            subtree->getBalanceFactor()=-1;
            subchild->getBalanceFactor()=0;
        }
        subchild2->getBalanceFactor()=0;
    }

    void rotationLeftRightDeleting(Node *subtree)
    {
        Node  *subchild=subtree->goLeft(), *subchild2=rotationLeftRight(subtree, subchild);

        if(subchild2->getBalanceFactor()==0)
        {
            subtree->getBalanceFactor()=0;
            subchild->getBalanceFactor()=0;
        }
        else
        if(subchild2->getBalanceFactor()==-1)
        {
            subtree->getBalanceFactor()=1;
            subchild->getBalanceFactor()=0;
        }
        else
        {
            subchild->getBalanceFactor()=-1;
            subtree->getBalanceFactor()=0;
        }
        subchild2->getBalanceFactor()=0;
    }

    Node* rotationRightLeft(Node *subtree, Node *subchild)
    {
        Node *father=subtree->goBack(), *subchild2, *leaf, *leaf2;
        if(subchild==nullptr) throw std::out_of_range("rotationRightLeft(Node *subtree) -> no right child. TreeMap.h");
        subchild2=subchild->goLeft();

        if(subchild2==nullptr) throw std::out_of_range("rotationRightLeft(Node *subtree) -> no right->left child. TreeMap.h");
        leaf=subchild2->goLeft();
        leaf2=subchild2->goRight();

        if(subtree->isRight())
        {
            father->changeRight(subchild2);
        }
        else
        {
            father->changeLeft(subchild2);
        }

        subchild2->changeBack(father);
        subchild2->changeLeft(subtree);
        subchild2->changeRight(subchild);
        subchild->changeBack(subchild2);
        subchild->changeLeft(leaf2);
        subtree->changeBack(subchild2);
        subtree->changeRight(leaf);
        if(leaf!=nullptr) leaf->changeBack(subtree);
        if(leaf2!=nullptr) leaf2->changeBack(subchild);

        return subchild2;
    }

    void rotationRightLeftAdding(Node *subtree)
    {
        Node *subchild=subtree->goRight(), *subchild2=rotationRightLeft(subtree, subchild);

        if(subchild2->getBalanceFactor()==-1)
        {
            subtree->getBalanceFactor()=1;
            subchild->getBalanceFactor()=0;
        }
        else
        if(subchild2->getBalanceFactor()==0)
        {
            subtree->getBalanceFactor()=0;
            subchild->getBalanceFactor()=0;
        }
        else    // ==1
        {
            subtree->getBalanceFactor()=0;
            subchild->getBalanceFactor()=-1;
        }
        subchild2->getBalanceFactor()=0;
    }

    void rotationRightLeftDeleting(Node *subtree)
    {
        Node *subchild=subtree->goRight(), *subchild2=rotationRightLeft(subtree, subchild);

        if(subchild2->getBalanceFactor()==0)
        {
            subtree->getBalanceFactor()=0;
            subchild->getBalanceFactor()=0;
        }
        else
        if(subchild2->getBalanceFactor()==-1)
        {
            subtree->getBalanceFactor()=0;
            subchild->getBalanceFactor()=1;
        }
        else
        {
            subchild->getBalanceFactor()=0;
            subtree->getBalanceFactor()=-1;
        }
        subchild2->getBalanceFactor()=0;
    }

    // adding node
    void addNodeToGuard(Node *node)
    {
        node->changeBack(guard);
        guard->changeLeft(node);
        guard->getBalanceFactor()=1;
        size=1;
    }

    void addNodeWithoutGuard(Node *node)    // without setting fistLeaf, check for balance
    {
        Node *tmp=guard->goLeft(), *previous;
        ++size;

        while(true)
        {
            if(tmp->getValue().first>node->getValue().first)    // left
            {
                if(tmp->goLeft()!=nullptr)
                {
                    tmp=tmp->goLeft();
                }
                else
                {
                    tmp->changeLeft(node);
                    node->changeBack(tmp);
                    break;
                }
            }
            else    // right
            {
                if(tmp->goRight()!=nullptr)
                {
                    tmp=tmp->goRight();
                }
                else
                {
                    tmp->changeRight(node);
                    node->changeBack(tmp);
                    break;
                }
            }
        }

        if(tmp->getBalanceFactor()!=0)
        {
            tmp->getBalanceFactor()=0;
            return;
        }
        else
        {
            if(tmp->goRight()!=nullptr) // new node added to the right
            {
                tmp->getBalanceFactor()=-1;
            }
            else
            {
                tmp->getBalanceFactor()=1;
            }
        }

        previous=tmp;
        tmp=tmp->goBack();

        while(tmp!=guard)
        {
            if(tmp->goLeft()==previous) ++(tmp->getBalanceFactor());
            else --(tmp->getBalanceFactor());

            if(tmp->getBalanceFactor()>1)  // left is bigger
            {
                if(tmp->goLeft()!=nullptr&&tmp->goLeft()->getBalanceFactor()==1)
                {
                    rotationRightAdding(tmp);
                }
                else
                {
                    rotationLeftRightAdding(tmp);
                }
                return;
            }
            else
            if(tmp->getBalanceFactor()<-1)    // right is bigger
            {
                if(tmp->goRight()!=nullptr&&tmp->goRight()->getBalanceFactor()==1)
                {
                    rotationRightLeftAdding(tmp);
                }
                else
                {
                    rotationLeftAdding(tmp);
                }
                return;
            }
            previous=tmp;
            tmp=tmp->goBack();
        }
    }

    void addNode(Node *node)
    {
        if(guard->goLeft()==nullptr)
        {
            addNodeToGuard(node);
            return;
        }
        addNodeWithoutGuard(node);
    }

    // other
    void deleteNodeWithSetFatherAndCopyValue(Node *node, Node *prev)
    {
        if(prev!=nullptr) prev->changeValue(node->getValue());
        if(node->goBack()!=nullptr)
        {
            if(node->isRight()) node->goBack()->changeRight(nullptr);
            else node->goBack()->changeLeft(nullptr);
        }
        delete node;
    }

    Node* deleteNodeRecursion(Node *node, Node *prev) // check for balance
    {
        Node *tmp=node->goBack(), *recur=node;

        // first case
        if(node->goLeft()!=nullptr&&node->goRight()!=nullptr)
        {
            recur=recur->operator++();
            node->changeValue(recur->getValue());
            return deleteNodeRecursion(recur, node);
        }
        else
        // second case
        if(node->goLeft()!=nullptr&&node->goRight()==nullptr)
        {
            if(node->isRight())
            {
                tmp->changeRight(node->goLeft());
                node->goLeft()->changeBack(tmp);
            }
            else
            {
                tmp->changeLeft(node->goLeft());
                node->goLeft()->changeBack(tmp);
            }
            deleteNodeWithSetFatherAndCopyValue(node, prev);
            return tmp;
        }
        else
        if(node->goLeft()==nullptr&&node->goRight()!=nullptr)
        {
            if(node->isRight())
            {
                tmp->changeRight(node->goRight());
                node->goRight()->changeBack(tmp);
            }
            else
            {
                tmp->changeLeft(node->goRight());
                node->goRight()->changeBack(tmp);
            }
            deleteNodeWithSetFatherAndCopyValue(node, prev);
            return tmp;
        }
        else
        // third case
        {
            deleteNodeWithSetFatherAndCopyValue(node, prev);
            return tmp;
        }
    }

    void rebalanceDeletedTree(Node* node)
    {
        if(node!=guard)
        {
            Node *prev=node;
            // 1. real deleted node was a leaf then at least father`s one child is nullptr,
            // father have bf = -1, 0 or 1 (decided by nullptr)
            // 2. real deleted node was in straight branch but that branch was limited,
            // had height of 3 (from father) (father has another child) and now have only one member
            // father have now bf=0

            if(node->goLeft()==nullptr&&node->goRight()==nullptr) node->getBalanceFactor()=0;
            // the real deleted node had to be where is nullptr now
            else if(node->goLeft()!=nullptr&&node->goRight()==nullptr)
            {
                node->getBalanceFactor()=1;
                return;
            }
            else if(node->goLeft()==nullptr&&node->goRight()!=nullptr)
            {
                node->getBalanceFactor()=-1;
                return;
            }
            // when real deleted node was on branch
            else node->getBalanceFactor()=0;

            node=node->goBack();

            while(node!=guard)
            {
                if(node->getBalanceFactor()==0) // from 0 to -1, 1
                {
                    if(prev->isRight()) node->getBalanceFactor()=1;
                    else node->getBalanceFactor()=-1;
                    return;
                }
                else
                // from -1, 1 to 0 if it can be
                if(node->getBalanceFactor()==1&&!(prev->isRight())) node->getBalanceFactor()=0;
                else
                if(node->getBalanceFactor()==-1&&prev->isRight()) node->getBalanceFactor()=0;
                else
                // with rotations, when deleted node was in lighter subtree
                {
                    if(node->getBalanceFactor()==-1&&node->goRight()->getBalanceFactor()==0)
                    {
                        rotationLeftDeleting(node);
                        return;
                    }
                    else
                    if(node->getBalanceFactor()==1&&node->goLeft()->getBalanceFactor()==0)
                    {
                        rotationRightDeleting(node);
                        return;
                    }
                    else
                    if(node->getBalanceFactor()==-1&&node->goRight()->getBalanceFactor()==-1) rotationLeftDeleting(node);
                    else
                    if(node->getBalanceFactor()==1&&node->goLeft()->getBalanceFactor()==1) rotationRightDeleting(node);
                    else
                    // bf of node and child are opposed
                    if(node->getBalanceFactor()==-1&&node->goLeft()->getBalanceFactor()==1) rotationRightLeftDeleting(node);
                    else rotationLeftRightDeleting(node);
                }
                prev=node;
                node=node->goBack();
            }
        }
    }


        /*Node *tmp=node->goBack();
        --size;
        // first case
        if(node->goLeft()==nullptr&&node->goRight()==nullptr)
        {
            if(node->isRight())
            {
                tmp->changeRight(nullptr);
            }
            else
            {
                tmp->changeLeft(nullptr);
            }
        }
        else
        // second case
        if((node->goLeft()==nullptr&&node->goRight!=nullptr))
        {
            if(tmp->goLeft()==node) // left
            {
                tmp->changeLeft(node->goRight());
            }
            else       // right
            {
                tmp->changeRight(node->goRight());
            }
            node->goRight()->changeBack()=tmp;
        }
        else
        if(node->goLeft()!=nullptr&&node->goRight==nullptr)
        {
            if(tmp->goLeft()==node) // left
            {
                tmp->changeLeft(node->goLeft());
            }
            else       // right
            {
                tmp->changeRight(node->goLeft());
            }
            node->goLeft()->changeBack()=tmp;
        }
        else
        // third case
        {
            tmp=++node;
            node->getValue()=tmp->getValue();
            node=tmp;
        }
        delete node;*/

    void deleteNode(Node *node)
    {
        rebalanceDeletedTree(deleteNodeRecursion(node, nullptr)); // deleteNodeRecursion return father of real deleted node
        --size;
    }

    Node* copyNodes(Node *other)
    {
        Node *tmp, *tmp2;
        if(other==nullptr) return nullptr;

        tmp=new Node(other->getValue());

        tmp2=copyNodes(other->goLeft());
        if(tmp2!=nullptr)
        {
            tmp2->changeBack(tmp);
            tmp->changeLeft(tmp2);
        }

        tmp2=copyNodes(other->goRight());
        if(tmp2!=nullptr)
        {
            tmp2->changeBack(tmp);
            tmp->changeRight(tmp2);
        }

        return tmp;
    }

    int rebalance(Node *node)
    {
        int l=0, r=0;
        if(node==nullptr) return 0;

        l=rebalance(node->goLeft());
        r=rebalance(node->goRight());

        if(l-r>1)
        {
            rotationRight(node);
        }
        else
        if(l-r<-1)
        {
            rotationLeft(node);
        }

        return l>r ? l+1 : r+1;
    }

    Node* findNode(const KeyType& key) const
    {
        Node *tmp=guard->goLeft();

        while(tmp!=nullptr)
        {
            if(key==tmp->getValue().first)
            {
                return tmp;
            }
            else
            if(key<tmp->getValue().first) tmp=tmp->goLeft();
            else tmp=tmp->goRight();
        }
        return guard;
    }

public:

    void print()
    {
        print2(guard->goLeft());
    }

    void print2(Node *node, int con=0)
    {
        if(node->goRight()!=nullptr)
        {
            print2(node->goRight(), con+1);
            std::cout <<  node->getValue().first << ":R" << node->goRight()->getValue().first << " BF: " << node->getBalanceFactor() << ":" << node->goRight()->getBalanceFactor() << std::endl;
        }

        for (int i=0; i<con; ++i)
        {
            //std::cout << "  ";
        }
        //std::cout << node->getValue().first << std::endl;

        if(node->goLeft()!=nullptr)
        {
            print2(node->goLeft(), con+1);
            std::cout <<  node->getValue().first << ":L" << node->goLeft()->getValue().first << " BF: " << node->getBalanceFactor() << ":" << node->goLeft()->getBalanceFactor() << std::endl;
        }

    }

public:

    TreeMap()
    :size(0), guard(new Node()) {}

    TreeMap(std::initializer_list<value_type> list)
    :size(list.size()), guard(new Node())
    {
        Node *node;
        auto it=list.begin();

        node=new Node(*it);
        ++it;
        addNodeToGuard(node);

        for (; it!=list.end(); ++it)
        {
            node=new Node(*it);
            addNodeWithoutGuard(node);
        }
    }

    TreeMap(const TreeMap& other)
    :size(other.size)
    {
        guard=copyNodes(other.guard);
    }

    TreeMap(TreeMap&& other)
    :size(other.size), guard(other.guard)
    {
        other.guard=nullptr;
    }

    ~TreeMap()
    {
        deleteTreeMap(guard);
    }

    TreeMap& operator=(const TreeMap& other)
    {
        if(this!=&other)
        {
            deleteTreeMap(guard->goLeft());
            guard->changeLeft(copyNodes(other.guard->goLeft()));
            if(guard->goLeft()!=nullptr) guard->goLeft()->changeBack(guard);
            size=other.size;
        }
        return *this;
    }

    TreeMap& operator=(TreeMap&& other)
    {
        if(this!=&other)
        {
            if(guard==nullptr) throw std::domain_error("Guard non exist. TreeMap.h");
            if(size!=0) deleteTreeMap(guard->goLeft());

            guard->changeLeft(other.guard->goLeft());
            other.guard->changeLeft(nullptr);
            size=other.size;
            other.size=0;
        }
        return *this;
    }

    bool isEmpty() const
    {
        if(size==0) return true;
        return false;
    }

    mapped_type& operator[](const key_type& key)
    {
        Node *tmp=findNode(key);
        if(tmp==guard)
        {
            tmp=new Node(key);
            addNode(tmp);
        }
        return tmp->getValue().second;
    }

    const mapped_type& valueOf(const key_type& key) const
    {
        Node *tmp=findNode(key);
        if(tmp==guard) throw std::out_of_range("There is no such key value. TreeMap.h");

        return tmp->getValue().second;
    }

    mapped_type& valueOf(const key_type& key)
    {
        Node *tmp=findNode(key);
        if(tmp==guard) throw std::out_of_range("There is no such key value. TreeMap.h");

        return tmp->getValue().second;
    }

    const_iterator find(const key_type& key) const
    {
        const_iterator it(findNode(key));
        return it;
    }

    iterator find(const key_type& key)
    {
        iterator it(findNode(key));
        return it;
    }

    void remove(const key_type& key)
    {
        Node *tmp=findNode(key);
        if(tmp==guard) throw std::out_of_range("No such pair find. Cannot remove. TreeMap.h");
        deleteNode(tmp);
    }

    void remove(const const_iterator& it)
    {
        Node *tmp=it.getNode();
        if(tmp==guard) throw std::out_of_range("Bad iterator. Cannot remove. TreeMap.h");
        deleteNode(tmp);
    }

    size_type getSize() const
    {
        return size;
    }

    bool operator==(const TreeMap& other) const
    {
        if(this!=&other)
        {
            if(size!=other.size) return false;

            for (auto it=cbegin(), itOther=other.cbegin(); it!=cend(); ++it, ++itOther)
            {
                if(it->first!=itOther->first||it->second!=itOther->second) return false;
            }
        }
        return true;
    }

    bool operator!=(const TreeMap& other) const
    {
        return !(*this == other);
    }

    iterator begin()
    {
        iterator tmp(findFirstLeaf());
        return tmp;
    }

    iterator end()
    {
        iterator tmp(findLastLeaf());
        return tmp;
    }

    const_iterator cbegin() const
    {
        const_iterator tmp(findFirstLeaf());
        return tmp;
    }

    const_iterator cend() const
    {
        const_iterator tmp(findLastLeaf());
        return tmp;
    }

    const_iterator begin() const
    {
        return cbegin();
    }

    const_iterator end() const
    {
        return cend();
    }
};

template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::ConstIterator
{
    friend class TreeMap;

private:
    Node *node;

    ConstIterator(Node *joint)
    :node(joint) {}

    inline Node* getNode() const
    {
        return node;
    }

public:
    using reference = typename TreeMap::const_reference;
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = typename TreeMap::value_type;
    using pointer = const typename TreeMap::value_type*;
    //using value_type = std::pair<const key_type, mapped_type>;

    explicit ConstIterator()
    {}

    ConstIterator(const ConstIterator& other)
    :node(other.node) {}

    ConstIterator& operator++()
    {
        if(node==nullptr||node->goBack()==nullptr) throw std::out_of_range("Cannot increment. TreeMap.h");

        node=node->operator++();
        return *this;
    }

    ConstIterator operator++(int)
    {
        ConstIterator tmp=*this;
        operator++();
        return tmp;
    }

    ConstIterator& operator--()
    {
        if(node==nullptr||(node->goLeft()==nullptr&&node->goRight()==nullptr&&(node->goBack()==nullptr||(node->goBack()!=nullptr&&!(node->isRight())))))
        throw std::out_of_range("Cannot decrement. TreeMap.h");

        node=node->operator--();
        return *this;
    }

    ConstIterator operator--(int)
    {
        ConstIterator tmp=*this;
        operator--();
        return tmp;
    }

    reference operator*() const
    {
        if(node==nullptr||node->goBack()==nullptr) throw std::out_of_range("Cannot dereference. TreeMap.h");

        reference tmp(node->getValue());
        return tmp;
    }

    pointer operator->() const
    {
        if(node==nullptr||node->goBack()==nullptr) throw std::out_of_range("Cannot dereference. TreeMap.h");

        pointer tmp=&(node->getValue());
        return tmp;
    }

    bool operator==(const ConstIterator& other) const
    {
        if(node==nullptr||other.node==nullptr) throw std::out_of_range("Cannot compare. Iterator do not point on anything. TreeMap.h");

        if(node->goBack()==nullptr&&(other.node)->goBack()==nullptr) return true;

        if(node->getValue()==other.node->getValue()&&node->goBack()!=nullptr&&other.node->goBack()!=nullptr) return true;

        return false;
    }

    bool operator!=(const ConstIterator& other) const
    {
        return !(*this == other);
    }
};

template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::Iterator : public TreeMap<KeyType, ValueType>::ConstIterator
{
public:
    using reference = typename TreeMap::reference;
    using pointer = typename TreeMap::value_type*;

    explicit Iterator()
    {}

    Iterator(const ConstIterator& other)
        : ConstIterator(other)
    {}

    Iterator(Node *joint)
    :ConstIterator(joint) {}

    Iterator& operator++()
    {
        ConstIterator::operator++();
        return *this;
    }

    Iterator operator++(int)
    {
        auto result = *this;
        ConstIterator::operator++();
        return result;
    }

    Iterator& operator--()
    {
        ConstIterator::operator--();
        return *this;
    }

    Iterator operator--(int)
    {
        auto result = *this;
        ConstIterator::operator--();
        return result;
    }

    pointer operator->() const
    {
        return &this->operator*();
    }

    reference operator*() const
    {
        // ugly cast, yet reduces code duplication.
        return const_cast<reference>(ConstIterator::operator*());
    }
};

template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::Node
{
    friend class TreeMap;
    friend class ConstIterator;

private:
    using pair = std::pair<const KeyType, ValueType>;
    using normalPair = std::pair<KeyType, ValueType>;

    int balanceFactor;
    Node *father, *left, *right;
    pair value;

    Node(const KeyType& key={}, Node *father=nullptr, Node *left=nullptr, Node *right=nullptr)
    :balanceFactor(0), father(father), left(left), right(right), value({key, ValueType{}}) {}

    Node(const normalPair& pairVariables, Node *father=nullptr, Node *left=nullptr, Node *right=nullptr)
    :balanceFactor(0), father(father), left(left), right(right), value(pairVariables) {}

    inline bool isRight()
    {
        if(father->left==this) return false;
        return true;
    }

    Node* operator++()   // in-order
    {
        Node *tmp=right, *tmp2=this;
        if(right!=nullptr)
        {
            while(tmp->left!=nullptr) tmp=tmp->left;
            return tmp;
        }

        tmp=this->father;
        while(tmp->right==tmp2)
        {
            tmp2=tmp;
            tmp=tmp->father;
        }
        return tmp;
    }

    Node* operator--()   // in-order
    {
        Node *tmp=left;
        if(tmp!=nullptr)
        {
            while(tmp->right!=nullptr) tmp=tmp->right;
            return tmp;
        }

        tmp=this->father;
        while(!tmp->isRight())
        {
            tmp=tmp->father;
        }
        return tmp;
    }

    inline void changeLeft(Node *newNode)
    {
        left=newNode;
    }

    inline void changeRight(Node *newNode)
    {
        right=newNode;
    }

    inline void changeBack(Node *newNode)
    {
        father=newNode;
    }

    inline Node* goLeft()
    {
        return left;
    }

    inline Node* goRight()
    {
        return right;
    }

    inline Node* goBack()
    {
        return father;
    }

    inline pair& getValue()
    {
        return value;
    }

    inline void changeValue(const pair& nodeValue)
    {
        // ugly cast, yet reduces code runtime.
        const_cast<KeyType&>(value.first)=nodeValue.first;
        value.second=nodeValue.second;
    }

    inline int& getBalanceFactor()
    {
        return balanceFactor;
    }
};

}

#endif /* AISDI_MAPS_MAP_H */
