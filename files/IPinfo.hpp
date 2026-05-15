#ifndef IP_INFO_HPP
#define IP_INFO_HPP

// IPv6
enum ip_address_v6_type
{
    ip_address_v6_type_Unknown,

    ip_address_v6_type_Unspecified_address,
    ip_address_v6_type_Loopback,
    ip_address_v6_type_IPv4_mapped_addresses,
    ip_address_v6_type_NAT64_translation,
    ip_address_v6_type_Local_use_translation,
    ip_address_v6_type_Discard_prefix,
    ip_address_v6_type_Teredo_tunneling,
    ip_address_v6_type_ORCHIDv2,
    ip_address_v6_type_Documentation_and_example,
    ip_address_v6_type_6to4,
    ip_address_v6_type_SRv6,
    ip_address_v6_type_Unique_local_address,
    ip_address_v6_type_Link_local_address,

    ip_address_v6_type_Multicast_address,
    ip_address_v6_type_Multicast_Solicited_node_address,
    ip_address_v6_type_Multicast_Node_information_queries,

    ip_address_v6_type_Count
};
inline const char* type_to_str(const ip_address_v6_type& _type_)
{
    switch (_type_)
    {
    case ip_address_v6_type_Unknown                           : return "Не указан";
    
    case ip_address_v6_type_Unspecified_address               : return "Неуказанный адрес";
    case ip_address_v6_type_Loopback                          : return "Loopback";
    case ip_address_v6_type_IPv4_mapped_addresses             : return "Адрес, сопоставленный с IPv4";
    case ip_address_v6_type_NAT64_translation                 : return "NAT64 (IPv4 <-> IPv6)";
    case ip_address_v6_type_Local_use_translation             : return "Локальное преобразование IPv4 <-> IPv6";
    case ip_address_v6_type_Discard_prefix                    : return "Отбрасываемый префикс";
    case ip_address_v6_type_Teredo_tunneling                  : return "Teredo тоннель";
    case ip_address_v6_type_ORCHIDv2                          : return "ORCHIDv2";
    case ip_address_v6_type_Documentation_and_example         : return "Документация или пример";
    case ip_address_v6_type_6to4                              : return "Адресация 6to4";
    case ip_address_v6_type_SRv6                              : return "Маршрутизация сегментов (SRv6)";
    case ip_address_v6_type_Unique_local_address              : return "Уникальный локальный адрес";
    case ip_address_v6_type_Link_local_address                : return "Локальный адрес";

    case ip_address_v6_type_Multicast_address                 : return "Мультикаст адрес";
    case ip_address_v6_type_Multicast_Solicited_node_address  : return "Многоадресная рассылка";
    case ip_address_v6_type_Multicast_Node_information_queries: return "Запрос информации об узле";

    default: return "<ОШИБКА>";
    }
}

//Область видимости / охвата
enum ip_address_v6_scope_type
{
    ip_address_v6_scope_type_Unknown           ,

    ip_address_v6_scope_type_Reserved          ,
    
    ip_address_v6_scope_type_Interface_local   , //Interface-local scope spans only a single interface on a node, and is useful only for loopback transmission of multicast.
    ip_address_v6_scope_type_Link_local        , //Link-local scope spans the same topological region as the corresponding unicast scope.
    ip_address_v6_scope_type_Realm_local       , //Realm-local scope is defined as larger than link-local, automatically determined by network topology and must not be larger than the following scopes.
    ip_address_v6_scope_type_Admin_local       , //Admin-local scope is the smallest scope that must be administratively configured, i.e., not automatically derived from physical connectivity or other, non-multicast-related configuration.
    ip_address_v6_scope_type_Site_local        , //Site-local scope is intended to span a single site belonging to an organisation.
    ip_address_v6_scope_type_Organization_local, //Organization-local scope is intended to span all sites belonging to a single organization.
    ip_address_v6_scope_type_Global            , //Global scope spans all reachable nodes on the Internet – it is unbounded.

    ip_address_v6_scope_type_Count             ,

