#include <iostream>
#include <vector>
//#include <array>
//#include <valarray>
#include <map>
#include <set>
//#include <list>
//#include <memory>
#include <cmath>
#include <numeric>
#include <sstream>

using namespace std;

struct GraphEdge;
using GraphEdgePtr = shared_ptr<GraphEdge>;
struct Element
{
    bool visited{false};
    string name;
    long long quantity;
    vector<GraphEdgePtr> parentsEdges{};
    vector<GraphEdgePtr> childrenEdges{};

    Element(std::string const& nn, long long qq) : name{nn}, quantity{qq} { }
    static shared_ptr<Element> createObject(std::string const& name, long long quantity)
    { return make_shared<Element>(name, quantity); }

    void rollBack() { visited = false; }

    friend bool operator >(const Element& l, const Element& r)
    { return l.name > r.name; }
    friend bool operator <(const Element& l, const Element& r)
    { return l.name < r.name; }
    friend bool operator==(const Element& l, const Element& r)
    { return l.name == r.name; }
    friend bool operator!=(const Element& l, const Element& r)
    { return !(l == r); }
};
using ElementPtr = shared_ptr<Element>;

std::istream& operator>>(std::istream& is, Element& obj)
{
    is >> obj.quantity >> obj.name;
    return is;
}
std::ostream& operator<<(std::ostream& os, const Element& obj)
{
    os << obj.quantity << " " << obj.name;
    return os;
}

struct GraphEdge
{
    struct InitialState
    {
        long long requiredQuantity{-1};
    } istate;
    bool visited = false;
    ElementPtr parent;
    ElementPtr child;
    // requred quntity of child to produce the quantity of the parent
    long long requiredQuantity{-1};

    GraphEdge(bool vis, ElementPtr pp, ElementPtr cc, long long weight)
        : istate{weight}, visited{vis}, parent{pp}, child{cc}, requiredQuantity{weight}
    { }

    static GraphEdgePtr createObject(ElementPtr parent, ElementPtr child, long long weight)
    { return std::make_shared<GraphEdge>(false, parent, child, weight); }
    static GraphEdgePtr createObject(bool vis, ElementPtr parent, long long weight)
    { return std::make_shared<GraphEdge>(vis, parent, parent, weight); }

    void rollBack() { visited = false; requiredQuantity = istate.requiredQuantity; }

    friend bool operator >(const GraphEdge& l, const GraphEdge& r)
    { return l.parent->name > r.parent->name && l.child->name > r.child->name; }
    friend bool operator <(const GraphEdge& l, const GraphEdge& r)
    { return !(l.parent->name > r.parent->name); }
    friend bool operator==(const GraphEdge& l, const GraphEdge& r)
    { return l.parent->name == r.parent->name && l.child->name == r.child->name; }
    friend bool operator!=(const GraphEdge& l, const GraphEdge& r)
    { return !(l == r); }
};

std::ostream& operator<<(std::ostream& os, const GraphEdge& obj)
{
    os << "(p -> c): ";
    os << obj.parent->quantity << " " << obj.parent->name;
    os << " -" << obj.requiredQuantity << "-> ";
    os << obj.child->quantity << " " << obj.child->name;
    return os;
}

struct SystemOfEquations
{
    ElementPtr fuel{};
    ElementPtr ore{};
    std::vector<ElementPtr> elements{};
    std::vector<GraphEdgePtr> edges{};

    SystemOfEquations(std::vector<string>&& inputs)
    {
        map<ElementPtr, string> tmp_map{};
        for_each(inputs.begin(), inputs.end(),
                [&](std::string& input){
                    auto oel = addResultElement(input);
                    if (oel->name == "FUEL")
                        fuel = oel;
                    tmp_map.insert({std::move(oel), std::move(input)});
                });
        ore = Element::createObject("ORE", 1);
        elements.push_back(ore);
        for_each(tmp_map.begin(), tmp_map.end(),
                [&](auto& pairs){
                    addGraphConnections(pairs.first, std::move(pairs.second));
                });
    }

    ElementPtr addResultElement(std::string const& input)
    {
        auto start = find(input.crbegin(), input.crend(), '>');
        auto result = input.substr(std::distance(start, input.crend()) + 1);
        stringstream is{result};
        long long quantity;
        string name;
        is >> quantity >> name;
        auto el = Element::createObject(name, quantity);
        elements.push_back(el);
        return el;
    }

