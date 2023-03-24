#include "utils.h"

unsigned int virtualAddressToVPN(unsigned int virtualAddress, unsigned int mask, unsigned int shift)
{
    unsigned int maskedAddress = virtualAddress & mask;  // apply the mask to the virtual address
    unsigned int vpn = maskedAddress >> shift;  // shift the masked virtual address to obtain the VPN
    return vpn;
}

unsigned int bitmask(int start, int end) {
    return ((1 << (end - start + 1)) - 1) << start;
}
