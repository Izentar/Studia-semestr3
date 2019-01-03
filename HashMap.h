#ifndef AISDI_MAPS_HASHMAP_H
#define AISDI_MAPS_HASHMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <vector>
#include <list>
#include <functional>

namespace aisdi
{

template <typename KeyType, typename ValueType>
class HashMap
{
    class Data;
    class NumberClipboard;

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

    friend ConstIterator;

private:
    std::size_t numberOfElements;
    NumberClipboard resizeNumber;
    std::vector<Data> variable;

    HashMap(NumberClipboard newSize)
    :numberOfElements(0), resizeNumber(newSize), variable(newSize.size()) {}

    size_type getPosition(const key_type& key) const // TO CHANGE, ONLY FOR THE DURATION OF TESTS !!!!!!!!!!!!!!!!!!
    {
        std::hash<int> hashFunction;
        return hashFunction(key)%resizeNumber.size();
    }

    size_type getPosition(const key_type& key, const NumberClipboard& numberToResize) const // TO CHANGE, ONLY FOR THE DURATION OF TESTS !!!!!!!!!!!!!!!!!!
    {
        std::hash<int> hashFunction;
        return hashFunction(key)%numberToResize.size();
    }

    std::vector<Data>& getBoard()
    {
        return variable;
    }

    const std::vector<Data>& getBoard() const
    {
        return variable;
    }

    void add(const value_type& value)
    {
        variable[getPosition(value.first)].add(value);
        ++numberOfElements;
    }

    void setResize(const NumberClipboard& newResize)
    {
        std::vector<Data> tmp(newResize.size());

        for (auto it=cbegin(); it!=cend(); ++it)
        {
            tmp[getPosition((*it).first, newResize)].simpleCopy(*it);
        }

        variable=std::move(tmp);
        resizeNumber=newResize;
    }

    void resize()
    {
        NumberClipboard newResize=resizeNumber;
        ++newResize;
        setResize(newResize);
    }

    void copyVector(const HashMap& other)
    {
        variable.resize(other.resizeNumber.size());
        auto it=other.begin();
        for(; it!=other.end(); ++it)
        {
            variable[getPosition((*it).first)][(*it).first]=(*it).second;
        }
    }

public:

    HashMap()
    :numberOfElements(0), resizeNumber(0), variable(resizeNumber.size()) {}

    HashMap(std::initializer_list<value_type> list)
    :numberOfElements(list.size())
    {
        while(list.size()*2>resizeNumber.size())
        {
            ++resizeNumber;
        }
        variable.resize(resizeNumber.size());

        for (auto it=list.begin(); it!=list.end(); ++it)
        {
            variable[getPosition((*it).first)].add(*it);
        }
    }

    HashMap(const HashMap& other)
    :numberOfElements(other.numberOfElements), resizeNumber(other.resizeNumber)//, variable(other.variable)
    {
        copyVector(other);
    }

    HashMap(HashMap&& other)
    :numberOfElements(other.numberOfElements), resizeNumber(other.resizeNumber), variable(std::move(other.variable)) {}

    HashMap& operator=(const HashMap& other)
    {
        if(this!=&other)
        {
            variable.clear();
            copyVector(other);
            numberOfElements=other.numberOfElements;
            resizeNumber=other.resizeNumber;
        }

        return *this;
    }

    HashMap& operator=(HashMap&& other)
    {
        if(this!=&other)
        {
            variable=std::move(other.variable);
            numberOfElements=other.numberOfElements;
            resizeNumber=other.resizeNumber;
        }
        return *this;
    }

    bool isEmpty() const
    {
        if(numberOfElements==0) return true;
        return false;
    }

    mapped_type& operator[](const key_type& key)
    {
        if(numberOfElements*2>resizeNumber.size()) this->resize();

        size_type tmp=variable[getPosition(key)].size();
        mapped_type &ret=variable[getPosition(key)][key];
        if(tmp!=variable[getPosition(key)].size()) ++numberOfElements;
        return ret;
    }

    const mapped_type& valueOf(const key_type& key) const
    {
        return variable[getPosition(key)].find(key).second;
    }

    mapped_type& valueOf(const key_type& key)
    {
        // ugly cast, yet reduces code duplication.
        return const_cast<mapped_type&>(variable[getPosition(key)].find(key).second);
    }