    ip_address_v6_scope_type_All                 //Just a internal flag for this software
};
inline const char* type_to_str(const ip_address_v6_scope_type& _type_)
{
    switch (_type_)
    {
    case ip_address_v6_scope_type_Unknown           : return "Не указан";

    case ip_address_v6_scope_type_Reserved          : return "Зарезервированно";
    
    case ip_address_v6_scope_type_Interface_local   : return "";
    case ip_address_v6_scope_type_Link_local        : return "";
    case ip_address_v6_scope_type_Realm_local       : return "";
    case ip_address_v6_scope_type_Admin_local       : return "";
    case ip_address_v6_scope_type_Site_local        : return "";
    case ip_address_v6_scope_type_Organization_local: return "";
    case ip_address_v6_scope_type_Global            : return "";
    case ip_address_v6_scope_type_All               : return "";

    default: return "<ОШИБКА>";
    }
}
inline uint8_t                  scope_to_type(ip_address_v6_scope_type _type_ )
{
    switch (_type_)
    {
    case ip_address_v6_scope_type_Interface_local   : return 0x1;
    case ip_address_v6_scope_type_Link_local        : return 0x2;
    case ip_address_v6_scope_type_Realm_local       : return 0x3;
    case ip_address_v6_scope_type_Admin_local       : return 0x4;
    case ip_address_v6_scope_type_Site_local        : return 0x5;
    case ip_address_v6_scope_type_Organization_local: return 0x8;
    case ip_address_v6_scope_type_Global            : return 0xe;
    default: return 0x0;
    }
}
inline ip_address_v6_scope_type type_to_scope(uint8_t                  _scope_)
{
    switch (_scope_)
    {
    case 0x0: return ip_address_v6_scope_type_Reserved;
    case 0x1: return ip_address_v6_scope_type_Interface_local;
    case 0x2: return ip_address_v6_scope_type_Link_local;
    case 0x3: return ip_address_v6_scope_type_Realm_local;
    case 0x4: return ip_address_v6_scope_type_Admin_local;
    case 0x5: return ip_address_v6_scope_type_Site_local;
    case 0x8: return ip_address_v6_scope_type_Organization_local;
    case 0xe: return ip_address_v6_scope_type_Global;
    case 0xf: return ip_address_v6_scope_type_Reserved;
    default: return ip_address_v6_scope_type_Count;
    }
}

enum ip_address_v6_multicast_type
{
    ip_address_v6_multicast_type_All_nodes_address                   , //Identify the group of all IPv6 nodes
    ip_address_v6_multicast_type_All_routers                         ,
    ip_address_v6_multicast_type_OSPFIGP                             ,
    ip_address_v6_multicast_type_OSPFIGP_designated_routers          ,
    ip_address_v6_multicast_type_RIP_routers                         ,
    ip_address_v6_multicast_type_EIGRP_routers                       ,
    ip_address_v6_multicast_type_Web_Services_Dynamic_Discovery      ,
    ip_address_v6_multicast_type_All_PIM_routers                     ,
    ip_address_v6_multicast_type_All_RPL_routers                     ,
    ip_address_v6_multicast_type_mDNSv6                              ,
    ip_address_v6_multicast_type_All_NTP_servers                     ,
    ip_address_v6_multicast_type_Link_name                           ,
    ip_address_v6_multicast_type_DHCPv6_servers_and_relay            ,
    ip_address_v6_multicast_type_Link_local_multicast_name_resolution,
    ip_address_v6_multicast_type_All_DHCPv6_servers_for_relay_agent
};
inline const char* type_to_str(const ip_address_v6_multicast_type& _type_)
{
    //TODO
}

