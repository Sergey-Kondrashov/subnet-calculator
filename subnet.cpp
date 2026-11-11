// Subnet calculator: IPv4 address + CIDR prefix -> network details.
// Usage: ./subnet 192.168.10.37/26
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
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <ip>/<prefix>   e.g. 192.168.10.37/26\n";
        return 1;
    }
    Ipv4Network net{};
    if (!parseCidr(argv[1], net)) {
        std::cerr << "Invalid input: " << argv[1] << "\n";
        return 1;
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
