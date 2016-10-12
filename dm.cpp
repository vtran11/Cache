#include <iostream>
#include <fstream>
#include <iomanip>
#include <bitset>
#include <sstream>

using namespace std;

unsigned int RAM[0x10000];

ofstream output;

class Cache
{
private:
    char tag;
    string databit[16];
    
public:
    Cache()
    {
        tag = -1;
        for (int j =0; j < 16; j++)
            databit[j] = "FF";
    }
};