const std::pair<network_v6, ip_address_v6_type> IPv6_ranges[17]
{
    { { make_address_v6("::"            ), 128 }, ip_address_v6_type_Unspecified_address                },
    { { make_address_v6("::1"           ), 128 }, ip_address_v6_type_Loopback                           },
    { { make_address_v6("::ffff:0:0"    ),  96 }, ip_address_v6_type_IPv4_mapped_addresses              },
    { { make_address_v6("64:ff9b::"     ),  96 }, ip_address_v6_type_NAT64_translation                  },
    { { make_address_v6("64:ff9b:1::"   ),  48 }, ip_address_v6_type_Local_use_translation              },
    { { make_address_v6("100::"         ),  64 }, ip_address_v6_type_Discard_prefix                     },
    { { make_address_v6("2001::"        ),  32 }, ip_address_v6_type_Teredo_tunneling                   },
    { { make_address_v6("2001:20::"     ),  28 }, ip_address_v6_type_ORCHIDv2                           },
    { { make_address_v6("2001:db8::"    ),  32 }, ip_address_v6_type_Documentation_and_example          },
    { { make_address_v6("2002::"        ),  16 }, ip_address_v6_type_6to4                               },
    { { make_address_v6("3fff::"        ),  20 }, ip_address_v6_type_Documentation_and_example          },
    { { make_address_v6("5f00::"        ),  16 }, ip_address_v6_type_SRv6                               },
    { { make_address_v6("fc00::"        ),   7 }, ip_address_v6_type_Unique_local_address               },
    { { make_address_v6("fe80::"        ),  64 }, ip_address_v6_type_Link_local_address                 },

    { { make_address_v6("ff02::1:ff00:0"), 104 }, ip_address_v6_type_Multicast_Solicited_node_address   },
    { { make_address_v6("ff02::2:ff00:0"), 104 }, ip_address_v6_type_Multicast_Node_information_queries },
    { { make_address_v6("ff00::"        ),   8 }, ip_address_v6_type_Multicast_address                  }
};
const std::pair<network_v6, ip_address_v6_multicast_type> IPv6_multicast_addresses[20]
{
    { { make_address_v6("ff00::1"  ), ip_address_v6_scope_type_Interface_local }, ip_address_v6_multicast_type_All_nodes_address                    },
    { { make_address_v6("ff00::1"  ), ip_address_v6_scope_type_Link_local      }, ip_address_v6_multicast_type_All_nodes_address                    },
    { { make_address_v6("ff00::2"  ), ip_address_v6_scope_type_Interface_local }, ip_address_v6_multicast_type_All_routers                          },
    { { make_address_v6("ff00::2"  ), ip_address_v6_scope_type_Link_local      }, ip_address_v6_multicast_type_All_routers                          },
    { { make_address_v6("ff00::2"  ), ip_address_v6_scope_type_Site_local      }, ip_address_v6_multicast_type_All_routers                          },
    { { make_address_v6("ff00::5"  ), ip_address_v6_scope_type_Link_local      }, ip_address_v6_multicast_type_OSPFIGP                              },
    { { make_address_v6("ff00::6"  ), ip_address_v6_scope_type_Link_local      }, ip_address_v6_multicast_type_OSPFIGP_designated_routers           },
    { { make_address_v6("ff00::9"  ), ip_address_v6_scope_type_Link_local      }, ip_address_v6_multicast_type_RIP_routers                          },
    { { make_address_v6("ff00::a"  ), ip_address_v6_scope_type_Link_local      }, ip_address_v6_multicast_type_EIGRP_routers                        },
    { { make_address_v6("ff00::c"  ), ip_address_v6_scope_type_Link_local      }, ip_address_v6_multicast_type_Web_Services_Dynamic_Discovery       },
    { { make_address_v6("ff00::d"  ), ip_address_v6_scope_type_Link_local      }, ip_address_v6_multicast_type_All_PIM_routers                      },
    { { make_address_v6("ff00::1a" ), ip_address_v6_scope_type_Link_local      }, ip_address_v6_multicast_type_All_RPL_routers                      },
    { { make_address_v6("ff00::fb" ), ip_address_v6_scope_type_All             }, ip_address_v6_multicast_type_mDNSv6                               },
    { { make_address_v6("ff00::101"), ip_address_v6_scope_type_All             }, ip_address_v6_multicast_type_All_NTP_servers                      },
    { { make_address_v6("ff00::1:1"), ip_address_v6_scope_type_Link_local      }, ip_address_v6_multicast_type_Link_name                            },
    { { make_address_v6("ff00::1:2"), ip_address_v6_scope_type_Link_local      }, ip_address_v6_multicast_type_DHCPv6_servers_and_relay             },
    { { make_address_v6("ff00::1:3"), ip_address_v6_scope_type_Link_local      }, ip_address_v6_multicast_type_Link_local_multicast_name_resolution },
    { { make_address_v6("ff00::1:3"), ip_address_v6_scope_type_Site_local      }, ip_address_v6_multicast_type_All_DHCPv6_servers_for_relay_agent   },
};

// IPv4
enum ip_address_v4_type : uint8_t
{
    ip_address_v4_type_Current_network          , //Current - (local, "this").
    ip_address_v4_type_Shared_address_space     , //Shared address space for communications between a service provider and its subscribers when using a carrier-grade NAT.
    ip_address_v4_type_Loopback                 ,
    ip_address_v4_type_Link_local_addresses     , //Used for link-local addresses between two hosts on a single link when no IP address is otherwise specified, such as would have normally been retrieved from a DHCP server.
    ip_address_v4_type_Local_communications     , //Used for local communications within a private network.