    const_iterator find(const key_type& key) const
    {
        size_type tmpint=variable[getPosition(key)].findPosition(key);
        if(tmpint==variable[getPosition(key)].size())
        {
            const_iterator tmp(variable.size(), 0, this);
            return tmp;
        }
        const_iterator tmp(getPosition(key), tmpint, this);
        return tmp;
    }

    iterator find(const key_type& key)
    {
        size_type tmpint=variable[getPosition(key)].findPosition(key);
        if(tmpint==variable[getPosition(key)].size())
        {
            const_iterator tmp(variable.size(), 0, this);
            iterator tmp2(tmp);
            return tmp2;
        }
        const_iterator tmp(getPosition(key), tmpint, this);
        iterator tmp2(tmp);
        return tmp2;
    }

    void remove(const key_type& key)
    {
        variable[getPosition(key)].remove(key);
        --numberOfElements;
    }

    void remove(const const_iterator& it)
    {
        if(it.getPosition()==variable.size()) throw std::out_of_range("Cannot remove. HashMap.h");
        variable[it.getPosition()].removeFromPosition(it.getPositionData());
        --numberOfElements;
    }

    size_type getSize() const
    {
        return numberOfElements;
    }

    bool operator==(const HashMap& other) const
    {
        if(numberOfElements!=other.numberOfElements) return false;
        auto it=cbegin();
        auto it2=other.cbegin();
        for (; it!=cend(); ++it, ++it2)
        {
            if(*it!=*it2) return false;
        }
        return true;
    }

    bool operator!=(const HashMap& other) const
    {
        return !(*this == other);
    }

    const_iterator cbegin() const
    {
        for(size_type i=0; i<variable.size(); ++i)
        {
            if(!variable[i].empty())
            {
                const_iterator tmp(i, 0, this);
                return tmp;
            }
        }
        const_iterator tmp(variable.size(), 0, this);
        return tmp;

    }

    const_iterator cend() const
    {
        const_iterator tmp(variable.size(), 0, this);
        return tmp;
    }

    iterator begin()
    {
        iterator tmp(this->cbegin());
        return tmp;
    }

