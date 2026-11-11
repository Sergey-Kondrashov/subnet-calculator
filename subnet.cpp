// Subnet calculator: IPv4 address + CIDR prefix or netmask -> network details.
// Usage: ./subnet 192.168.10.37/26
//        ./subnet 192.168.10.37 255.255.255.192
#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct Ipv4Network {
    uint32_t address;
    int prefix;
};

static bool parseIpv4(const std::string& text, uint32_t& out) {
    std::stringstream ss(text);
    std::string part;
    uint32_t result = 0;
    int count = 0;
    while (std::getline(ss, part, '.')) {
        if (part.empty() || part.size() > 3) return false;
        for (char c : part)
            if (c < '0' || c > '9') return false;
        int value = std::stoi(part);
        if (value > 255) return false;
        result = (result << 8) | static_cast<uint32_t>(value);
        ++count;
    }
    if (count != 4) return false;
    out = result;
    return true;
}

static bool parseCidr(const std::string& text, Ipv4Network& net) {
    auto slash = text.find('/');
    if (slash == std::string::npos) return false;
    if (!parseIpv4(text.substr(0, slash), net.address)) return false;
    try {
        net.prefix = std::stoi(text.substr(slash + 1));
    } catch (...) {
        return false;
    }
    return net.prefix >= 0 && net.prefix <= 32;
}

// Converts a netmask such as 255.255.255.192 to a prefix length (26).
// A valid mask is a block of 1-bits followed by a block of 0-bits.
static bool maskToPrefix(uint32_t mask, int& prefix) {
    uint32_t inverted = ~mask;
    // Valid only if the inverted mask is 2^n - 1, i.e. inverted & (inverted + 1) == 0
    if ((inverted & (inverted + 1)) != 0) return false;
    prefix = 0;
    for (uint32_t m = mask; m != 0; m <<= 1) ++prefix;
    return true;
}

static std::string toString(uint32_t ip) {
    std::ostringstream os;
    os << ((ip >> 24) & 255) << '.' << ((ip >> 16) & 255) << '.'
       << ((ip >> 8) & 255) << '.' << (ip & 255);
    return os.str();
}

static std::string toBinary(uint32_t ip) {
    std::string s;
    for (int i = 31; i >= 0; --i) {
        s += ((ip >> i) & 1) ? '1' : '0';
        if (i % 8 == 0 && i != 0) s += '.';
    }
    return s;
}

int main(int argc, char** argv) {
    if (argc != 2 && argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <ip>/<prefix>        e.g. 192.168.10.37/26\n"
                  << "       " << argv[0] << " <ip> <netmask>      e.g. 192.168.10.37 255.255.255.192\n";
        return 1;
    }
    Ipv4Network net{};
    if (argc == 2) {
        if (!parseCidr(argv[1], net)) {
            std::cerr << "Invalid input: " << argv[1] << "\n";
            return 1;
        }
    } else {
        uint32_t maskValue = 0;
        if (!parseIpv4(argv[1], net.address)) {
            std::cerr << "Invalid IP address: " << argv[1] << "\n";
            return 1;
        }
        if (!parseIpv4(argv[2], maskValue)) {
            std::cerr << "Invalid netmask: " << argv[2] << "\n";
            return 1;
        }
        if (!maskToPrefix(maskValue, net.prefix)) {
            std::cerr << "Invalid netmask (1-bits must be contiguous): " << argv[2] << "\n";
            return 1;
        }
    }

    uint32_t mask = net.prefix == 0 ? 0 : (0xFFFFFFFFu << (32 - net.prefix));
    uint32_t network = net.address & mask;
    uint32_t broadcast = network | ~mask;
    uint64_t total = 1ULL << (32 - net.prefix);

    std::cout << "Address   : " << toString(net.address) << "/" << net.prefix << "\n";
    std::cout << "Netmask   : " << toString(mask) << "\n";
    std::cout << "Wildcard  : " << toString(~mask) << "\n";
    std::cout << "Network   : " << toString(network) << "\n";
    std::cout << "Broadcast : " << toString(broadcast) << "\n";
    std::cout << "Binary    : " << toBinary(net.address) << "\n";

    if (net.prefix >= 31) {
        // /31 (point-to-point, RFC 3021) and /32 (single host) have no classic broadcast
        std::cout << "Hosts     : " << total << " (no separate network/broadcast address)\n";
    } else {
        std::cout << "First host: " << toString(network + 1) << "\n";
        std::cout << "Last host : " << toString(broadcast - 1) << "\n";
        std::cout << "Hosts     : " << (total - 2) << "\n";
    }
    return 0;
}