    ip_address_v4_type_IETF_protocol            , // /24
    ip_address_v4_type_DS_Lite                  , // /29
    
    ip_address_v4_type_Documentation_and_example,
    ip_address_v4_type_Reserved                 ,
    ip_address_v4_type_Benchmark_testing        , //Used for benchmark testing of inter-network communications between two separate subnets.
    ip_address_v4_type_Multicast_address        , //In use for multicast (former Class D network)
    ip_address_v4_type_Broadcast                  //Reserved for the "limited broadcast" destination address.
};
inline const char* type_to_str(const ip_address_v4_type& _type_)
{
    switch (_type_)
    {
    case ip_address_v4_type_Current_network          : return "Текущая (локальная) сеть";
    case ip_address_v4_type_Shared_address_space     : return "Общее адресное пространство";
    case ip_address_v4_type_Loopback                 : return "Loopback";
    case ip_address_v4_type_Link_local_addresses     : return "Адрес прямой коммуникации";
    case ip_address_v4_type_Local_communications     : return "Локальный адрес";

    case ip_address_v4_type_DS_Lite                  : return "DS Lite";
    case ip_address_v4_type_IETF_protocol            : return "IETF протокол";
    
    case ip_address_v4_type_Documentation_and_example: return "Документация или примеры";
    case ip_address_v4_type_Reserved                 : return "Зарезервированно";
    case ip_address_v4_type_Benchmark_testing        : return "Нагрузочные тесты";
    case ip_address_v4_type_Multicast_address        : return "Multicast";
    case ip_address_v4_type_Broadcast                : return "Broadcast";

    default: return "<ОШИБКА>";
    }
}

const std::pair<network_v4, ip_address_v4_type> IPv4_ranges[18]
{
    { { make_address_v4("0.0.0.0"        ),  8 }, ip_address_v4_type_Current_network           },
    { { make_address_v4("10.0.0.0"       ),  8 }, ip_address_v4_type_Local_communications      },
    { { make_address_v4("100.64.0.0"     ), 10 }, ip_address_v4_type_Shared_address_space      },
    { { make_address_v4("127.0.0.0"      ),  8 }, ip_address_v4_type_Loopback                  },
    { { make_address_v4("169.254.0.0"    ), 16 }, ip_address_v4_type_Link_local_addresses      },
    { { make_address_v4("172.16.0.0"     ), 12 }, ip_address_v4_type_Local_communications      },

    { { make_address_v4("192.0.0.0"      ), 29 }, ip_address_v4_type_DS_Lite                   },
    { { make_address_v4("192.0.0.0"      ), 24 }, ip_address_v4_type_IETF_protocol             },

    { { make_address_v4("192.0.2.0"      ), 24 }, ip_address_v4_type_Documentation_and_example },
    { { make_address_v4("192.88.99.0"    ), 24 }, ip_address_v4_type_Reserved                  }, //Formerly used for IPv6 to IPv4 relay (included IPv6 address block 2002::/16).
    { { make_address_v4("192.168.0.0"    ), 16 }, ip_address_v4_type_Local_communications      },
    { { make_address_v4("198.18.0.0"     ), 15 }, ip_address_v4_type_Benchmark_testing         },
    { { make_address_v4("198.51.100.0"   ), 24 }, ip_address_v4_type_Documentation_and_example },
    { { make_address_v4("203.0.113.0"    ), 24 }, ip_address_v4_type_Documentation_and_example },
    { { make_address_v4("224.0.0.0"      ),  4 }, ip_address_v4_type_Multicast_address         },
    { { make_address_v4("233.252.0.0"    ), 24 }, ip_address_v4_type_Documentation_and_example },
    { { make_address_v4("240.0.0.0"      ),  4 }, ip_address_v4_type_Reserved                  }, //Reserved for future use (former Class E network).
    { { make_address_v4("255.255.255.255"), 32 }, ip_address_v4_type_Broadcast                 }  //Reserved for the "limited broadcast" destination address.
};

