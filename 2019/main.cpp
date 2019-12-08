#include <iostream>
#include <array>
using namespace std;

constexpr long wide = 25;
constexpr long tall = 6;

struct LayerInfo
{
    long index = 0;
    long zeros = 0;
    long ones = 0;
    long twos = 0;
    friend std::ostream& operator<<(std::ostream& os, const LayerInfo& obj);
};

std::ostream& operator<<(std::ostream& os, const LayerInfo& obj)
{
    os << "Layer info [ " << obj.index << " ](zeros, ones, twos): " << obj.zeros << " " << obj.ones << " " << obj.twos;
    return os;
}

////////////////////////////////////////////////////////////////////////////////
// main
int main()
{
    string input;
    cin >> input;

    long numLayers = input.size() / (wide * tall);
    //cerr << "numLayers " << numLayers << endl;
    for(long px = 0; px != wide*tall; ++px)
    {
        for(long layer = 0; layer != numLayers; ++layer)
        {
            const long pos = layer*wide*tall + px;
            if (input[pos] != '2') // transparent
            {
                // 0 is black, 1 is white
                const char res = input[pos] == '0' ? '.' : '*';
                cout << res;
                if ((px + 1) % wide == 0)
                    cout << endl;
                break;
            }
        }
    }
    return 0;
}

