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
struct Element
{
    string name;
    mutable long quantity;
    bool visited = false;

    vector<GraphEdge*> parentsEdges{};
    vector<GraphEdge*> childrenEdges{};

    friend bool operator >(const Element& l, const Element& r)
    { return l.name > r.name; }
    friend bool operator <(const Element& l, const Element& r)
    { return l.name < r.name; }
    friend bool operator==(const Element& l, const Element& r)
    { return l.name == r.name; }
    friend bool operator!=(const Element& l, const Element& r)
    { return !(l == r); }
};

struct GraphEdge
{
    static GraphEdge create(Element const& parent, Element const& child, long weight)
    {
        return GraphEdge{false, parent, child, weight};
    }
    bool visited = false;
    Element const& parent;
    Element const& child;
    // requred quntity of child to produce the quantity of the parent
    long requiredQuantity = {-1};

    friend bool operator >(const GraphEdge& l, const GraphEdge& r)
    { return l.parent.name > r.parent.name && l.child.name > r.child.name; }
    friend bool operator <(const GraphEdge& l, const GraphEdge& r)
    { return !(l.parent.name > r.parent.name); }
    friend bool operator==(const GraphEdge& l, const GraphEdge& r)
    { return l.parent.name == r.parent.name && l.child.name == r.child.name; }
    friend bool operator!=(const GraphEdge& l, const GraphEdge& r)
    { return !(l == r); }
};

std::istream& operator>>(std::istream& is, Element& obj)
{
    is >> obj.quantity;
    is >> obj.name;
    return is;
}

std::ostream& operator<<(std::ostream& os, const Element& obj)
{
    os << obj.quantity << " " << obj.name;
    return os;
}

std::ostream& operator<<(std::ostream& os, const GraphEdge& obj)
{
    os << "(p -> c): ";
    os << obj.parent.quantity << " " << obj.parent.name;
    os << " -" << obj.requiredQuantity << "-> ";
    os << obj.child.quantity << " " << obj.child.name;
    return os;
}

struct SystemOfEquations
{
    std::set<Element> elements{};
    std::set<GraphEdge> edges{};

    SystemOfEquations(std::vector<string>&& inputs)
    {
        map<Element, string> tmp_map{};
        for_each(inputs.begin(), inputs.end(),
                [&](std::string& input){
                    auto oel = addResultElement(input);
                    tmp_map.insert({std::move(oel), std::move(input)});
                });
        elements.insert({"ORE", 1, false, {}, {}});
        for_each(tmp_map.begin(), tmp_map.end(),
                [&](auto& pairs){
                    addGraphConnections(pairs.first, std::move(pairs.second));
                });
    }

    Element addResultElement(std::string const& input)
    {
        auto start = find(input.crbegin(), input.crend(), '>');
        auto result = input.substr(std::distance(start, input.crend()) + 1);
        stringstream is{result};
        Element res;
        is >> res;
        elements.insert(res);
        return std::move(res);
    }

    void addGraphConnections(Element const& out, std::string&& input)
    {
        auto res = elements.find(out);
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
            Element component;
            is >> component;
            if (component.name.back() == ',')
                component.name.pop_back();
            auto stored_comp = elements.find(component);
            assert(stored_comp != elements.end());
            auto e = edges.insert(GraphEdge::create(*res, *stored_comp, component.quantity));
            //cerr << "Added edge " << *e.first << endl;
            const_cast<Element&>(*res).childrenEdges.push_back(const_cast<GraphEdge*>(&(*e.first)));
            const_cast<Element&>(*stored_comp).parentsEdges.push_back(const_cast<GraphEdge*>(&(*e.first)));
        }
    }

    void simplifyEdges(Element& node)
    {
        if (node.visited)
            return;
        // sum all parent branches weights if visited
        long totalQuantity{};
        for(auto& edge : node.parentsEdges)
        {
            // too early to make calculations
            if (!edge->visited)
                return;
            totalQuantity += edge->requiredQuantity;
        }
        // find multiplier and increase weight of children
        // and mark them visited
        long multiplier = ceil(totalQuantity / double(node.quantity));
        if (multiplier == 0)
            multiplier = 1;
        for(auto& edge : node.childrenEdges)
        {
            edge->requiredQuantity *= multiplier;
            edge->visited = true;
        }
        // push recursion down the edge
        node.visited = true;
        for_each(node.childrenEdges.begin(), node.childrenEdges.end(),
                [&](auto& edge){
                    assert(edge->parent == node);
                    simplifyEdges(const_cast<Element&>(edge->child));
                });
    }

    long solve()
    {
        auto fuel = find_if(elements.begin(), elements.end(), [](auto& el){ return el.name == "FUEL"; });
        assert(fuel != elements.end());
        // no changes in names
        simplifyEdges(const_cast<Element&>(*fuel));
        auto ore = find_if(elements.begin(), elements.end(), [](auto& el){ return el.name == "ORE"; });
        assert(ore != elements.end());
        return std::accumulate(ore->parentsEdges.cbegin(), ore->parentsEdges.cend(), 0,
                [](long a, auto& b){
                    return a + b->requiredQuantity;
                });
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
    for(long line = 0; line != N; ++line)
    {
        std::string input;
        std::getline(cin, input);
        inputs.push_back(std::move(input));
    }
    auto soe = SystemOfEquations(std::move(inputs));
    const long answer{soe.solve()};
    // part 1
    cout << "Part 1\n";
    cout << "We need " << answer << " ORE.\n";
    // part 2
    cout << "Part 2\n";
    return 0;
}