//TODO check
inline bool is_ip_in_range(const ip_address_v4& _ip_, const network_v4& _range_)
{
    //ip_network_v4 buffer(_ip_, 32);
    //return buffer.is_subnet_of(_range_);
    constexpr uint8_t number_of_bits = 32;
    std::bitset<128>   _ip_bits_;
    std::bitset<128> _mask_bits_;
    for (uint8_t i = 0; i < number_of_bits; i++)
    {
          _ip_bits_[i] = _ip_             .to_bytes()[i / 8] & (0b10000000 >> (i % 8));
        _mask_bits_[i] = _range_.address().to_bytes()[i / 8] & (0b10000000 >> (i % 8));
    }
    for (uint8_t i = 0; i < _range_.prefix_length(); i++)
        if (_ip_bits_[i] != _mask_bits_[i])
            return false;
    return true;
}
//TODO check
inline bool is_ip_in_range(const ip_address_v6& _ip_, const network_v6& _range_)
{
    //ip_network_v6 buffer(_ip_, 128);
    //return buffer.is_subnet_of(_range_);
    constexpr uint8_t number_of_bits = 128;
    std::bitset<128>   _ip_bits_;
    std::bitset<128> _mask_bits_;
    for (uint8_t i = 0; i < number_of_bits; i++)
    {
          _ip_bits_[i] = _ip_             .to_bytes()[i / 8] & (0b10000000 >> (i % 8));
        _mask_bits_[i] = _range_.address().to_bytes()[i / 8] & (0b10000000 >> (i % 8));
    }
    for (uint8_t i = 0; i < _range_.prefix_length(); i++)
        if (_ip_bits_[i] != _mask_bits_[i])
            return false;
    return true;
}
inline std::optional<ip_address_v4_type> get_ip_type(const ip_address_v4& _ip_)
{
    for (size_t i = 0; i < sizeof(IPv4_ranges) / sizeof(decltype(IPv4_ranges[0])); i++)
        if (is_ip_in_range(_ip_, IPv4_ranges[i].first))
            return IPv4_ranges[i].second;
    return std::nullopt;
}
inline std::optional<ip_address_v6_type> get_ip_type(const ip_address_v6& _ip_)
{
    for (size_t i = 0; i < sizeof(IPv6_ranges) / sizeof(decltype(IPv6_ranges[0])); i++)
        if (is_ip_in_range(_ip_, IPv6_ranges[i].first))
            return IPv6_ranges[i].second;
    return std::nullopt;
}

// Useful class with ping data values
template<typename adress_t>
struct IP_info
{
public:
    IP_info() noexcept = default;
    IP_info(int16_t  _init_ping_                      ) noexcept : Ping(_init_ping_)                  {}
    IP_info(adress_t _init_IP_                        ) noexcept :                    IP(_init_IP_  ) {}
    IP_info(adress_t _init_IP_  , int16_t  _init_ping_) noexcept : Ping(_init_ping_), IP(_init_IP_  ) {}
    IP_info(int16_t  _init_ping_, adress_t _init_IP_  ) noexcept : Ping(_init_ping_), IP(_init_IP_  ) {}

    void clear_statistics() noexcept
    {
        Ping  = -5;
        Avg   =  0;
        Count =  0;
        Fails =  0;
    }
    
    adress_t get_ip   () const noexcept { return IP;    }
    int16_t  get_ping () const noexcept { return Ping;  }
    double   get_avg  () const noexcept { return Avg;   }
    size_t   get_count() const noexcept { return Count; }
    size_t   get_fails() const noexcept { return Fails; }
    double   get_loss () const noexcept { return Count ? (float)Fails / (float)Count * 100.f : 100.f; }

    void add_new_ping(const int16_t _value_)
    {
        Ping = _value_;
        if (Ping != -5)
        {
            if (Ping < 0)
                Fails++;
            else
                Avg = Avg * Count / double(Count + 1) + Ping / double(Count + 1);
            Count++;
        }
    }

    bool operator==(const IP_info& other) const noexcept { return IP == other.IP; }
    bool operator!=(const IP_info& other) const noexcept { return !(*this == other); }

    friend struct traceroute_tool_t;
    friend struct  edit_node_tool_t;

protected:
    adress_t IP;
    int16_t  Ping { -5 };
    double   Avg  {  0 };
    size_t   Count{  0 };
    size_t   Fails{  0 };
};

#endif // !IP_INFO_HPP
