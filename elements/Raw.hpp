#ifndef RAW_HPP
#define RAW_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#   pragma once
#endif

#include <boost/asio/detail/config.hpp>
#include <boost/asio/detail/socket_types.hpp>
#include <boost/asio/basic_raw_socket.hpp>
#include <boost/asio/ip/basic_endpoint.hpp>
#include <boost/asio/ip/basic_resolver.hpp>
#include <boost/asio/ip/basic_resolver_iterator.hpp>
#include <boost/asio/ip/basic_resolver_query.hpp>

#include <boost/asio/detail/push_options.hpp>

typedef boost::asio::ip::address_v4 ip_address_v4;
typedef boost::asio::ip::address_v6 ip_address_v6;

namespace boost::asio::ip
{
    class raw
    {
    public:
        typedef basic_endpoint  <raw> endpoint;
        typedef basic_raw_socket<raw> socket;
        typedef basic_resolver  <raw> resolver;

        explicit raw() :
            Protocol(IPPROTO_RAW),
            Family  (PF_INET)
        {}

        static raw v4() { return raw(IPPROTO_RAW, PF_INET ); }
        static raw v6() { return raw(IPPROTO_RAW, PF_INET6); }

        int type    () const { return SOCK_RAW; }
        int protocol() const { return Protocol; }
        int family  () const { return Family  ; }

        friend bool operator==(const raw& p1, const raw& p2)
        {
            return p1.Protocol == p2.Protocol &&
                   p1.Family   == p2.Family;
        }
        friend bool operator!=(const raw& p1, const raw& p2)
        {
            return p1.Protocol != p2.Protocol ||
                   p1.Family   != p2.Family;
        }

    private:
        explicit raw(int protocol_id, int protocol_family) :
            Protocol(protocol_id    ),
            Family  (protocol_family)
        {}

        int Protocol;
        int Family;
    };
}

#include <istream>
#include <ostream>
#include <algorithm>
#include <numeric>

[[nodiscard]] constexpr uint16_t decode(const uint8_t* _rep_, uint8_t _a_, uint8_t _b_)
{
    return (_rep_[_a_] << 8) + _rep_[_b_];
}
[[nodiscard]] constexpr uint32_t decode(const uint8_t* _rep_, uint8_t _a_, uint8_t _b_, uint8_t _c_, uint8_t _d_)
{
    return (_rep_[_a_] << 24) + (_rep_[_b_] << 16) + (_rep_[_c_] << 8) + _rep_[_d_];
}

constexpr void encode(uint8_t* _rep_, uint8_t _a_, uint8_t _b_                          , uint16_t _n_)
{
    _rep_[_a_] = static_cast<uint8_t>((_n_ >> 8) & 0xFF);
    _rep_[_b_] = static_cast<uint8_t>((_n_ >> 0) & 0xFF);
}
constexpr void encode(uint8_t* _rep_, uint8_t _a_, uint8_t _b_, uint8_t _c_, uint8_t _d_, uint32_t _n_)
{
    _rep_[_a_] = static_cast<uint8_t>((_n_ >> 24) & 0xFF);
    _rep_[_b_] = static_cast<uint8_t>((_n_ >> 16) & 0xFF);
    _rep_[_c_] = static_cast<uint8_t>((_n_ >>  8) & 0xFF);
    _rep_[_d_] = static_cast<uint8_t>((_n_ >>  0) & 0xFF);
}

inline void print(uint8_t* _rep_)
{
    unsigned int i;
    printf("{");
    for (i = 0; i < sizeof(_rep_); i++)
        printf(i == sizeof(_rep_) - 1 ? "%.2X}\n" : "%.2X, ", _rep_[i]);
}

//Deprecated
/*
template<typename Protocol, typename Executor>
int set_manual_IP_header(boost::asio::basic_raw_socket<Protocol, Executor>* _socket_)
{
#   ifndef BOOST_WINDOWS
        int value = false;
#   else
        const char value = false;
#   endif
    return setsockopt(_socket_->native_handle(), IPPROTO_IP, IP_HDRINCL, &value, sizeof(value));
}
template<typename Protocol, typename Executor>
int   set_auto_IP_header(boost::asio::basic_raw_socket<Protocol, Executor>* _socket_)
{
#   ifndef BOOST_WINDOWS
        int value = true;
#   else
        const char value = true;
#   endif
    return setsockopt(_socket_->native_handle(), IPPROTO_IP, IP_HDRINCL, &value, sizeof(value));
}
*/

