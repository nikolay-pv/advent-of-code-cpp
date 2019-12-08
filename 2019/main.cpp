#include <iostream>
#include <vector>
#include <map>
#include <memory>
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
    vector<LayerInfo> data;
    //data.resize(numLayers);
    for(long layer = 0; layer != numLayers; ++layer)
    {
        long zeros = 0, ones = 0, twos = 0;
        for(long px = 0; px != wide*tall; ++px)
        {
            const long pos = layer*wide*tall + px;
            switch (input[pos])
            {
                case '0':
                    ++zeros;
                    break;
                case '1':
                    ++ones;
                    break;
                case '2':
                    ++twos;
                    break;
                //default:
                //    cerr << "Not interested in this color!" << endl;
            }
        }
        data.push_back(LayerInfo{layer, zeros, ones, twos});
    }
    for_each(data.cbegin(), data.cend(), [](const auto& el){ cerr << el << endl;});

    sort(data.begin(), data.end(),
            [](const auto& el, const auto& anotherEl)
            { return el.zeros < anotherEl.zeros; });
    auto minLayer = data.front();
    cout << "The layer with least zeros is " << minLayer.index << endl;
    cout << "The number of 1 digits multiplied by the number of 2 digits at this layer is ";
    cout << minLayer.ones * minLayer.twos << endl;
    return 0;
}

