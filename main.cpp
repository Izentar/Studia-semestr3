#include <cstddef>
#include <cstdlib>
#include <string>

#include "TreeMap.h"

using namespace std;

namespace
{

template <typename K, typename V>
using Map = aisdi::TreeMap<K, V>;

/*void perfomTest()
{
  Map<int, std::string> map;
  map[1] = "TODO";
}*/

} // namespace

int main()//int argc, char** argv)
{
  //const std::size_t repeatCount = argc > 1 ? std::atoll(argv[1]) : 10000;
  //for (std::size_t i = 0; i < repeatCount; ++i)
    //perfomTest();


    Map<int, int> sth={{6,2}, {5,8}, {4,3}, {7,6}, {55, -9}, {8,99}, {2,7}, {3,22}, {0,3}, {77,0}};
    Map<int, int> sth2={{6,2}, {4,3}, {55, -9}, {8,99}, {2,7}, {3,22}, {0,3}, {77,0}};
    //={{55, -9}, {4,3}, {2,7}, {6,2}, {0,3}, {8,99}, {5,8}, {77,0}, {3,22}, {7,6}};



    for (auto it=sth.cbegin(); it!=sth.cend(); ++it)
    {
        cout << (*it).first << endl;
    }

    sth.remove(7);
    sth.remove(5);
    sth.remove(77);
    //sth.remove(0);
    //sth2[7]=6;
    //sth2[5]=8;

    if(sth==sth2) cout << "A " << endl;

    for (auto it=sth.cbegin(); it!=sth.cend(); ++it)
    {
        cout << (*it).first << endl;
    }

    cout << endl;

    sth.print();

  return 0;
}
