#include <list>
#include <iostream>
#include <typeinfo>
using namespace std;
int main()
{
    std::list<size_t> l;
    l.push_back(0);
    l.end();
    std::cout << typeid(l).name <<std::endl;
    l.emplace(l.end(),1);
    return 0;
}
