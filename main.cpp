#include <cstddef>
#include <cstdlib>
#include <string>
#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>

//{{6,2}, {5,8}, {4,3}, {7,6}, {55, -9}, {8,99}, {2,7}, {3,22}, {0,3}, {77,0}};

#include "TreeMap.h"
#include "HashMap.h"

using namespace std;

static const std::string PATH="../../src/performanceTests/";
static const std::size_t loopLenght=10;
static const std::size_t additionalKeyBegin=2000000;

namespace
{

template <typename K, typename T>
using Tree = aisdi::TreeMap<K, T>;

template <typename K, typename T>
using Hash = aisdi::HashMap<K, T>;

template <typename iniList>
void clearFiles(iniList& container)
{
    std::fstream file;
    std::string name;
    for (auto it=container.cbegin(); it!=container.cend(); ++it)
    {
        name=PATH;
        name=name+*it;
        file.open(name, std::ios_base::out | std::ios_base::trunc);
        if(!file.good())
        {
            throw std::domain_error("Cannot access to file to erase. main.cpp ");
        }

        file.close();
    }
}

template <typename iniList>
void giveSpace(iniList& container)
{
    std::fstream file;
    std::string name;
    for (auto it=container.cbegin(); it!=container.cend(); ++it)
    {
        name=PATH;
        name=name+*it;
        file.open(name, std::ios::app);
        if(!file.good())
        {
            throw std::domain_error("Cannot access to file to erase. main.cpp ");
        }
        file << "\n";

        file.close();
    }
}

template <typename KeyType, typename ValueType, typename iniList>
void performTest(const iniList& initialVal, const size_t loop, std::string fileNameT, std::string fileNameH, const size_t mode)
{
    Tree<KeyType, ValueType> collectionT;
    Hash<KeyType, ValueType> collectionH;
    std::fstream fileT;
    std::fstream fileH;

    for (auto it=initialVal.begin(); it!=initialVal.end(); ++it)
    {
        collectionT[(*it).first]=(*it).second;
        collectionH[(*it).first]=(*it).second;
    }
    auto start=std::chrono::high_resolution_clock::now();
    auto finish=std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedT = finish - start;
    std::chrono::duration<double> elapsedH = finish - start;

    switch(mode)
    {
    case 0:   // begin-end
    {
        start=std::chrono::high_resolution_clock::now();
        for (size_t i=0; i<loop; i++)
        {
            collectionT.begin();
            collectionT.end();
        }
        finish=std::chrono::high_resolution_clock::now();
        elapsedT = finish - start;

        start=std::chrono::high_resolution_clock::now();
        for (size_t i=0; i<loop; i++)
        {
            collectionH.begin();
            collectionH.end();
        }
        finish=std::chrono::high_resolution_clock::now();
        elapsedH = finish - start;
        break;
    }

    case 1:   //operator[]
    {
        if(collectionT.getSize()<loop||collectionH.getSize()<loop) throw out_of_range("In test 'operator[]' loop is too big case 1.");

        start=std::chrono::high_resolution_clock::now();
        for (size_t i=0; i<loop; i++)
        {
            collectionT[i];
            collectionT[i+additionalKeyBegin];
        }
        finish=std::chrono::high_resolution_clock::now();
        elapsedT = finish - start;

        start=std::chrono::high_resolution_clock::now();
        for (size_t i=0; i<loop; i++)
        {
            collectionH[i];
            collectionH[i+additionalKeyBegin];
        }
        finish=std::chrono::high_resolution_clock::now();
        elapsedH = finish - start;
        break;
    }

    case 2:   //find
    {
        if(collectionT.getSize()<loop||collectionH.getSize()<loop) throw out_of_range("In test 'find' loop is too big case 2.");

        start=std::chrono::high_resolution_clock::now();
        for (size_t i=1; i<=loop; i++)
        {
            collectionT.find(i);
            collectionT.find(collectionT.getSize()-i);
        }
        finish=std::chrono::high_resolution_clock::now();
        elapsedT = finish - start;

        start=std::chrono::high_resolution_clock::now();
        for (size_t i=1; i<=loop; i++)
        {
            collectionH.find(i);
            collectionH.find(collectionH.getSize()-i);
        }
        finish=std::chrono::high_resolution_clock::now();
        elapsedH = finish - start;
        break;
    }

    case 3:   //remove
    {
        if(collectionT.getSize()<loop||collectionH.getSize()<loop) throw out_of_range("In test 'remove' loop is too big case 3.");

        start=std::chrono::high_resolution_clock::now();
        for (size_t i=0; i<loop; i++)
        {
            collectionT.remove(i);
        }
        finish=std::chrono::high_resolution_clock::now();
        elapsedT = finish - start;

        start=std::chrono::high_resolution_clock::now();
        for (size_t i=0; i<loop; i++)
        {
            collectionH.remove(i);
        }
        finish=std::chrono::high_resolution_clock::now();
        elapsedH = finish - start;
        break;
    }

    case 4:   //assignment
    {
        Tree<KeyType, ValueType> tmpT;
        Hash<KeyType, ValueType> tmpH;

        start=std::chrono::high_resolution_clock::now();
        for (size_t i=0; i<loop; i++)
        {
            tmpT=collectionT;
        }
        finish=std::chrono::high_resolution_clock::now();
        elapsedT = finish - start;

        start=std::chrono::high_resolution_clock::now();
        for (size_t i=0; i<loop; i++)
        {
            tmpH=collectionH;
        }
        finish=std::chrono::high_resolution_clock::now();
        elapsedH = finish - start;
        break;
    }

    case 5:   //iterator_forward
    {
        start=std::chrono::high_resolution_clock::now();
        for (auto it=collectionT.cbegin(); it!=collectionT.cend(); ++it)
        {

        }
        finish=std::chrono::high_resolution_clock::now();
        elapsedT = finish - start;

        start=std::chrono::high_resolution_clock::now();
        for (auto it=collectionH.cbegin(); it!=collectionH.cend(); ++it)
        {

        }
        finish=std::chrono::high_resolution_clock::now();
        elapsedH = finish - start;
        break;
    }


    default:
        throw std::out_of_range("Non existing test. main.cpp" );
    }

    fileNameT=PATH+fileNameT;
    fileNameH=PATH+fileNameH;

    fileT.open(fileNameT, std::ios::app);
    if(!fileT.good())
    {
        throw std::domain_error("Cannot access to file. main.cpp ");
    }
    fileH.open(fileNameH, std::ios::app);
    if(!fileH.good())
    {
        throw std::domain_error("Cannot access to file. main.cpp ");
    }
    fileT << elapsedT.count() << "\n";
    fileH << elapsedH.count() << "\n";

    fileT.close();
    fileH.close();
}

} // namespace

