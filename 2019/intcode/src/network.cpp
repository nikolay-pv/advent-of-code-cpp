#include "network.hpp"

namespace net {

Computer::Computer(long addr, std::vector<long>& mem, Network* dm)
    : netAdress{std::move(addr)}, domain{dm}
{
    netInterfaceController = IntCodeComputer{"", mem};
    netInterfaceController.pausable = true;

    auto input = [&]() -> long
        {
            if (this->juststarted)
            {
                this->juststarted = false;
                this->netInterfaceController.interrupt = true;
                return this->netAdress;
            }
            if (this->state == State::Idle && this->packetsToProcess.empty())
            {
                this->netInterfaceController.interrupt = true;
                return -1;
            }
            long res{};
            if (this->state == State::Idle)
            {
                this->currentlyReading = this->packetsToProcess.front();
                this->packetsToProcess.pop();
                res = this->currentlyReading.first;
                this->state = State::xRead;
            } else {
                res = this->currentlyReading.second;
                this->state = State::Idle;
            }
            return res;
        };
    static_cast<Input*>(netInterfaceController.instructionSet[OpcodeInstruction::input].get())->setCallBack(input);

    auto output = [&](long output)
        {
            cache.push_back(output);
            if (this->cache.size() != 3)
                return;
            const long dest = cache.front();
            cache.pop_front();
            const long x = cache.front();
            cache.pop_front();
            const long y = cache.front();
            cache.pop_front();
            this->domain->sendPacket(dest, {x, y});
            this->netInterfaceController.interrupt = true;
        };
    static_cast<Output*>(netInterfaceController.instructionSet[OpcodeInstruction::output].get())->setCallBack(output);
}

void Computer::run()
{
    auto state = IntCodeComputer::Running;
    long output{0};
    netInterfaceController.interrupt = false;
    while (state == IntCodeComputer::Running && !emergencyHalt)
    {
        std::tie(output, state) = netInterfaceController.runningLoop(nullopt);
    }
}

Network::Network(long num, std::vector<long>& mem)
{
    for(long addr{0}; addr != num; ++addr)
    {
        theNet.push_back(make_shared<Computer>(addr, mem, this));
    }
}

void Network::halt() const
{
    for(const auto& c : theNet)
        c->emergencyHalt = true;
    halting = true;
}

void Network::run() const
{
    while (!halting)
    {
        for(long i = 0; i != theNet.size(); ++i)
        {
            cout << "Running on computer " << i << "\r";
            cout.flush();
            theNet[i]->run();
        }
    }
}

void Network::sendPacket(long dst, packet pckt) const
{
    if (dst == 255)
    {
        cout << "Part1: Recieved packet to adress 255, the Y is " << pckt.second << endl << "Halting." << endl;
        halt();
        return;
    }
    assert(dst >= 0 && dst < 50 && "There must be 50 computers in the network so far.");

    theNet[dst]->packetsToProcess.push(pckt);
}

} // namespace net