    iterator end()
    {
        iterator tmp(this->cend());
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
class HashMap<KeyType, ValueType>::ConstIterator
{
    friend HashMap;

public:
    using reference = typename HashMap::const_reference;
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = typename HashMap::value_type;
    using pointer = const typename HashMap::value_type*;

private:
    size_type position, positionData;
    const HashMap *container;

    ConstIterator(const size_type &posX, const size_type &posY, const HashMap *cont)
    :position(posX), positionData(posY), container(cont) {}

    const size_type& getPosition() const
    {
        return position;
    }

    const size_type& getPositionData() const
    {
        return positionData;
    }

public:

    explicit ConstIterator()
    :position(0), positionData(0), container(nullptr) {}

    ConstIterator(const ConstIterator& other)
    :position(other.position), positionData(other.positionData), container(other.container) {}

    ConstIterator& operator++()
    {
        if(container==nullptr||position==container->getBoard().size()) throw std::out_of_range("Cannot increment iterator. HashMap.h");

        if(container->getBoard()[position].canGoForward(positionData))
        {
            ++positionData;
        }
        else
        {
            positionData=0;
            ++position;
            for (; position<container->getBoard().size(); ++position)
            {
                if(!(container->getBoard()[position].empty())) return *this;
            }
            position=container->getBoard().size();
        }
        return *this;
    }

    ConstIterator operator++(int)
    {
        ConstIterator tmp(*this);
        operator++();
        return tmp;
    }

    ConstIterator& operator--()
    {
        if(container==nullptr||position==0) throw std::out_of_range("Cannot decrement iterator. HashMap.h");

        if(container->getBoard()[position].canGoBack(positionData))
        {
            --positionData;
            return *this;
        }
        else
        {
            for (std::ptrdiff_t i=position-1; i!=0; --i)
            {
                if(!(container->getBoard()[i].empty()))
                {
                    positionData=container->getBoard()[i].size()-1;
                    position=i;
                    return *this;
                }

            }
            throw std::out_of_range("Cannot decrement iterator. HashMap.h");
        }
    }

    ConstIterator operator--(int)
    {
        ConstIterator tmp(*this);
        operator--();
        return tmp;
    }

    reference operator*() const
    {
        if(position==container->getBoard().size()||!(container->getBoard()[position].positionExist(positionData))) throw std::out_of_range("Cannot dereference. HashMap.h");
        return container->getBoard()[position].getPair(positionData);
    }

    pointer operator->() const
    {
        return &(this->operator*());
    }

    bool operator==(const ConstIterator& other) const
    {
        if(container==nullptr||other.container==nullptr) throw std::out_of_range("Cannot compare. HashMap.h");

        if(position==container->getBoard().size()&&other.position==other.container->getBoard().size()) return true;

        if(position!=container->getBoard().size()&&other.position!=other.container->getBoard().size()&&container->getBoard()[position].getPair(positionData)
           == other.container->getBoard()[other.position].getPair(other.positionData)) return true;

        return false;
    }

    bool operator!=(const ConstIterator& other) const
    {
        return !(*this == other);
    }
};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::Iterator : public HashMap<KeyType, ValueType>::ConstIterator
{
public:
    using reference = typename HashMap::reference;
    using pointer = typename HashMap::value_type*;

    explicit Iterator()
    {}

    Iterator(const ConstIterator& other)
        : ConstIterator(other)
    {}

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
class HashMap<KeyType, ValueType>::NumberClipboard
{
    class ListOfPrimeNumber;

private:
    std::size_t position;
    static ListOfPrimeNumber numbers;

public:
    NumberClipboard()
    :position(0) {}

    NumberClipboard(const size_type pos)
    :position(pos) {}

    NumberClipboard(const NumberClipboard& other)
    :position(other.position) {}

    const std::size_t& operator++()
    {
        ++position;
        return numbers[position];
    }

    const std::size_t& operator++(int)
    {
        std::size_t tmp=position;
        operator++();
        return numbers[tmp];
    }

    const std::size_t& operator--()
    {
        --position;
        return numbers[position];
    }

    const std::size_t& operator--(int)
    {
        std::size_t tmp=position;
        operator--();
        return numbers[tmp];
    }

    NumberClipboard& operator=(const NumberClipboard& other)
    {
        position=other.position;
        return *this;
    }

    NumberClipboard& operator=(const NumberClipboard&& other)
    {
        position=std::move(other.position);
        return *this;
    }

    const std::size_t& size() const
    {
        return numbers[position];
    }

};

template <typename KeyType, typename ValueType>
typename HashMap<KeyType, ValueType>::NumberClipboard::ListOfPrimeNumber HashMap<KeyType, ValueType>::NumberClipboard::numbers;

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::NumberClipboard::ListOfPrimeNumber
{
    std::size_t board[29];

public:
    ListOfPrimeNumber()
    {
        board[0]=23;
        board[1]=53;
        board[2]=107;
        board[3]=223;
        board[4]=449;
        board[5]=907;
        board[6]=1823;
        board[7]=3659;
        board[8]=7321;
        board[9]=14653;
        board[10]=29311;
        board[11]=58631;
        board[12]=117269;
        board[13]=234539;
        board[14]=469099;
        board[15]=938207;
        board[16]=1876417;
        board[17]=3752839;
        board[18]=7505681;
        board[19]=15011389;
        board[20]=30022781;
        board[21]=60045577;
        board[22]=120091177;
        board[23]=240182359;
        board[24]=480364727;
        board[25]=960729461;
        board[26]=1921458943;
        board[27]=3842917907;
        board[28]=4294967291;
    }

    const std::size_t& operator[](const std::size_t index) const
    {
        return board[index];
    }

};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::Data
{
    using MainValue = std::pair<const KeyType, ValueType>;
    using Container = std::vector <MainValue>;

    bool isPresent;
    Container *additional;
    MainValue first;

    void changeMainValue(MainValue& toChange, const MainValue& value)
    {
        const_cast<KeyType&>(toChange.first)=value.first;
        toChange.second=value.second;
    }

public:
    Data()
        :isPresent(false), additional(nullptr) {}

    Data(const Data& other)
        :isPresent(other.isPresent), additional(new Container(*(other.additional))), first(other.first) {}

    Data(const Data&& other)
        :isPresent(other.isPresent), additional(other.additional), first(other.first)
    {
        other.additional=nullptr;
    }

    ~Data()
    {
        delete additional;
    }

    Data& operator=(const Data& other)
    {
        isPresent=other.isPresent;
        changeMainValue(first, other.first);
        delete additional;
        additional=new Container(*(other.additional));
        return *this;
    }

    size_type size() const
    {
        if(additional!=nullptr) return additional->size()+1;
        if(isPresent) return 1;
        return 0;
    }

    void add(const MainValue& value)
    {
        if(!isPresent)
        {
            isPresent=true;
            changeMainValue(first, value);
        }
        else
        {
            if(first.first==value.first) throw std::out_of_range("Cannot add. Key value found. HashMap.h");

            if(additional==nullptr)
            {
                additional=new Container;
                additional->push_back(value);
            }
            else
            {
                for (size_type i=0; i<additional->size(); ++i)
                {
                    if((*additional)[i].first==value.first) throw std::out_of_range("Cannot add. Key value found. HashMap.h");
                }
                additional->push_back(value);
            }
        }
    }

    void simpleCopy(const MainValue& value)
    {
        if(!isPresent)
        {
            isPresent=true;
            changeMainValue(first, value);
        }
        else
        {
            if(additional==nullptr)
            {
                additional=new Container;
            }
            additional->push_back(value);
        }
    }

    void remove(const KeyType& key)
    {
        if(!isPresent) throw std::out_of_range("Cannot remove. No key found. HashMap.h");
        if(first.first==key)
        {
            if(additional==nullptr)
            {
                isPresent=false;
            }
            else if(additional->size()==1)
            {
                changeMainValue(first, additional->front());
                delete additional;
                additional=nullptr;
            }
            else
            {
                changeMainValue(first, additional->back());
                additional->pop_back();
            }
        }
        else
        {
            if(additional==nullptr) throw std::out_of_range("Cannot remove. No key found. HashMap.h");
            else
            {
                for (size_type i=0; i<additional->size(); ++i)
                {
                    if((*additional)[i].first==key)
                    {
                        if(i!=(additional->size())-1)
                        {
                            changeMainValue((*additional)[i], additional->back());
                        }
                        additional->pop_back();
                        return;
                    }
                }
                throw std::out_of_range("Cannot remove. No key found. HashMap.h");
            }
        }
    }

    void removeFromPosition(const size_type& pos)
    {
        if(pos<this->size())
        {
            if(pos==0)
            {
                if(additional==nullptr)
                {
                    isPresent=false;
                }
                else if(additional->size()==1)
                {
                    changeMainValue(first, additional->front());
                    delete additional;
                    additional=nullptr;
                }
                else
                {
                    changeMainValue(first, additional->back());
                    additional->pop_back();
                }
            }
            else
            {
                changeMainValue((*additional)[pos], additional->back());
                additional->pop_back();
            }
        }
        else
        {
            throw std::out_of_range("Cannot remove. No key found. HashMap.h");
        }
    }

    const MainValue& find(const key_type& key) const
    {
        if(isPresent&&first.first==key)
        {
            return first;
        }

        if(additional!=nullptr)
        {
            for (size_type i=0; i<additional->size(); ++i)
            {
                if((*additional)[i].first==key) return (*additional)[i];
            }
        }
        throw std::out_of_range("Cannot find pair. HashMap.h");
    }

    size_type findPosition(const key_type& key) const
    {
        if(isPresent&&first.first==key)
        {
            return 0;
        }

        if(additional!=nullptr)
        {
            for (size_type i=0; i<additional->size(); ++i)
            {
                if((*additional)[i].first==key) return i+1;
            }
        }
        return size();
    }

    mapped_type& operator[](const key_type& key)
    {
        if(isPresent&&first.first==key)
        {
            return first.second;
        }

        if(additional!=nullptr)
        {
            for (size_type i=0; i<additional->size(); ++i)
            {
                if((*additional)[i].first==key) return (*additional)[i].second;
            }
        }

        MainValue tmp(key, ValueType{});

        if(!isPresent)
        {
            changeMainValue(first, tmp);
            isPresent=true;
            return first.second;
        }
        else
        if(additional==nullptr) additional=new Container;

        additional->push_back(tmp);
        return additional->back().second;
    }

    bool canGoForward(const size_type& pos) const
    {
        if(pos+1<this->size()) return true;
        return false;
    }

    bool positionExist(const size_type& pos) const
    {
        if(pos<this->size()) return true;
        return false;
    }

    bool canGoBack(const size_type& pos) const
    {
        if(isPresent&&pos!=0) return true;
        return false;
    }

    bool empty() const
    {
        return !isPresent;
    }

    const MainValue& getPair(const size_type& pos) const
    {
        if(pos==0) return first;
        return (*additional)[pos-1];
    }

    MainValue& getPair(const size_type& pos)
    {
        if(pos==0) return first;
        return (*additional)[pos-1];
    }

};

}

#endif /* AISDI_MAPS_HASHMAP_H */
