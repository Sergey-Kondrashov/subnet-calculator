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
./subnet 192.168.10.37/26                    # CIDR notation
./subnet 192.168.10.37 255.255.255.192       # address + netmask
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

## Features

- CIDR input (`192.168.10.37/26`) and netmask input (`192.168.10.37 255.255.255.192`)
- Netmask, wildcard, network, broadcast, first and last host, number of hosts
- Binary view of the address
- Input validation, including a check that a netmask is valid (contiguous 1-bits)
- Special cases for /31 and /32

## Tests

```bash
bash test.sh
```

The script builds the program and checks several known examples (/24, /26, /30, /32,
netmask input and invalid input). On Windows run it in Git Bash.

## Notes

- Input is validated (octet range, prefix 0-32).
- /31 and /32 are handled as special cases.
- Only the C++ standard library is used.

## Roadmap

- Split a network into smaller subnets
- Check whether an address belongs to a network
- IPv6 support
