/**============================================================================
Name        : ICMPHeader.h
Created on  : 12.12.2022
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : ICMPHeader
============================================================================**/

#ifndef CPPPROJECTS_ICMPHEADER_H
#define CPPPROJECTS_ICMPHEADER_H

#include <arpa/inet.h>

enum ICMPType {
    ICMP_EchoReply = 0,
    ICMP_DestinationUnreachable = 3,
    ICMP_EchoRequest = 8,
    ICMP_RouterAdvertisement = 9,
    ICMP_RouterSolicitation = 10,
    ICMP_TimestampRequest = 13,
    ICMP_TimestampReply = 14,
    ICMP_InformationRequest = 15
};

/** ICMPHeader class declaration: **/
struct ICMPHeader final
{
    /** Type: Identifies the ICMP message type.                                                               **/
    /** For ICMPv6, values from 0 to 127 are error messages and values 128 to 255 are informational messages. **/
    uint8_t type {0};

    /** Code: Identifies the “subtype” of message within each ICMP message Type value. **/
    /** Thus, up to 256 “subtypes” can be defined for each message type.               **/
    uint8_t code {0};

    /** Checksum: **/
    /** It provides error detection coverage for the entire ICMP message.  **/
    uint16_t checksum {0};

    /** Identifier: **/
    uint16_t identifier {0};

    /** Sequence: **/
    uint16_t sequence {0};

public:

    [[nodiscard]]
    inline uint16_t GetType() const {
        return type;
    }

    inline void SetType(uint16_t type) {
        this->type = type;
    }

    [[nodiscard]]
    inline uint16_t GetCode() const {
        return code;
    }

    inline void SetCode(uint16_t code) {
        this->code = code;
    }

    [[nodiscard]]
    inline uint16_t GetChecksum() const noexcept {
        return htons(checksum);
    }

    inline void SetChecksum(uint16_t checkSum) {
        checksum = checkSum;
    }

    [[nodiscard]]
    inline uint16_t GetIdentifier() const noexcept {
        return htons(identifier);
    }

    inline void SetIdentifier(uint16_t id) {
        identifier = htons(id);
    }

    [[nodiscard]]
    inline uint16_t GetSequence() const noexcept{
        return htons(sequence);
    }

    inline void SetSequence(uint16_t seq) {
        sequence = htons(seq);
    }
};

#endif //CPPPROJECTS_ICMPHEADER_H