/*
                          <IPv4 header>
0               8               16              24             32
+-------+-------+---------+-----+---------------+--------------+      ---
|version|header |   DSCP  | ECN |             size             |       ^
|       | size  |         |     |                              |       |
+-------+-------+---------+-----+-+--+--+-------+--------------+       |
|              id               |R|NF|MF|   fragment offset    |       | 
+---------------+---------------+-+--+--+-------+--------------+       |
|      ttl      |   protocol    |        header checksum       |   20 bytes
+---------------+---------------+---------------+--------------+       |
|                          source addr                         |       |
+---------------+---------------+---------------+--------------+       |
|                     destination addr                         |       v
+---------------+---------------+---------------+--------------+      ---

DSCP - Differentiated Services Code Point
ECN  - Explicit Congestion Notification
R    - null flag (reserved)
NF   - don't fragment flag
MF   - the package still has fragments flag
*/
class alignas(1) IPv4_header
{
public:
    IPv4_header() noexcept { std::fill(rep, rep + sizeof(rep), 0); }
    
    uint8_t  version                () const noexcept { return (rep[0] & 0b11110000) >> 4; }
    uint8_t  header_size            () const noexcept { return  rep[0] & 0b00001111      ; }
    uint8_t  code_point             () const noexcept { return (rep[1] & 0b11111100) >> 2; }
    uint8_t  congestion_notification() const noexcept { return  rep[1] & 0b00000011      ; }
    uint16_t size() const noexcept { return decode(rep, 2, 3); }
    uint16_t id  () const noexcept { return decode(rep, 4, 5); }
    bool     dont_fragment () const noexcept { return rep[6] & 0b01000000; }
    bool     more_fragments() const noexcept { return rep[6] & 0b00100000; }
    uint16_t fragmet_offset() const noexcept { return decode(rep, 6, 7) & 0b0001111111111111; }
    uint8_t  ttl     () const noexcept { return rep[8]; }
    uint8_t  protocol() const noexcept { return rep[9]; }
    uint16_t checksum() const noexcept { return decode(rep, 10, 11); }
    ip_address_v4      source_addr() const noexcept { return ip_address_v4(decode(rep, 12, 13, 14, 15)); }
    ip_address_v4 destination_addr() const noexcept { return ip_address_v4(decode(rep, 16, 17, 18, 19)); }
    
    void version                (const uint8_t n) noexcept { rep[0] |= ((n & 0b00001111) << 4); }
    void header_size            (uint8_t n) noexcept { rep[0] |= ( n & 0b00001111      ); }
    void code_point             (uint8_t n) noexcept { rep[1] |= ((n & 0b00111111) << 2); }
    void congestion_notification(uint8_t n) noexcept { rep[1] |= ( n & 0b00000011      ); }
    void size(uint16_t n) noexcept { encode(rep, 2, 3, n); }
    void id  (uint16_t n) noexcept { encode(rep, 4, 5, n); }
    void dont_fragment (bool n) noexcept { rep[6] ^= (-n ^ rep[6]) & 0b01000000; }
    void more_fragments(bool n) noexcept { rep[6] ^= (-n ^ rep[6]) & 0b00100000; }
    void fragment_offset(uint16_t n) { encode(rep, 6, 7, (n & 0x1FFF) | ((rep[6] & 0b11100000) << 8)); }
    void ttl     (uint8_t  n) noexcept { rep[8] = n; }
    void protocol(uint8_t  n) noexcept { rep[9] = n; }
    void checksum(uint16_t n) noexcept { encode(rep, 10, 11, n); }
    void      source_addr(const ip_address_v4& n) noexcept { encode(rep, 12, 13, 14, 15, n.to_uint()); }
    void destination_addr(const ip_address_v4& n) noexcept { encode(rep, 16, 17, 18, 19, n.to_uint()); }

    static uint16_t generate_checksum(IPv4_header _header_) noexcept
    {
        _header_.checksum(0);
        uint32_t sum = std::accumulate<uint16_t*, uint32_t>
            (
                reinterpret_cast<uint16_t*>(&_header_.rep[0]                       ),
                reinterpret_cast<uint16_t*>(&_header_.rep[0] + sizeof(_header_.rep)),
                0
            );

        while (sum >> 16)
            sum = (sum & 0xFFFF) + (sum >> 16);

        return ~sum;
    }

