#pragma once
#include <utility>
#include <set>
#include <queue>
#include <list>

#include "intcode.hpp"

namespace net {

// X Y
using packet = std::pair<long, long>;

struct Network;

struct Computer
{
    enum State : int
    {
        Idle = 0,
        xRead = 1
    } state{0};

    Computer(long addr, std::vector<long>& mem, Network* dm);

    IntCodeComputer netInterfaceController;
    queue<packet> packetsToProcess;
    list<long> cache{};
    packet currentlyReading{};
    Network* domain;
    long netAdress{-1};
    bool emergencyHalt{false};
    bool juststarted{true};

    void run();

    friend bool operator >(const Computer& l, const Computer& r)
    { return l.netAdress > r.netAdress; }
    friend bool operator <(const Computer& l, const Computer& r)
    { return l.netAdress < r.netAdress; }
    friend bool operator==(const Computer& l, const Computer& r)
    { return l.netAdress == r.netAdress; }
    friend bool operator!=(const Computer& l, const Computer& r)
    { return l.netAdress != r.netAdress; }

};

using ComputerPtr = shared_ptr<Computer>;

struct Network
{
public:
    Network() = delete;
    Network(Network&&) = default;
    Network(const Network&) = default;
    ~Network() = default;

    Network(long num, std::vector<long>& mem);

    void sendPacket(long dst, packet pckt) const;
    void halt() const;
    void run() const;

public:
    vector<ComputerPtr> theNet{};
    mutable bool halting{false};
};

} // namespace net
