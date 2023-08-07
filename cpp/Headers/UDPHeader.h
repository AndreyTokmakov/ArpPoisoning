//============================================================================
// Name        : UDPHeader.h
// Created on  : May 19, 2015
// Author      : Tokmakov Andrey
// Version     :
// Copyright   : Your copyright notice
// Description :
//============================================================================

#ifndef UDP_HEADER__H_
#define UDP_HEADER__H_

#include <arpa/inet.h>

/** UDPPseudoHeader class declaration: **/
struct UDPPseudoHeader final {
	uint32_t sourceAddress {};
    uint32_t destinationAddress {};
    uint8_t  useless {};
    uint8_t  protocol {};
    uint16_t totalLength {};
};

/** UDPHeader class declaration: **/
class UDPHeader final
{
private:
	/** Source port: **/
	/** The 16-bit port number of the process that originated the UDP message on the source device.         **/
	/** This will normally be an ephemeral (client) port number for a request sent by a client to a server, **/ 
	/** or a well-known/registered (server) port number for a reply sent by a server to a client            **/
    uint16_t sourcePort;

	/** Destination port: **/
	/** The 16-bit port number of the process that is the ultimate intended recipient of the message on the destination device.  **/
	/** This will usually be a well-known/registered (server) port number for a client request, or an ephemeral (client) port    **/
	/** number for a server reply                                                                                                **/
    uint16_t destinationPort;

	/** Length:  **/
	/** The length of the entire UDP datagram, including both header and Data fields.  **/
    uint16_t length;

	/** Checksum: **/
	/** An optional 16-bit checksum computed over the entire UDP datagram plus a special pseudo header of fields.
	 * See below for more information.**/
    uint16_t checksum;

	/** Data: The encapsulated higher-layer message to be sent.**/
	//char data[MTU]; 

public:

    [[nodiscard]]
    inline uint16_t GetSourcePort() const noexcept {
        return htons(sourcePort);
    }

    inline void SetSourcePort(uint16_t port) {
        sourcePort = htons(port);
    }

    [[nodiscard]]
    inline uint16_t GetDestinationPort() const noexcept {
        return htons(destinationPort);
    }

    inline void SetDestinationPort(uint16_t port) {
        destinationPort = htons(port);
    }

    [[nodiscard]]
    inline uint16_t GetPacketLength() const noexcept {
        return ntohs(length);
    }

    inline void SetPacketLength(uint16_t len) {
        length = htons(len);
    }

    [[nodiscard]]
    inline uint16_t GetChecksum() const noexcept {
        return htons(checksum);
    }

    void SetChecksum(uint16_t checkSum) {
        checksum = checkSum;
    }
};

#endif // !(UDP_HEADER__H_)