    friend std::istream& operator>>(std::istream& is,       IPv4_header& header) noexcept { return is.read (reinterpret_cast<      char*>(header.rep), sizeof(rep)); }
    friend std::ostream& operator<<(std::ostream& os, const IPv4_header& header) noexcept { return os.write(reinterpret_cast<const char*>(header.rep), sizeof(rep)); }

    bool operator==(const IPv4_header& other) const noexcept { return !std::memcmp(rep, other.rep, sizeof(rep)); }

private:
    uint8_t rep[20];
};

/*
                       <Pseudo IPv4 header>
0               8               16              24             32
+---------------+---------------+---------------+--------------+      ---
|                          source addr                         |       ^
+---------------+---------------+---------------+--------------+       |
|                     destination addr                         |   12 bytes
+---------------+---------------+---------------+--------------+       |
|     zeros     |   protocol    |            length            |       v
+---------------+---------------+---------------+--------------+      ---
*/
class alignas(1) pseudo_IPv4_header
{
public:
    pseudo_IPv4_header() noexcept { std::fill(rep, rep + sizeof(rep), 0); }

    ip_address_v4      source_addr() const noexcept { return ip_address_v4(decode(rep, 0, 1, 2, 3)); }
    ip_address_v4 destination_addr() const noexcept { return ip_address_v4(decode(rep, 4, 5, 6, 7)); }
    uint8_t  protocol() const noexcept { return rep[9]; }
    uint16_t lenght  () const noexcept { return decode(rep, 10, 11); }

    void      source_addr(const ip_address_v4& n) noexcept { encode(rep, 0, 1, 2, 3, n.to_uint()); }
    void destination_addr(const ip_address_v4& n) noexcept { encode(rep, 4, 5, 6, 7, n.to_uint()); }
    void protocol(uint8_t  n) noexcept { rep[9] = n; }
    void lenght  (uint16_t n) noexcept { encode(rep, 10, 11, n); }

    friend std::istream& operator>>(std::istream& is,       pseudo_IPv4_header& header) noexcept { return is.read (reinterpret_cast<      char*>(header.rep), sizeof(rep)); }
    friend std::ostream& operator<<(std::ostream& os, const pseudo_IPv4_header& header) noexcept { return os.write(reinterpret_cast<const char*>(header.rep), sizeof(rep)); }

private:
    uint8_t rep[12];
};

/*
                       <Pseudo IPv6 header>
0               8               16              24             32
+---------------+---------------+---------------+--------------+      ---
|                                                              |       ^
|                          source addr                         |       |
|                                                              |       |
|                                                              |       |
+---------------+---------------+---------------+--------------+       |
|                                                              |       |
|                     destination addr                         |   40 bytes
|                                                              |       |
|                                                              |       |
+---------------+---------------+---------------+--------------+       |
|                            lenght                            |       |
+---------------+---------------+---------------+--------------+       |
|                     zeros                     | next header  |       v
+---------------+---------------+---------------+--------------+      ---
*/
class alignas(1) pseudo_IPv6_header
{
public:
    pseudo_IPv6_header() noexcept { std::fill(rep, rep + sizeof(rep), 0); }

    ip_address_v6      source_addr() const noexcept { return ip_address_v6({ rep[ 0], rep[ 1], rep[ 2], rep[ 3], rep[ 4], rep[ 5], rep[ 6], rep[ 7], rep[ 8], rep[ 9], rep[10], rep[11], rep[12], rep[13], rep[14], rep[15] }); }
    ip_address_v6 destination_addr() const noexcept { return ip_address_v6({ rep[16], rep[17], rep[18], rep[19], rep[20], rep[21], rep[22], rep[23], rep[24], rep[25], rep[26], rep[27], rep[28], rep[29], rep[30], rep[31] }); }
    uint16_t lenght     () const noexcept { return decode(rep, 32, 33, 34, 35); }
    uint8_t  next_header() const noexcept { return rep[39]; }

