#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <memory>
using namespace std;

struct Node
{
    friend bool operator==(const Node& lhs, const Node& rhs);
    friend bool operator==(const Node& lhs, const string& rhs);
    friend std::ostream& operator<<(std::ostream& os, const Node& obj);

    string name{};
    long distance = 0;
    shared_ptr<Node> parent;
};

bool operator==(const Node& lhs, const Node& rhs) { return lhs.name == rhs.name; };
bool operator==(const Node& lhs, const string& rhs) { return lhs.name == rhs; };
bool operator==(const shared_ptr<Node>& lhs, const string& rhs) { return lhs->name == rhs; };
std::ostream& operator<<(std::ostream& os, const Node& obj)
{
    string pname = (obj.parent.get() != nullptr) ? obj.parent->name : "nullptr";
    os << obj.name << " : dist = " << obj.distance << " and parent name is " << pname;
    return os;
}

long calculateDistance(Node* target)
{
    //cerr << *target << endl;
    long counter{};
    for (Node* next = target->parent.get(); next != nullptr; next = next->parent.get())
        ++counter;
    //cerr << counter << endl;
    target->distance = counter;
    return counter;
}

void processNodes(const string& searchName, shared_ptr<Node> parent,
                 map<string, vector<string>>& inputdata,
                 vector<shared_ptr<Node>>& nodes)
{
    //cerr << "Searching for " << searchName << endl;
    auto inputFound = find_if(inputdata.cbegin(), inputdata.cend(),
            [&searchName](const auto& el){ return el.first == searchName; });
    if (inputFound == inputdata.cend())
        return;
    auto children = inputFound->second;
    for_each(children.cbegin(), children.cend(),
            [&](const auto& name){
                auto anotherPerent = nodes.emplace_back(new Node{name, 0, parent});
                processNodes(name, anotherPerent, inputdata, nodes);
            });
    inputdata.erase(inputFound);
}

int main()
{
    int N;
    cin >> N;
    map<string, vector<string>> inputdata{};
    for(int input = 0; input != N; ++input)
    {
        string in;
        cin >> in;
        string key = in.substr(0, 3);
        string val = in.substr(4,3);
        auto result = inputdata.try_emplace(key, vector{val});
        if (result.second == false)
            result.first->second.push_back(val);
    }
    //cerr << "Input data: " << endl;
    //for_each(inputdata.cbegin(), inputdata.cend(), [](const auto& el) { cerr << el.first << ", ";});
    //cerr << endl;

    string rootName{"COM"};
    vector<shared_ptr<Node>> nodes;
    auto parent = nodes.emplace_back(new Node{rootName, 0, nullptr});
    processNodes(rootName, parent, inputdata, nodes);

    long result = 0;
    for_each(nodes.cbegin(), nodes.cend(), [&result](const auto& el) { result += calculateDistance(el.get()); });
    cout << "The total number of direct and indirect orbits is " << result << endl;

    //for_each(nodes.cbegin(), nodes.cend(), [](const auto& el) { cerr << *el << endl;});

    return 0;
}