    void addGraphConnections(ElementPtr const& out, std::string&& input)
    {
        auto res = find(elements.begin(), elements.end(), out);
        if (res == elements.end())
        {
            cerr << "Boom!" << endl;
            return;
        }

        auto start = find(input.crbegin(), input.crend(), '>');
        auto firstpart = input.substr(0, input.size() - std::distance(input.crbegin(), start) - 3);
        istringstream is{firstpart};
        while(is.rdstate() == std::ios_base::goodbit)
        {
            long long quantity;
            string name;
            is >> quantity >> name;
            if (name.back() == ',')
                name.pop_back();
            auto stored_comp = find_if(elements.begin(), elements.end(), [&](auto& el){ return el->name == name; });
            assert(stored_comp != elements.end());
            auto ne = GraphEdge::createObject(*res, *stored_comp, quantity);
            edges.push_back(ne);
            //cerr << "Added edge " << *ne << endl;
            (*res)->childrenEdges.push_back(ne);
            (*stored_comp)->parentsEdges.push_back(ne);
        }
    }

    void simplifyEdges(ElementPtr const& node)
    {
        if (node->visited)
            return;
        // sum all parent branches weights if visited
        long long totalQuantity{};
        for(auto& edge : node->parentsEdges)
        {
            // too early to make calculations
            if (!edge->visited)
                return;
            totalQuantity += edge->requiredQuantity;
        }
        // find multiplier and increase weight of children
        // and mark them visited
        long long multiplier = ceil(totalQuantity / double(node->quantity));
        if (multiplier == 0)
            multiplier = 1;
        //cerr << "mult " << multiplier << " " << totalQuantity << " / " << node->quantity << endl;
        //cerr << "Node: " << *node << endl;
        for(auto& edge : node->childrenEdges)
        {
            //cerr << "\t " << *edge << endl;
            edge->requiredQuantity *= multiplier;
            edge->visited = true;
            //cerr << "\t " << *edge << endl;
        }
        // push recursion down the edge
        node->visited = true;
        for_each(node->childrenEdges.begin(), node->childrenEdges.end(),
                [&](auto& edge){
                    assert(edge->parent == node);
                    simplifyEdges(edge->child);
                });
    }

    void resetEquations()
    {
        for_each(elements.begin(), elements.end(), [](auto& el){ el->rollBack(); });
        for_each(edges.begin(), edges.end(), [](auto& el){ el->rollBack(); });
    }

    long long solve(long long withObjective)
    {
        auto objective  = GraphEdge::createObject(true, fuel, withObjective);
        fuel->parentsEdges.push_back(objective);
        // no changes in names
        simplifyEdges(fuel);
        fuel->parentsEdges.clear();
        long long answ = 0;
        for_each(ore->parentsEdges.cbegin(), ore->parentsEdges.cend(),
                [&](auto& edge){ answ += edge->requiredQuantity; });
        resetEquations();
        return answ;
    }
};

////////////////////////////////////////////////////////////////////////////////
// main
int main()
{
    int N;
    cin >> N;
    string garbage;
    std::getline(cin, garbage);

    vector<string> inputs{};
    for(long long line = 0; line != N; ++line)
    {
        std::string input;
        std::getline(cin, input);
        inputs.push_back(std::move(input));
    }
    auto soe = SystemOfEquations(std::move(inputs));
    SystemOfEquations busoe = soe;
    // part 1
    cout << "Part 1\n";
    long long answer{soe.solve(1)};
    cout << "We need " << answer << " ORE.\n";
    // part 2
    cout << "Part 2\n";
    const long long availableOre = 1000000000000;
    long long lowerfuel{availableOre / answer};
    long long fuel{lowerfuel};
    long long upperfuel = 1.2 * lowerfuel;
    long long differnece{-1};
    while (differnece != 0)
    {
        //cerr << "L U" << lowerfuel << " " << upperfuel << endl;
        answer = soe.solve(fuel);
        if (answer == availableOre)
            break;
        else if (answer < availableOre)
        {
            lowerfuel = fuel;
            differnece = (upperfuel - lowerfuel)/2;
            fuel += differnece;
        }
        else if (answer > availableOre)
        {
            upperfuel = fuel;
            differnece = round((upperfuel - lowerfuel)/2.0);
            fuel -= differnece;
        }
    }

    cout << "The max amout of fuel is " << fuel << " (" << answer << ")\n";
    return 0;
}

