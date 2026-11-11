# Subnet Calculator (C++)

Small command-line tool that takes an IPv4 address with a CIDR prefix and prints
the netmask, wildcard, network address, broadcast address, host range and the
number of usable hosts.

Written as a networking exercise (background: electronics and telecommunications).

## Build

```bash
g++ -std=c++17 -Wall -Wextra -O2 -o subnet subnet.cpp
```

## Usage

```bash
./subnet 192.168.10.37/26
```

Example output:

```
Address   : 192.168.10.37/26
Netmask   : 255.255.255.192
Wildcard  : 0.0.0.63
Network   : 192.168.10.0
Broadcast : 192.168.10.63
Binary    : 11000000.10101000.00001010.00100101
First host: 192.168.10.1
Last host : 192.168.10.62
Hosts     : 62
```

## Notes

- Input is validated (octet range, prefix 0-32).
- /31 and /32 are handled as special cases.
- Only the C++ standard library is used.
