#ifndef ARP_HPP
#define ARP_HPP

#include <backend_include.hpp>
#include "IPinfo.hpp"

struct Adapter_t
{
public:
    std::wstring Name;
    std::wstring Description;
    std::string Type;
    uint64_t    LUID { 0 };
    uint64_t    GUID { 0 };
    size_t      Index{ 0 };

    mac_address CurrentMAC;
    mac_address PermanentMAC;

    bool Connected{ false };
};

struct ARP_line_t
{
public:
    enum ARP_type_t : uint8_t
    {
        ARP_type_OTHER   = 1,
        ARP_type_INVALID = 2,
        ARP_type_DYNAMIC = 3,
        ARP_type_STATIC  = 4
    };
    static const char* type_to_str(const ARP_type_t& _type_)
    {
        switch (_type_)
        {
        case ARP_type_OTHER  : return "другой"      ; break;
        case ARP_type_INVALID: return "ошибка"      ; break;
        case ARP_type_DYNAMIC: return "динамический"; break;
        case ARP_type_STATIC : return "статический" ; break;
        default: throw "<ошибка>";
        }
    }

    ARP_line_t() noexcept = default;
    ARP_line_t(const mac_address& _mac_, const ip_address& _ip_, const ARP_type_t& _type_) noexcept :
        MAC(_mac_),
        IP(_ip_),
        Type(_type_)
    {}

    bool operator==(const ARP_line_t& _other_) const noexcept
    {
        return
            (this->MAC  == _other_.MAC ) and
            (this->IP   == _other_.IP  ) and
            (this->Type == _other_.Type);
    }
    bool operator!=(const ARP_line_t& _other_) const noexcept { return !((*this) == _other_); }
    bool operator< (const ARP_line_t& _other_) const noexcept
    {
        if (this->MAC < _other_.MAC)
            return true;
        else if (this->MAC == _other_.MAC)
        {
            if (this->IP < _other_.IP)
                return true;
            else if(this->IP == _other_.IP)
            {
                if (this->Type < _other_.Type)
                    return true;
            }
        }
        return false;
    }
    bool operator> (const ARP_line_t& _other_) const noexcept
    {
        if (this->MAC > _other_.MAC)
            return true;
        else if (this->MAC == _other_.MAC)
        {
            if (this->IP > _other_.IP)
                return true;
            else if (this->IP == _other_.IP)
            {
                if (this->Type > _other_.Type)
                    return true;
            }
        }
        return false;
    }

    friend struct ARP_table_t;

private:
    mac_address MAC;
     ip_address IP;
    ARP_type_t  Type;
};
struct ARP_table_t
{
public:
    bool update();
    void clear() noexcept { table.clear(); }

    IPs_t  get_IPs_by_MAC(const mac_address& _mac_) const noexcept
    {
        IPs_t ret;
        for (auto& line : table)
            if (line.MAC == _mac_)
                ret.insert(line.IP);
        return ret;
    }
    MACs_t get_MACs_by_IP(const ip_address & _ip_ ) const noexcept
    {
        MACs_t ret;
        for (auto& line : table)
            if (line.IP == _ip_)
                ret.insert(line.MAC);
        return ret;
    }

private:
    std::set<ARP_line_t> table;
} ARP_table;

#if defined UTILIB_WINDOWS or defined UTILIB_WINDOWS_64
#include <Iphlpapi.h>
#include <tchar.h>
#pragma comment (lib, "Iphlpapi.lib")

bool ARP_table_t::update()
{
    MIB_IPNETTABLE* net_table    = (MIB_IPNETTABLE*)malloc(0xFFFF);
    ULONG           size_pointer = 0xFFFF;
    ULONG           ret2         = GetIpNetTable(net_table, &size_pointer, TRUE);
    switch (ret2)
    {
    case NO_ERROR                 : INF_SYS_S("All is ok"                ); break;
    case ERROR_INSUFFICIENT_BUFFER: ERR_SYS_S("ERROR_INSUFFICIENT_BUFFER"); break;
    case ERROR_INVALID_PARAMETER  : ERR_SYS_S("ERROR_INVALID_PARAMETER"  ); break;
    case ERROR_NO_DATA            : ERR_SYS_S("ERROR_NO_DATA"            ); break;
    case ERROR_NOT_SUPPORTED      : ERR_SYS_S("ERROR_NOT_SUPPORTED"      ); break;
    }
    for (DWORD i = 0; i < net_table->dwNumEntries; i++)
    {
        ip_address_v4  ip_buf(ip_address_v4::bytes_type {
                uint8_t((net_table->table[i].dwAddr & 0x000000FF) >>  0),
                uint8_t((net_table->table[i].dwAddr & 0x0000FF00) >>  8),
                uint8_t((net_table->table[i].dwAddr & 0x00FF0000) >> 16),
                uint8_t((net_table->table[i].dwAddr & 0xFF000000) >> 24)
            });
        mac_address   mac_buf(mac_address  ::bytes_type {
                net_table->table[i].bPhysAddr[0],
                net_table->table[i].bPhysAddr[1],
                net_table->table[i].bPhysAddr[2],
                net_table->table[i].bPhysAddr[3],
                net_table->table[i].bPhysAddr[4],
                net_table->table[i].bPhysAddr[5]
            });
        table.insert({ mac_buf, ip_buf, static_cast<ARP_line_t::ARP_type_t>(net_table->table[i].dwType) });
    }
    free(net_table);

    return true;
}

