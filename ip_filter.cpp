#include <cassert>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <optional>

// ("",  '.') -> [""]
// ("11", '.') -> ["11"]
// ("..", '.') -> ["", "", ""]
// ("11.", '.') -> ["11", ""]
// (".11", '.') -> ["", "11"]
// ("11.22", '.') -> ["11", "22"]
std::vector<std::string> split(const std::string &str, char d)
{
    std::vector<std::string> r;

    std::string::size_type start = 0;
    std::string::size_type stop = str.find_first_of(d);
    while(stop != std::string::npos)
    {
        r.push_back(str.substr(start, stop - start));

        start = stop + 1;
        stop = str.find_first_of(d, start);
    }

    r.push_back(str.substr(start));

    return r;
}

bool reverseOrderComparer(const std::vector<std::string> &a, const std::vector<std::string> &b) {

    if(a.size() != b.size()) {
        return a.size() > b.size();
    }

    for(int i = 0; i < a.size(); i++) {
        if(a[i] != b[i]) {
            return std::stoi(a[i]) > std::stoi(b[i]);
        }
    }

    return false;
}

void printIps(const std::vector<std::vector<std::string>> &ip_pool) {

    for(std::vector<std::vector<std::string> >::const_iterator ip = ip_pool.cbegin(); ip != ip_pool.cend(); ++ip)
    {
        for(std::vector<std::string>::const_iterator ip_part = ip->cbegin(); ip_part != ip->cend(); ++ip_part)
        {
            if (ip_part != ip->cbegin())
            {
                std::cout << ".";

            }
            std::cout << *ip_part;
        }
        std::cout << std::endl;
    }
}

bool match(const std::vector<std::string> &ip, const std::vector<std::string> &mask) {
    for(int i = 0; i < mask.size(); i++) {
        if(!mask[i].empty() && mask[i] != ip[i]) {
            return false;
        }
    }

    return true;
}

bool matchAny(const std::vector<std::string> &ip, const std::vector<std::string> &mask) {
    for(int i = 0; i < mask.size(); i++) {
        if(!mask[i].empty() && mask[i] == ip[i]) {
            return true;
        }
    }

    return false;
}

/*
   We can optimize this significantly, given that we are sorted.
   But problem size is small and filter function would not be working in general case
*/
std::vector<std::vector<std::string>> filter(const std::vector<std::vector<std::string>> &ip_pool,
                                             const std::vector<std::string> &mask,
                                             const bool any = false) {

    std::vector<std::vector<std::string>> result;
    std::copy_if(ip_pool.cbegin(),
                 ip_pool.cend(),
                 std::back_inserter(result),
                 [mask, any](const std::vector<std::string> &ip) {

        if(any) {
            return matchAny(ip, mask);
        } else {
            return match(ip, mask);
        }
    }
);

    return result;
}

int main(int argc, char const *argv[])
{
    try
    {
        std::vector<std::vector<std::string>> ip_pool;

        for(std::string line; std::getline(std::cin, line);)
        {
            std::vector<std::string> v = split(line, '\t');
            ip_pool.push_back(split(v.at(0), '.'));
        }

        std::sort(ip_pool.begin(), ip_pool.end(), reverseOrderComparer);
        printIps(ip_pool);

        auto filtered = filter(ip_pool, {"1"});
        printIps(filtered);

        filtered = filter(ip_pool, {"46", "70"});
        printIps(filtered);

        filtered = filter(ip_pool, {"46", "46", "46", "46"}, true);
        printIps(filtered);
    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
