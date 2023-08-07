/**============================================================================
Name        : TCPHeader.h
Created on  : 09.12.2022
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : TCPHeader
============================================================================**/

#ifndef TCPHEADER_H_
#define TCPHEADER_H_

#include "IPv4Header.h"
#include "../Utilities/Utilities.h"
#include <cstring>

/** PseudoTCPHeader class declaration: **/
struct PseudoTCPHeader final
{
    uint32_t sourceAddress;
    uint32_t destinationAddress;
    uint8_t  useless;
    uint8_t  protocol;
    uint16_t totalLength;
} __attribute__((packed, aligned(1)));

// TODO: Set alignment
struct TCPHeader final
{   /** Source port: **/
    uint16_t sourcePort { 0 };

    /** Destination port: **/
    uint16_t destinationPort { 0 };

    /** Sequence number: **/
    uint32_t sequence { 0 };

    /** Acknowledgement number: **/
    uint32_t acknowledge { 0 };

    /** : **/
    uint8_t reserved_part1:4{};

    /** This indicates where the data begins: **/
    uint8_t dataOffset:4 {};

    /** Unsigned char  flags: **/
    uint8_t FYN :1 {};

    /** : **/
    uint8_t SYN :1 {};

    /** Reset Flag: **/
    uint8_t RST :1 {};

    /** Push Flag: **/
    uint8_t PSH :1 {};

    /** Acknowledgement Flag: **/
    uint8_t ACK :1 {};

    /** Urgent Flag: **/
    uint8_t URG :1 {};

    /** : **/
    uint8_t reserved2:2 {};

    /** Window: **/
    uint16_t window { 0 };

    /** Checksum: **/
    uint16_t checksum { 0 };

    /** Urgent pointer: **/
    uint16_t urgent_pointer { 0 };

    [[nodiscard]] inline uint16_t GetDataOffset() const noexcept {
        return dataOffset * 4;
    }

    void SetDataOffset(uint16_t offset) noexcept {
        dataOffset = offset / 4;
    }

    [[nodiscard]]
    inline uint16_t GetSourcePort() const noexcept {
        return htons(sourcePort);
    }

    void SetSourcePort(uint16_t port) noexcept {
        sourcePort = htons(port);
    }

    [[nodiscard]]
    inline uint16_t GetDestinationPort() const noexcept {
        return htons(destinationPort);
    }

    void SetDestinationPort(uint16_t port) noexcept {
        destinationPort = htons(port);
    }

    [[nodiscard]]
    inline uint16_t GetWindow() const noexcept {
        return htons(window);
    }

    void SetWindow(uint16_t wnd) noexcept {
        window = htons(wnd);
    }

    [[nodiscard]]
    inline uint32_t GetAcknowledgeSeqNum() const noexcept {
        return htonl(acknowledge);
    }

    void SetAcknowledgeSeqNum(uint32_t ackSeq) noexcept {
        acknowledge = htonl( ackSeq);
    }

    [[nodiscard]]
    inline uint32_t GetSequenceNum() const noexcept {
        return htonl(sequence);
    }

    inline void SetSequenceNum(uint32_t seq) noexcept {
        sequence = htonl(seq);
    }

    [[nodiscard]]
    inline bool GetFlag_SYN() const noexcept  {
        return ((uint16_t)(SYN)) == 1;
    }

    inline void SetFlag_SYN(bool state) noexcept {
        SYN = (uint16_t)state;
    }

    [[nodiscard]]
    inline bool GetFlag_ACK() const noexcept  {
        return ((uint16_t) (ACK)) == 1;
    }

    inline void SetFlag_ACK(bool state) noexcept {
        ACK = (uint16_t)state;
    }

    [[nodiscard]]
    inline bool GetFlag_FYN() const noexcept {
        return ((uint16_t) (FYN)) == 1;
    }

    inline void SetFlag_FYN(bool state) noexcept {
        FYN = (uint16_t)state;
    }

    [[nodiscard]]
    inline bool GetFlag_RST() const noexcept {
        return ((uint16_t) (RST)) == 1;
    }

    inline void SetFlag_RST(bool state) noexcept {
        RST = (uint16_t)state;
    }

    [[nodiscard]]
    inline bool GetFlag_PSH() const noexcept {
        return ((uint16_t) (PSH)) == 1;
    }

    inline void SetFlag_PSH(bool state) noexcept {
        PSH = (uint16_t)state;
    }

    [[nodiscard]]
    inline bool GetFlag_URG() const noexcept {
        return ((uint16_t) (URG)) == 1;
    }

    inline void SetFlag_URG(bool state) noexcept {
        URG = (uint16_t)state;
    }

    [[nodiscard]]
    inline uint16_t GetChecksum() const noexcept {
        return htons(checksum);
    }

    inline void SetChecksum(uint16_t checkSum) {
        checksum = checkSum;
    }

    [[nodiscard]]
    inline uint16_t GetUrgentPointer() const noexcept {
        return htons(urgent_pointer);
    }

    inline void SetUrgentPointer(uint16_t urgPtr){
        urgent_pointer = urgPtr;
    }

    // FIXME: Check for performance
    [[nodiscard("Call is expensive")]]
    static uint16_t CalcChecksum(const IPv4Header* ipHeader,
                                 const TCPHeader* tcpHeader,
                                 const uint8_t*  tcpOptions,
                                 const uint8_t* tcpData,
                                 uint16_t tcpDataLength)
    {
        char pseudoBuffer[10240]; // TODO: Bad!!
        unsigned short tcpHeaderLength = tcpHeader->GetDataOffset();
        unsigned short tcpOptionsLength = tcpHeaderLength - sizeof(TCPHeader);

        /** Pseudo TCP Header initialization: **/
        PseudoTCPHeader tcpPseudoHeader {};
        tcpPseudoHeader.sourceAddress = ipHeader->srcaddr;
        tcpPseudoHeader.destinationAddress = ipHeader->destaddr;
        tcpPseudoHeader.protocol = IPPROTO_TCP;
        tcpPseudoHeader.totalLength = htons(tcpHeaderLength + tcpDataLength);
        tcpPseudoHeader.useless = 0;

        /** Preparing pseudo data buffer. For TCP Checksum calculation: **/
        memcpy(pseudoBuffer, &tcpPseudoHeader, sizeof(PseudoTCPHeader));
        memcpy(pseudoBuffer + sizeof(PseudoTCPHeader), tcpHeader, sizeof(TCPHeader));
        if (tcpOptionsLength) {
            memcpy(pseudoBuffer + sizeof(PseudoTCPHeader) + sizeof(TCPHeader),
                   tcpOptions,
                   tcpOptionsLength);
        }
        if (tcpDataLength) {
            memcpy(pseudoBuffer + sizeof(PseudoTCPHeader) + sizeof(TCPHeader) + tcpOptionsLength,
                   tcpData,
                   tcpDataLength);
        }

        /** Calculating the checksum: **/
        return Utilities::Checksum((unsigned short*)pseudoBuffer,
                                     tcpHeaderLength + sizeof(PseudoTCPHeader) + tcpDataLength);
    }
};


#endif /* TCPHEADER_H_ */