    void      source_addr(const ip_address_v6& n) noexcept
    {
        auto arr = n.to_bytes();
        rep[ 0] = arr[ 0];
        rep[ 1] = arr[ 1];
        rep[ 2] = arr[ 2];
        rep[ 3] = arr[ 3];
        rep[ 4] = arr[ 4];
        rep[ 5] = arr[ 5];
        rep[ 6] = arr[ 6];
        rep[ 7] = arr[ 7];
        rep[ 8] = arr[ 8];
        rep[ 9] = arr[ 9];
        rep[10] = arr[10];
        rep[11] = arr[11];
        rep[12] = arr[12];
        rep[13] = arr[13];
        rep[14] = arr[14];
        rep[15] = arr[15];
    }
    void destination_addr(const ip_address_v6& n) noexcept
    {
        auto arr = n.to_bytes();
        rep[16] = arr[ 0];
        rep[17] = arr[ 1];
        rep[18] = arr[ 2];
        rep[19] = arr[ 3];
        rep[20] = arr[ 4];
        rep[21] = arr[ 5];
        rep[22] = arr[ 6];
        rep[23] = arr[ 7];
        rep[24] = arr[ 8];
        rep[25] = arr[ 9];
        rep[26] = arr[10];
        rep[27] = arr[11];
        rep[28] = arr[12];
        rep[29] = arr[13];
        rep[30] = arr[14];
        rep[31] = arr[15];
    }
    void lenght     (uint16_t n) noexcept { encode(rep, 32, 33, 34, 35, n); }
    void next_header(uint8_t  n) noexcept { rep[39] = n; }

    friend std::istream& operator>>(std::istream& is,       pseudo_IPv6_header& header) noexcept { return is.read (reinterpret_cast<      char*>(header.rep), sizeof(rep)); }
    friend std::ostream& operator<<(std::ostream& os, const pseudo_IPv6_header& header) noexcept { return os.write(reinterpret_cast<const char*>(header.rep), sizeof(rep)); }

private:
    uint8_t rep[40];
};

/*
                           <UDP header>
0               8               16              24             31
+---------------+---------------+---------------+--------------+      ---
|          source port          |       destination port       |       ^
+---------------+---------------+---------------+--------------+    8 bytes
|            length             |           checksum           |       v
+---------------+---------------+---------------+--------------+      ---
*/
class alignas(1) udp_header
{
public:
    udp_header() noexcept { std::fill(rep, rep + sizeof(rep), 0); }

    uint16_t      source_port() const noexcept { return decode(rep, 0, 1); }
    uint16_t destination_port() const noexcept { return decode(rep, 2, 3); }
    uint16_t   length        () const noexcept { return decode(rep, 4, 5); }
    uint16_t checksum        () const noexcept { return decode(rep, 6, 7); }

    void source_port     (uint16_t n) noexcept { encode(rep, 0, 1, n); }
    void destination_port(uint16_t n) noexcept { encode(rep, 2, 3, n); }
    void length          (uint16_t n) noexcept { encode(rep, 4, 5, n); }
    void checksum        (uint16_t n) noexcept { encode(rep, 6, 7, n); }

    friend std::istream& operator>>(std::istream& is,       udp_header& header) noexcept { return is.read (reinterpret_cast<      char*>(header.rep), sizeof(rep)); }
    friend std::ostream& operator<<(std::ostream& os, const udp_header& header) noexcept { return os.write(reinterpret_cast<const char*>(header.rep), sizeof(rep)); }

private:
    uint8_t rep[8];
};

/*
                           <TCP header>
0               8               16              24             31
+---------------+---------------+---------------+--------------+      ---
|          source port          |       destination port       |       ^
+---------------+---------------+---------------+--------------+       |
|                       Sequence Number (SN)                   |       |
+---------------+---------------+---------------+--------------+       |
|                 Acknowledgment Number (ACK SN)               |       |
+-------+-----------+-+-+-+-+-+-+---------------+--------------+   20 bytes
| Data  |           |U|A|P|R|S|F|                              |       |
| offset| reserved  |R|C|S|S|Y|I|         Window size          |       |
|       |           |G|K|H|T|N|N|                              |       |
+-------+-----------+-+-+-+-+-+-+---------------+--------------+       |
|           Checksum            |        Urgent Pointer        |       v
+---------------+---------------+---------------+--------------+      ---
*/
class alignas(1) tcp_header
{
public:
    tcp_header() noexcept { std::fill(rep, rep + sizeof(rep), 0); }

