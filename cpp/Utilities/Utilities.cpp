/**============================================================================
Name        : Utilities.cpp
Created on  : 09.12.2022
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Utilities
============================================================================**/

#include "Utilities.h"

#include <iostream>
#include <unistd.h>
#include <cstring>
#include <array>
#include <charconv>

#include <netinet/in.h>
#include <sys/ioctl.h>
#include <bits/ioctls.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netdb.h>

void Utilities::PrintMACAddress(const uint8_t* mac)
{
    for (int i = 0; i < 5; i++)
        printf ("%02x:", mac[i]);
    printf ("%02x", mac[5]);
}

void Utilities::checkRunningUnderRoot()
{
    const uint32_t userID { getuid() };
    if (0 != userID)
    {
        // throw std::runtime_error("Application require the ROOT user access"  );
        std::cerr << "Application require the ROOT user access" << std::endl;
        std::exit(0);
    }
}


[[nodiscard("nodiscard")]]
std::string Utilities::IpToStr(unsigned long address) {
    return inet_ntoa({ static_cast<in_addr_t>(htonl(address)) });
}


[[nodiscard("Don't forget to use the return value somehow.")]]
std::string HostToIp(std::string_view host) noexcept
{
    const hostent* hostname { gethostbyname(host.data()) };
    if (hostname)
        return std::string { inet_ntoa(**(in_addr**)hostname->h_addr_list) };
    return std::string {};
}


[[nodiscard("Call may be expensive")]]
uint16_t Utilities::Checksum(uint16_t *ptr, uint16_t bytes) noexcept
{
    long sum = 0; // FIXME
    uint16_t oddByte {0};

    while (bytes > 1) {
        sum += *ptr++;
        bytes -= 2;
    }
    if (bytes == 1) {
        oddByte = 0;
        *((u_char *) & oddByte) = *(u_char *) ptr;
        sum += oddByte;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);

    // FIXME
    uint16_t answer = ~sum;

    return answer;
}


[[nodiscard]]
sockaddr_ll Utilities::ResolveInterfaceAddress(std::string_view interfaceName)
{
    sockaddr_ll device {};

    /** Submit request for a socket descriptor to look up interface. **/
    Utilities::SocketScoped socketHandle = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (socketHandle < 0) {
        std::cout << "Failed to create socket" << std::endl;
        throw std::runtime_error("Ohh shit!");
    }

    /** Interface request structure used for socket ioctl: **/
    ifreq ifr { {""},{} };
    snprintf (ifr.ifr_name, sizeof (ifr.ifr_name), "%s", interfaceName.data()); // FIXME

    if (ioctl (socketHandle, SIOCGIFHWADDR, &ifr) < 0) {
        std::cout << "Failed to get source MAC address" << std::endl;
        throw std::runtime_error("Ohh shit!");
    }

    /** **/
    memset(&device, 0, sizeof(device));
    memcpy(device.sll_addr, ifr.ifr_hwaddr.sa_data, 6 * sizeof (uint8_t)); // Copy source MAC address.

    /** Find interface index from interface name and store index in struct sockaddr_ll device, which will be used as an argument of sendto(): **/
    if ((device.sll_ifindex = if_nametoindex(ifr.ifr_name)) == 0) {
        std::cout << "Failed to obtain interface index" << std::endl;
        throw std::runtime_error("Ohh shit!");
    }
    /** Fill out sockaddr_ll: **/
    device.sll_family = AF_PACKET;
    device.sll_halen = 6;

    return device;
}

bool Utilities::ResolveInterfaceAddress(std::string_view interfaceName,
                                        sockaddr_ll& device) {
    /** Submit request for a socket descriptor to look up interface. **/
    Utilities::SocketScoped socketHandle = ::socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (socketHandle < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return false;
    }

    /** Interface request structure used for socket ioctl: **/
    ifreq ifr { {""},{} };
    snprintf (ifr.ifr_name, sizeof (ifr.ifr_name), "%s", interfaceName.data()); // FIXME

    if (ioctl (socketHandle, SIOCGIFHWADDR, &ifr) < 0) {
        std::cerr << "Failed to get source MAC address" << std::endl;
        //::close(socketHandle);
        return false;
    }

    /** ZeroMemory + copy source MAC address. **/
    memset(&device, 0, sizeof(device));
    memcpy(device.sll_addr, ifr.ifr_hwaddr.sa_data, 6 * sizeof (uint8_t));

    /** Find interface index from interface name and store index in struct
     * sockaddr_ll device, which will be used as an argument of sendto(): **/
    if ((device.sll_ifindex = if_nametoindex(ifr.ifr_name)) == 0) {
        std::cerr << "Failed to obtain interface index" << std::endl;
        return false;
    }

    /** Fill out sockaddr_ll: **/
    device.sll_family = AF_PACKET;
    device.sll_halen = 6;

    return true;
}

void Utilities::SocketScoped::closeSocket(int s)
{
    if (INVALID_HANDLE != s && SOCKET_ERROR == ::close(s)) {
        std::cerr << "close() function failed with error: " << errno << std::endl;
    }
}

void setBroadcast(Utilities::SocketScoped& sock,
                  bool enabled)
{
    uint32_t broadcastEnable { static_cast<uint32_t>(enabled)};
    int32_t ret = setsockopt(sock, SOL_SOCKET, SO_BROADCAST,
                             &broadcastEnable, sizeof(broadcastEnable));
    if (-1 == ret) {
        std::cerr << "Failed to enable broadcast on socket (" << sock << ", Error = " << errno << std::endl;
        std::exit(0);
    }
}

namespace Utilities::IP
{

    [[nodiscard]]
    std::string ipInt2Str(uint32_t ip)
    {
        std::string ipStr { "000.000.000.000"};
        const uint32_t len = snprintf(ipStr.data(), ipStr.capacity(), "%d.%d.%d.%d",
                                      ip / 16777216 % 256, ip / 65536 % 256, ip / 256 % 256, ip % 256);
        ipStr.resize(len);
        ipStr.shrink_to_fit();
        return ipStr;
    }

    [[nodiscard]]
    std::string ipInt2StrOLD(uint32_t ip)
    {
        std::array<unsigned char, 4> bytes {};
        for (size_t i = 0; i < bytes.size(); ++i) {
            bytes[i] = (ip >> i*8) & 0xFF;
        }
        std::string ipStr(16, '\0');
        int n = std::sprintf(ipStr.data(), "%d.%d.%d.%d", bytes[3], bytes[2], bytes[1], bytes[0]);
        ipStr.resize(n);
        return ipStr;
    }


    uint32_t ipInt2Str(std::string_view ip)
    {
        size_t pos = 0, prev = 0, id = 0;
        std::array<uint16_t, 4> octets {};
        while ((pos = ip.find('.', prev)) != std::string::npos) {
            std::from_chars(ip.data() + prev, ip.data() + pos, octets[id++]);
            prev = pos + 1;
        }

        std::from_chars(ip.data() + prev, ip.data() + ip.length() - prev, octets[id++]);
        return (octets[0] << 24) | (octets[1] << 16) | (octets[2] << 8) | (octets[3]);
    }

    uint32_t ipInt2Str2(std::string_view ip)
    {
        size_t pos = 0, prev = 0, result = 0;
        uint8_t octet =0, bit = 32;
        while ((pos = ip.find('.', prev)) != std::string::npos) {
            std::from_chars(ip.data() + prev, ip.data() + pos, octet);
            prev = pos + 1;
            result |= octet << (bit-= 8);
        }

        std::from_chars(ip.data() + prev, ip.data() + ip.length() - prev, octet);
        return result |= octet;
    }

}