void interfaces_update1()
{
    ULONG Family = AF_UNSPEC;
    ULONG Flags  = GAA_FLAG_INCLUDE_PREFIX             |
                   GAA_FLAG_INCLUDE_WINS_INFO          |
                   GAA_FLAG_INCLUDE_GATEWAYS           |
                   GAA_FLAG_INCLUDE_ALL_INTERFACES     |
                   GAA_FLAG_INCLUDE_ALL_COMPARTMENTS   |
                   GAA_FLAG_INCLUDE_TUNNEL_BINDINGORDER;
    ULONG Size   = 0xFFFF;
    IP_ADAPTER_ADDRESSES* AdapterAddresses = (IP_ADAPTER_ADDRESSES*)malloc(Size);
    ULONG ret = GetAdaptersAddresses(Family, Flags, NULL, AdapterAddresses, &Size);
    switch (ret)
    {
    case NO_ERROR                    : INF_SYS_S("NO_ERROR"                    ); break;
    case ERROR_ADDRESS_NOT_ASSOCIATED: ERR_SYS_S("ERROR_ADDRESS_NOT_ASSOCIATED"); break;
    case ERROR_BUFFER_OVERFLOW       : ERR_SYS_S("ERROR_BUFFER_OVERFLOW"       ); break;
    case ERROR_INVALID_PARAMETER     : ERR_SYS_S("ERROR_INVALID_PARAMETER"     ); break;
    case ERROR_NOT_ENOUGH_MEMORY     : ERR_SYS_S("ERROR_NOT_ENOUGH_MEMORY"     ); break;
    case ERROR_NO_DATA               : ERR_SYS_S("ERROR_NO_DATA"               ); break;
    default                          : ERR_SYS_S("ERROR_UNKNOWN"               ); break;
    }
    size_t i = 0;
    while (AdapterAddresses)
    {
        std::cout  << i << '\t' << AdapterAddresses->AdapterName << '\t';
        std::wcout << AdapterAddresses->FriendlyName << '\t' << AdapterAddresses->Description << '\t' << AdapterAddresses->DnsSuffix;
        std::cout << std::endl;
        AdapterAddresses = AdapterAddresses->Next;
        i++;
    }
}

bool interfaces_update2()
{
    PMIB_IF_TABLE2 table = NULL;
    ULONG          ret1  = GetIfTable2(&table);
    switch (ret1)
    {
    case NO_ERROR                 : break;
    case ERROR_INSUFFICIENT_BUFFER: ERR_SYS("ERROR_INSUFFICIENT_BUFFER"); return false;
    case ERROR_INVALID_PARAMETER  : ERR_SYS("ERROR_INVALID_PARAMETER"  ); return false;
    case ERROR_NOT_SUPPORTED      : ERR_SYS("ERROR_NOT_SUPPORTED"      ); return false;
    default                       : ERR_SYS("ERROR_UNKNOWN"            ); return false;
    }
    for (ULONG i = 0; i < table->NumEntries; i++)
    {
        Adapter_t new_adapter;
        std::wstring buffer;
        //buffer                  = std::wstring(table->Table[i].Alias);
        //new_adapter.Name        = std::string (buffer.begin(), buffer.end());
        //buffer                  = std::wstring(table->Table[i].Description);
        //new_adapter.Description = std::string (buffer.begin(), buffer.end());
        new_adapter.Name        = std::wstring(table->Table[i].Alias);
        new_adapter.Description = std::wstring(table->Table[i].Description);
        new_adapter.Type;
        new_adapter.LUID        = *((uint64_t*)&table->Table[i].InterfaceLuid);
        new_adapter.GUID        = *((uint64_t*)&table->Table[i].InterfaceGuid);
        new_adapter.Index       = table->Table[i].InterfaceIndex;

        if (table->Table[i].PhysicalAddressLength == 6)
        {
            auto& _data1_ = table->Table[i].PhysicalAddress;
            new_adapter.CurrentMAC   = mac_address{ mac_address::bytes_type{ _data1_[0], _data1_[1], _data1_[2], _data1_[3], _data1_[4], _data1_[5] } };
            auto& _data2_ = table->Table[i].PermanentPhysicalAddress;
            new_adapter.PermanentMAC = mac_address{ mac_address::bytes_type{ _data2_[0], _data2_[1], _data2_[2], _data2_[3], _data2_[4], _data2_[5] } };
        }

        new_adapter.Connected =
            (table->Table[i].OperStatus == IfOperStatusUp || table->Table[i].OperStatus == IfOperStatusDormant) &&
             table->Table[i].AdminStatus       == NET_IF_ADMIN_STATUS_UP &&
             table->Table[i].MediaConnectState == MediaConnectStateConnected;

        std::wcout << new_adapter.Name.c_str() << new_adapter.Description.c_str() << std::endl;
    }
    FreeMibTable(table);

    return true;
}

#else
    //TODO
#endif

#endif ARP_HPP //  !ARP_LIB_HPP