#include <iostream>
#include "core/F.hpp"

using namespace std;

int main()
{
    Numeric A(10, 10);
    try{
        A.Division(10, 0);
    }
    catch(const char& msg){
        cout << msg;
    }

    return 0;
}
