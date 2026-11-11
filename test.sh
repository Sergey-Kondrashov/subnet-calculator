#!/usr/bin/env bash
# Simple tests for the subnet calculator.
# Usage: bash test.sh     (on Windows run it in Git Bash)
set -u

g++ -std=c++17 -Wall -Wextra -O2 -o subnet subnet.cpp || { echo "Build failed"; exit 1; }

pass=0
fail=0

# expect_output "<expected line>" <args...>  -> program output must contain the line
expect_output() {
    local expected="$1"; shift
    local out
    out="$(./subnet "$@" 2>&1)"
    if grep -qF -- "$expected" <<< "$out"; then
        pass=$((pass + 1)); echo "PASS: ./subnet $* -> $expected"
    else
        fail=$((fail + 1)); echo "FAIL: ./subnet $* (expected: $expected)"
    fi
}

# expect_error <args...>  -> program must exit with a non-zero code
expect_error() {
    if ./subnet "$@" > /dev/null 2>&1; then
        fail=$((fail + 1)); echo "FAIL: ./subnet $* should have failed"
    else
        pass=$((pass + 1)); echo "PASS: ./subnet $* -> error as expected"
    fi
}

expect_output "Network   : 192.168.10.0"   192.168.10.37/26
expect_output "Broadcast : 192.168.10.63"  192.168.10.37/26
expect_output "Hosts     : 62"             192.168.10.37/26
expect_output "Broadcast : 10.0.0.255"     10.0.0.5/24
expect_output "Hosts     : 254"            10.0.0.5/24
expect_output "Network   : 172.16.5.8"     172.16.5.9/30
expect_output "Hosts     : 2"              172.16.5.9/30
expect_output "Netmask   : 255.255.255.192" 192.168.10.37 255.255.255.192
expect_output "Hosts     : 1"              8.8.8.8/32
expect_error 300.1.1.1/24
expect_error 1.2.3.4/33
expect_error 192.168.10.37 255.0.255.0

echo
echo "Passed: $pass, Failed: $fail"
[ "$fail" -eq 0 ]