    uint16_t           source_port() const noexcept { return decode(rep, 0, 1); }
    uint16_t      destination_port() const noexcept { return decode(rep, 2, 3); }
    uint32_t       sequence_number() const noexcept { return decode(rep, 4, 5,  6,  7); }
    uint32_t acknowledgment_number() const noexcept { return decode(rep, 8, 9, 10, 11); }
    uint8_t data_offset() const noexcept { return rep[12] >> 4; }
    bool URG() const noexcept { return rep[13] & 0b00100000; }
    bool ACK() const noexcept { return rep[13] & 0b00010000; }
    bool PSH() const noexcept { return rep[13] & 0b00001000; }
    bool RST() const noexcept { return rep[13] & 0b00000100; }
    bool SYN() const noexcept { return rep[13] & 0b00000010; }
    bool FIN() const noexcept { return rep[13] & 0b00000001; }
    uint16_t window_size   () const noexcept { return decode(rep, 14, 15); }
    uint16_t checksum      () const noexcept { return decode(rep, 16, 17); }
    uint16_t urgent_pointer() const noexcept { return decode(rep, 18, 19); }

    void      source_port     (uint16_t n) noexcept { encode(rep, 0, 1, n); }
    void destination_port     (uint16_t n) noexcept { encode(rep, 2, 3, n); }
    void       sequence_number(uint32_t n) noexcept { encode(rep, 4, 5,  6,  7, n); }
    void acknowledgment_number(uint32_t n) noexcept { encode(rep, 8, 9, 10, 11, n); }
    void data_offset(uint8_t n) noexcept { rep[12] = n << 4; }
    void URG(bool n) noexcept { rep[13] = (n ? (rep[13] | 0b00100000) : (rep[13] & (~0b00100000))); }
    void ACK(bool n) noexcept { rep[13] = (n ? (rep[13] | 0b00010000) : (rep[13] & (~0b00010000))); }
    void PSH(bool n) noexcept { rep[13] = (n ? (rep[13] | 0b00001000) : (rep[13] & (~0b00001000))); }
    void RST(bool n) noexcept { rep[13] = (n ? (rep[13] | 0b00000100) : (rep[13] & (~0b00000100))); }
    void SYN(bool n) noexcept { rep[13] = (n ? (rep[13] | 0b00000010) : (rep[13] & (~0b00000010))); }
    void FIN(bool n) noexcept { rep[13] = (n ? (rep[13] | 0b00000001) : (rep[13] & (~0b00000001))); }
    void window_size   (uint16_t n) noexcept { encode(rep, 14, 15, n); }
    void checksum      (uint16_t n) noexcept { encode(rep, 16, 17, n); }
    void urgent_pointer(uint16_t n) noexcept { encode(rep, 18, 19, n); }

    friend std::istream& operator>>(std::istream& is,       tcp_header& header) noexcept { return is.read (reinterpret_cast<      char*>(header.rep), sizeof(rep)); }
    friend std::ostream& operator<<(std::ostream& os, const tcp_header& header) noexcept { return os.write(reinterpret_cast<const char*>(header.rep), sizeof(rep)); }

private:
    uint8_t rep[20];
};

/*
                          <ICMP header>
0               8               16              24             31
+---------------+---------------+---------------+--------------+      ---
|               |               |                              |       ^
|     type      |     code      |           checksum           |    4 bytes
|               |               |                              |       v
+---------------+---------------+---------------+--------------+      ---
*/
class alignas(1) icmp_header
{
public:
    icmp_header() noexcept { std::fill(rep, rep + sizeof(rep), 0); }

    uint8_t  type    () const noexcept { return rep[0]; }
    uint8_t  code    () const noexcept { return rep[1]; }
    uint16_t checksum() const noexcept { return decode(rep, 2, 3); }

    void type    (uint8_t  n) noexcept { rep[0] = n; }
    void code    (uint8_t  n) noexcept { rep[1] = n; }
    void checksum(uint16_t n) noexcept { encode(rep, 2, 3, n); }

    friend std::istream& operator>>(std::istream& is,       icmp_header& header) noexcept { return is.read (reinterpret_cast<      char*>(header.rep), sizeof(rep)); }
    friend std::ostream& operator<<(std::ostream& os, const icmp_header& header) noexcept { return os.write(reinterpret_cast<const char*>(header.rep), sizeof(rep)); }

private:
    uint8_t rep[4];
};

#endif // RAW_HPP