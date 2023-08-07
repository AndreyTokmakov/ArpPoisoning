/**============================================================================
Name        : Utilities.h
Created on  : 09.12.2022
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Utilities
============================================================================**/

#ifndef CPPPROJECTS_UTILITIES_H
#define CPPPROJECTS_UTILITIES_H

#include <string>
#include <linux/if_packet.h>
#include <cstdint>
#include <utility>


namespace Utilities
{
    [[nodiscard("Call may be expensive")]]
    uint16_t Checksum(uint16_t *ptr, uint16_t bytes) noexcept;

    [[nodiscard]]
    sockaddr_ll ResolveInterfaceAddress(std::string_view interfaceName);

    // [[nodiscard]]
    bool ResolveInterfaceAddress(std::string_view interfaceName, sockaddr_ll& out);

    // TODO: Fix perf
    void PrintMACAddress(const uint8_t* mac);

    // TODO: Fix perf
    [[nodiscard("nodiscard")]]
    std::string IpToStr(unsigned long address);

    // TODO: Fix perf
    [[nodiscard("Don't forget to use the return value somehow.")]]
    std::string HostToIp(std::string_view host) noexcept;

    // Terminates if not
    void checkRunningUnderRoot();

    struct SocketScoped
    {
        static constexpr int INVALID_HANDLE { -1 };
        static constexpr int SOCKET_ERROR { -1 };

        int handle { INVALID_HANDLE };

        SocketScoped(int s = INVALID_HANDLE) : handle { s } {
        }

        SocketScoped& operator=(int s) {
            handle = s;
            return *this;
        }

        SocketScoped(const SocketScoped& sock) = default;
        SocketScoped& operator=(const SocketScoped& sock) = default;

        SocketScoped(SocketScoped&& sock) noexcept :
                handle { std::exchange(sock.handle, INVALID_HANDLE)}  {
        }

        SocketScoped& operator=(SocketScoped&& sock) noexcept {
            handle = std::exchange(sock.handle, INVALID_HANDLE);
            return *this;
        }

        operator int() const { // No explicit
            return handle;
        }

        [[nodiscard]]
        inline bool isValid() const noexcept {
            return INVALID_HANDLE == handle;
        }

        explicit operator bool() const noexcept {
            return (INVALID_HANDLE != handle);
        }

        ~SocketScoped() {
            closeSocket(handle);
        }

    private:
        static void closeSocket(int s);
    };

    void setBroadcast(Utilities::SocketScoped& sock,
                      bool enabled = true);
};

namespace Utilities::IP
{
    constexpr uint32_t ip_octets_to_int(uint8_t first, uint8_t second, uint8_t third, uint8_t fourth)
    {
        return (first << 24) | (second << 16) | (third << 8) | (fourth);
    }

    constexpr uint32_t ip_octets_to_int2(uint8_t first, uint8_t second, uint8_t third, uint8_t fourth)
    {
        int result = first << 24;
        result |= second << 16;
        result |= third << 8;
        return result |= fourth;
    }

    [[nodiscard]]
    std::string ipInt2Str(uint32_t ip);

    [[nodiscard]]
    std::string ipInt2StrOLD(uint32_t ip);

    uint32_t ipInt2Str(std::string_view ip);
    uint32_t ipInt2Str2(std::string_view ip);

}


#endif //CPPPROJECTS_UTILITIES_H
