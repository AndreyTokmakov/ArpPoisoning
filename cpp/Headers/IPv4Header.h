/**============================================================================
Name        : IPv4Header.h
Created on  : 09.12.2022
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : IPv4Header
============================================================================**/

#ifndef IPHEADER_H_
#define IPHEADER_H_

#include <arpa/inet.h>
#include <string_view>

#include "../Utilities/Utilities.h"


// TODO: Set alignment
struct IPv4Header final
{
    /** 4-bit header length (in 32-bit words) normally=5 (Means 20 Bytes may be 24 also): **/
    uint8_t ipHeaderLength:4;

    /** 4-bit IPv4 version: **/
    uint8_t ipVersion :4;

    /** IP type of service: **/
    uint8_t serviceType;

    /** Packet total length. Payload data size included: **/
    uint16_t packetLength;

    /** Unique packet identifier: **/
    uint16_t packetId;

    /** Fragment offset fields: **/
    uint8_t  frag_offset :5;
    uint8_t  more_fragment :1;
    uint8_t  dont_fragment :1;
    uint8_t  reserved_zero :1;

    /** Fragment offset. **/
    uint8_t  frag_offset1;

    /** Time to live: (TTL) **/
    uint8_t  timeToLive;

    /** Protocol(TCP,UDP etc): **/
    uint8_t  protocol;

    /** IP checksum: **/
    uint16_t checksum;

    /** Source address: **/
    uint32_t srcaddr;

    /** Dext address: **/
    uint32_t destaddr;

public:
    [[nodiscard]]
    inline uint16_t GetIPVersion() const {
        return ipVersion;
    }

    inline void SetIPVersion(uint16_t version = 4) {
        ipVersion = version;
    }

    [[nodiscard]]
    inline uint16_t GetIPHeaderLength() const noexcept {
        return (ipHeaderLength * 4);
    }

    // TODO: Remove?
    inline void SetIPHeaderLength(uint16_t len = 20) {
        ipHeaderLength = (len / 4);
    }

    [[nodiscard]]
    inline uint16_t GetServiceType() const noexcept {
        return serviceType;
    }

    inline void SetServiceType(uint16_t tos = 16) {
        serviceType = (uint8_t)tos;
    }

    [[nodiscard]]
    inline uint16_t GetPacketLength() const noexcept {
        return ntohs(packetLength);
    }

    inline void SetPacketLength(uint16_t length) {
        packetLength = htons(length);
    }

    [[nodiscard]]
    inline uint16_t GetPacketId() const noexcept {
        return htons(packetId);
    }

    inline void SetPacketId(uint16_t id) {
        packetId = htons(id);
    }

    [[nodiscard]]
    inline uint16_t GetTimeToLive() const noexcept {
        return timeToLive;
    }

    inline void SetTimeToLive(uint16_t ttl = 64) {
        timeToLive = (uint8_t)ttl;
    }

    [[nodiscard]]
    inline uint16_t GetProtocol() const noexcept {
        return protocol;
    }

    inline void SetProtocol(uint16_t proto = IPPROTO_TCP) {
        protocol = (uint8_t)proto;
    }

    [[nodiscard]]
    inline uint16_t GetChecksum() const noexcept {
        return checksum;
    }

    inline void SetChecksum(uint16_t csum) {
        checksum = csum;
    }

    [[nodiscard]]
    inline uint16_t GetDontFragment() const noexcept {
        return dont_fragment;
    }

    inline void SetDontFragment(uint16_t fragment) {
        dont_fragment = (uint8_t)fragment;
    }

    [[nodiscard]]
    inline uint32_t GetSourceAddressAsInt() const noexcept {
        return srcaddr;
    }

    [[nodiscard]]
    std::string GetSourceAddress() const  {
        return ResolveAddress(srcaddr);
    }

    void SetSourceAddress(const std::string &address) {
        srcaddr = inet_addr(address.c_str());
    }

    void SetSourceAddress(const sockaddr_in &address) {
        srcaddr = inet_addr(inet_ntoa(address.sin_addr));
    }

    [[nodiscard]]
    inline std::string GetDestAddress() const {
        return ResolveAddress(destaddr);
    }

    [[nodiscard]]
    inline uint32_t GetDestAddressAsInt() const noexcept {
        return destaddr;
    }

    void SetDestAddress(const std::string &address) {
        destaddr = inet_addr(address.c_str());
    }

    void SetDestAddress(const sockaddr_in &address) {
        destaddr = inet_addr(inet_ntoa(address.sin_addr));
    }

    // TODO: Check for performance
    // FIXME:
    [[nodiscard]]
    static std::string ResolveAddress(uint32_t address) {
        in_addr addr {};
        addr.s_addr = address;
        return inet_ntoa(addr);
    }
};

#endif /* IPHEADER_H_ */