int main(int argc, char** argv)
{
    std::cout << "HELLO WORLD" << std::endl;
    const std::size_t repeatCount = argc > 1 ? std::atoll(argv[1]) : 10000;
    std::cout << repeatCount << std::endl;

    std::vector<std::pair<int, int>> testVal;
    std::vector<std::string> fileNamesT;
    std::vector<std::string> fileNamesH;

    fileNamesT.push_back("testT0_begin-end.txt");
    fileNamesH.push_back("testH0_begin-end.txt");

    fileNamesT.push_back("testT1_operator[].txt");
    fileNamesH.push_back("testH1_operator[].txt");

    fileNamesT.push_back("testT2_find.txt");
    fileNamesH.push_back("testH2_find.txt");

    fileNamesT.push_back("testT3_remove.txt");
    fileNamesH.push_back("testH3_remove.txt");

    fileNamesT.push_back("testT4_assignment.txt");
    fileNamesH.push_back("testH4_assignment.txt");

    fileNamesT.push_back("testT5_iterator_forward.txt");
    fileNamesH.push_back("testH5_iterator_forward.txt");

    clearFiles<std::vector<std::string>>(fileNamesT);
    clearFiles<std::vector<std::string>>(fileNamesH);

    for (int i=0; i<1000; i++)
    {
        testVal.push_back({i, rand()%7000});
    }

    for (std::size_t i = 1; i <= repeatCount; ++i)
    {
        performTest<int, int, vector<pair<int, int>>>(testVal, loopLenght, fileNamesT[0], fileNamesH[0], 0);
        performTest<int, int, vector<pair<int, int>>>(testVal, loopLenght, fileNamesT[1], fileNamesH[1], 1);
        performTest<int, int, vector<pair<int, int>>>(testVal, loopLenght, fileNamesT[2], fileNamesH[2], 2);
        performTest<int, int, vector<pair<int, int>>>(testVal, loopLenght, fileNamesT[3], fileNamesH[3], 3);
        performTest<int, int, vector<pair<int, int>>>(testVal, loopLenght, fileNamesT[4], fileNamesH[4], 4);
        performTest<int, int, vector<pair<int, int>>>(testVal, loopLenght, fileNamesT[5], fileNamesH[5], 5);

        testVal.push_back({i+999, rand()%7000});
    }
    /*Tree<int, int> tmp2={{6,2}, {5,8}, {4,3}, {7,6}, {55, -9}, {8,99}, {2,7}, {3,22}, {0,3}, {77,0}};
    Tree<std::string, int> tmp={{"Jan",2}, {"Mirek",8}, {"Zdzislaw",3}, {"Malgosia",6}, {"Janusz", -9}, {"Marcin",99}, {"Seba",7}, {"Michal",22}, {"Joanna",3}, {"Marcepan",0}};
    tmp2.print();
    tmp.print();*/




    return 0;
}
