#include <iostream>
#include <set>
#include <vector>
#include <algorithm>
#include <sstream>
using namespace std;


bool sizeRequirements(std::string const& psswd)
{
    return psswd.size() == 6;
}

bool sameDigitsRequirements(std::string const& psswd)
{
    for(int i = 1; i != psswd.size(); ++i)
        if (psswd[i] == psswd[i - 1])
            return true;
    return false;
}

bool increasingDigitsRequirements(std::string const& psswd)
{
    for(int i = 1; i != psswd.size(); ++i)
        if (psswd[i - 1] > psswd[i])
            return false;
    return true;
}

bool passwordValidation(std::string const& psswd)
{
    // 6 digits;
    return //sizeRequirements(psswd)
        /*&&*/ sameDigitsRequirements(psswd)
        && increasingDigitsRequirements(psswd);
}

int main()
{
    int N;
    cin >> N;
    int M;
    cin >> M;

    const int max_pass = 999999;
    if (N > max_pass)
        N = max_pass;
    if (M > max_pass)
        M = max_pass;

    vector<string> sutiablePsswds;
    for(int i = N; i != M; ++i)
    {
        std::string input{to_string(i)};
        if (passwordValidation(input))
            sutiablePsswds.push_back(std::move(input));
    }

    cout << "All paswords are: " << endl;
    for_each(sutiablePsswds.cbegin(), sutiablePsswds.cend(),
             [](const auto& el){ cout << el << ", "; });
    cout << endl << "Answer is " << sutiablePsswds.size() << endl;
    return 0;
}

