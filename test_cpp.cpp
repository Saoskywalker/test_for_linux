#include <iostream>
#include "test_cpp.h"
using namespace std;

// extern "C" int test_cpp(int argc, char *argv[]);
int test_cpp(int argc, char *argv[])
{
    cout << "Hello, world!" << endl;
    return 0;
}
