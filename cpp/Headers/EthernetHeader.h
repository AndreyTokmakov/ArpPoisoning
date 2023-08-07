/**============================================================================
Name        : EthernetHeader.cpp
Created on  : 09.12.2022
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : EthernetHeader
============================================================================**/

#ifndef ETHERNETHEADER_H_
#define ETHERNETHEADER_H_

#include <cstdio>
#include <cstdlib>
#include <unistd.h>           // close()
#include <cstring>            // strcpy, memset(), and memcpy()
#include <netdb.h>            // struct addrinfo
#include <sys/types.h>        // needed for socket(), uint8_t, uint16_t
#include <sys/socket.h>       // needed for socket()
#include <netinet/in.h>       // IPPROTO_RAW, INET_ADDRSTRLEN
#include <netinet/ip.h>       // IP_MAXPACKET (which is 65535)
#include <arpa/inet.h>        // inet_pton() and inet_ntop()
#include <sys/ioctl.h>        // macro ioctl is defined
#include <bits/ioctls.h>      // defines values for argument "request" of ioctl.
#include <net/if.h>           // struct ifreq
#include <linux/if_ether.h>   // ETH_P_ARP = 0x0806
#include <linux/if_packet.h>  // struct sockaddr_ll (see man 7 packet)
#include <net/ethernet.h>
#include <cerrno>            // errno, perror()

#include <string>
#include <string_view>
#include <array>
#include <algorithm>

/** EthernetHeader class: **/
struct EthernetHeader final
{
    uint8_t destMac[6] {};
    uint8_t sourceMac[6] {};
    uint8_t type[2] {};

    inline void SetType(uint16_t packetType) noexcept {
        type[0] = packetType / 256;
        type[1] = packetType % 256;
    }

    [[nodiscard]]
    inline uint16_t GetType() const noexcept  {
        return (((unsigned short)(type[0])) * 256 + ((unsigned short)(type[1])));
    }

    bool CompareSourceMac(const std::string_view mac) {
        return CompareMacs(mac.data(), this->sourceMac);
    }

    bool CompareDestMac(const std::string_view mac){
        return CompareMacs(mac.data(), this->destMac);
    }

    bool CompareSourceMacFast(const uint8_t* mac){
        return (0 == memcmp(sourceMac, mac, 6));
    }

    bool CompareDestMacFast(const uint8_t* mac){
        return (0 == memcmp(destMac, mac, 6));
    }

    inline void SetSourceMACAddress(const uint8_t* const mac) {
        std::copy_n(mac, std::size(sourceMac), sourceMac);
    }

    inline void SetDestinationMACAddress(const uint8_t* const mac) {
        std::copy_n(mac, std::size(destMac), destMac);
    }

    bool SetSourceMACAddress(const std::string_view mac) {
        return SetMACAddress(mac.data(), sourceMac);
    }

    bool SetDestinationMACAddress(std::string_view mac)  {
        return SetMACAddress(mac, this->destMac);
    }

public:
    // TODO: Refactor
    static bool CompareMacs(std::string_view s, const uint8_t* d) {
        unsigned u[6], i;
        if (sscanf(s.data(), "%02x:%02x:%02x:%02x:%02x:%02x", u+0, u+1, u+2, u+3, u+4, u+5) != 6)
            return false;
        for (i = 0; i < 6; i++)
            if (d[i] != u[i])
                return false;
        return true;
    }

    // TODO: Refactor
    static bool SetMACAddress(std::string_view s, uint8_t * d)
    {
        unsigned u[6], i;
        if (sscanf(s.data(), "%02x:%02x:%02x:%02x:%02x:%02x", u+0, u+1, u+2, u+3, u+4, u+5) != 6)
            return false;
        for (i = 0; i < 6; i++) {
            if (u[i] > 255)
                return false;;
            d[i] = u[i];
        }
        return true;
    }

    void clear() noexcept
    {
        std::fill_n(destMac, std::size(destMac), 0);
        std::fill_n(sourceMac, std::size(sourceMac), 0);
        std::fill_n(type, std::size(type), 0);
    }

};


#endif /* ETHERNETHEADER_H_ */
