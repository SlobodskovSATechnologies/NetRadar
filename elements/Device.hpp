#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <backend_include.hpp>
#include "IPinfo.hpp"

enum Device_RJ45_port_type : uint8_t
{
    Device_RJ45_port_type_Unknown = 0,

    Device_RJ45_port_type_RJ45_10M_100M,
    Device_RJ45_port_type_RJ45_1G,
    Device_RJ45_port_type_RJ45_2_5G,
    Device_RJ45_port_type_RJ45_5G,
    Device_RJ45_port_type_RJ45_10G,
    Device_RJ45_port_type_RJ45_25G,
    Device_RJ45_port_type_RJ45_40G,

    Device_RJ45_port_type_Count
};
inline const char* type_to_str(const Device_RJ45_port_type& _type_) noexcept
{
    switch (_type_)
    {
    case Device_RJ45_port_type_Unknown: return "Не указан";

    case Device_RJ45_port_type_RJ45_10M_100M: return "10M/100M";
    case Device_RJ45_port_type_RJ45_1G      : return "1G";
    case Device_RJ45_port_type_RJ45_2_5G    : return "2.5G";
    case Device_RJ45_port_type_RJ45_5G      : return "5G";
    case Device_RJ45_port_type_RJ45_10G     : return "10G";
    case Device_RJ45_port_type_RJ45_25G     : return "25G";
    case Device_RJ45_port_type_RJ45_40G     : return "40G";

    default: return "<ОШИБКА>";
    }
}

enum Device_SFP_port_type : uint8_t
{
    Device_SFP_port_type_Unknown = 0,

    Device_SFP_port_type_SFP_100M,
    Device_SFP_port_type_SFP_1G,
    Device_SFP_port_type_SFP_PLUS_10G,
    Device_SFP_port_type_SFP28_25G,
    Device_SFP_port_type_SFP56_50G,
    Device_SFP_port_type_QSFP_PLUS_40G,
    Device_SFP_port_type_QSFP28_50G,
    Device_SFP_port_type_QSFP28_100G,
    Device_SFP_port_type_QSFP56_200G,
    Device_SFP_port_type_QSFP_DD_400G,

    Device_SFP_port_type_Count
};
inline const char* type_to_str(const Device_SFP_port_type& _type_)
{
    switch (_type_)
    {
    case Device_SFP_port_type_Unknown: return "Не указан";

    case Device_SFP_port_type_SFP_100M     : return "SFP 100M";
    case Device_SFP_port_type_SFP_1G       : return "SFP 1G";
    case Device_SFP_port_type_SFP_PLUS_10G : return "SFP+ 10G";
    case Device_SFP_port_type_SFP28_25G    : return "SFP28 25G";
    case Device_SFP_port_type_SFP56_50G    : return "SFP56 50G";
    case Device_SFP_port_type_QSFP_PLUS_40G: return "QSFP+ 40G";
    case Device_SFP_port_type_QSFP28_50G   : return "QSFP28 50G";
    case Device_SFP_port_type_QSFP28_100G  : return "QSFP28 100G";
    case Device_SFP_port_type_QSFP56_200G  : return "QSFP56 200G";
    case Device_SFP_port_type_QSFP_DD_400G : return "QSFP DD 400G";

    default: return "<ОШИБКА>";
    }
}

enum Device_WiFi_port_type
{
    Device_WiFi_port_type_Unknown = 0,

    //                                          2.4   5   6
    Device_WiFi_port_type_802_11    , //WiFi 1    X
    Device_WiFi_port_type_802_11b   , //WiFi 2    X
    Device_WiFi_port_type_802_11a   , //WiFi 2G       X
    Device_WiFi_port_type_802_11g   , //WiFi 3    X
    Device_WiFi_port_type_802_11n   , //WiFi 4    X   X
    Device_WiFi_port_type_802_11ac  , //WiFi 5        X
    Device_WiFi_port_type_802_11ax  , //WiFi 6    X   X
                                      //WiFi 6E   X   X   X
    Device_WiFi_port_type_802_11be  , //WiFi 7    X   X   X
    Device_WiFi_port_type_802_11bn  , //WiFi 8    X   X   X

    Device_WiFi_port_type_Count
};
inline const char* type_to_str(const Device_WiFi_port_type& _type_)
{
    switch (_type_)
    {
    case Device_WiFi_port_type_Unknown: return "Не указан";

    case Device_WiFi_port_type_802_11  : return "WiFi 1";
    case Device_WiFi_port_type_802_11b : return "WiFi 2 (b)";
    case Device_WiFi_port_type_802_11a : return "WiFi 2G (a)";
    case Device_WiFi_port_type_802_11g : return "WiFi 3 (g)";
    case Device_WiFi_port_type_802_11n : return "WiFi 4 (n)";
    case Device_WiFi_port_type_802_11ac: return "WiFi 5 (ac)";
    case Device_WiFi_port_type_802_11ax: return "WiFi 6/6E (ax)";

    case Device_WiFi_port_type_802_11be: return "WiFi 7 (be)";
    case Device_WiFi_port_type_802_11bn: return "WiFi 8 (bn)";

    default: return "<ОШИБКА>";
    }
}

enum Device_port_type : uint8_t
{
    Device_port_type_Unknown = 0,

    Device_port_type_RJ45,
    Device_port_type_SFP,
    Device_port_type_WiFi,
    Device_port_type_Radio,
    Device_port_type_Virtual,
    Device_port_type_VPN,

    Device_port_type_Count
};
inline const char* type_to_str(const Device_port_type& _type_)
{
    switch (_type_)
    {
    case Device_port_type_Unknown: return "Не указан";

    case Device_port_type_RJ45   : return "RJ45";
    case Device_port_type_SFP    : return "SFP";
    case Device_port_type_WiFi   : return "WiFi";
    case Device_port_type_Radio  : return "Радиомост";
    case Device_port_type_Virtual: return "Виртуальный";
    case Device_port_type_VPN    : return "VPN";

    default: return "<ОШИБКА>";
    }
}

struct Device_port_t
{
public:
    Device_port_t() noexcept = default;
    Device_port_t(const std::string&      _name_                                ) : Name(_name_)               {}
    Device_port_t(                                const Device_port_type& _type_) :               Type(_type_) {}
    Device_port_t(const std::string&      _name_, const Device_port_type& _type_) : Name(_name_), Type(_type_) {}
    Device_port_t(const Device_port_type& _type_, const std::string&      _name_) : Name(_name_), Type(_type_) {}

    [[nodiscard]] json to_json() const
    {
        json ret;

        ret["Name"     ] = Name;
        ret[     "Type"] = (int)     Type;
        ret["RJ45_Type"] = (int)RJ45_Type;
        ret[ "SFP_Type"] = (int) SFP_Type;
        ret["WiFi_Type"] = (int)WiFi_Type;

        return ret;
    }
    [[nodiscard]] bool from_json(json& j)
    {
        bool ret = false;

        if (j.contains("Name"     ) && j["Name"     ].is_string        ()) { Name      =                                    j["Name"     ].get<std::string>() ; ret = true; }
        if (j.contains(     "Type") && j[     "Type"].is_number_integer()) {      Type = static_cast<Device_port_type     >(j[     "Type"].get<int        >()); ret = true; }
        if (j.contains("RJ45_Type") && j["RJ45_Type"].is_number_integer()) { RJ45_Type = static_cast<Device_RJ45_port_type>(j["RJ45_Type"].get<int        >()); ret = true; }
        if (j.contains( "SFP_Type") && j[ "SFP_Type"].is_number_integer()) {  SFP_Type = static_cast<Device_SFP_port_type >(j[ "SFP_Type"].get<int        >()); ret = true; }
        if (j.contains("WiFi_Type") && j["WiFi_Type"].is_number_integer()) { WiFi_Type = static_cast<Device_WiFi_port_type>(j["WiFi_Type"].get<int        >()); ret = true; }
        
        return ret;
    }

    bool operator==(const Device_port_t& _other_) const noexcept { return Name == _other_.Name && Type == _other_.Type; }
    bool operator!=(const Device_port_t& _other_) const noexcept { return !(*this == _other_); }

    std::string           Name;
    Device_port_type           Type{ Device_port_type_Unknown      };
    Device_RJ45_port_type RJ45_Type{ Device_RJ45_port_type_Unknown };
    Device_SFP_port_type   SFP_Type{ Device_SFP_port_type_Unknown  };
    Device_WiFi_port_type WiFi_Type{ Device_WiFi_port_type_Unknown };
};

enum Device_type : uint8_t
{
    Device_type_Unknown = 0,

    Device_type_Phone,
    Device_type_Tab,
    Device_type_PC,
    Device_type_Notebook,
    Device_type_Server,
    Device_type_SmartHome,

    Device_type_Switch,
    Device_type_Router,
    Device_type_WiFi_accsess_point,
    Device_type_RadioBridge,
    Device_type_NAS,
    Device_type_PatchPanel,
    Device_type_Converter,

    Device_type_VM,

    Device_type_Other,

    Device_type_Count
};
inline const char* type_to_str(const Device_type& _type_)
{
    switch (_type_)
    {
    case Device_type_Unknown           : return "Не указан";

    case Device_type_Phone             : return "Телефон";
    case Device_type_Tab               : return "Планшет";
    case Device_type_PC                : return "Компьютер";
    case Device_type_Notebook          : return "Ноутбук";
    case Device_type_Server            : return "Сервер";
    case Device_type_SmartHome         : return "Умный дом";

    case Device_type_Switch            : return "Коммутатор";
    case Device_type_Router            : return "Роутер";
    case Device_type_WiFi_accsess_point: return "Точка доступа WiFi";
    case Device_type_RadioBridge       : return "Радиомост";
    case Device_type_NAS               : return "NAS хранилище";
    case Device_type_PatchPanel        : return "Патч панель";
    case Device_type_Converter         : return "Переходник";

    case Device_type_VM                : return "Виртуальная машина";

    case Device_type_Other             : return "Другое";
    
    default: return "<ОШИБКА>";
    }
}

struct Device_t
{
public:
    Device_t() noexcept = default;
    Device_t(int _id_) noexcept :
        id(_id_)
    {}
    Device_t(std::initializer_list<ip_address_v4> _IPv4_, std::initializer_list<ip_address_v6> _IPv6_, std::initializer_list<mac_address> _MAC_, const std::string& _Nickname_, bool _CanBeDeleted_, bool _NeedToPing_, std::initializer_list<Device_port_t> _Inputs_, std::initializer_list<Device_port_t> _Outputs_) noexcept :
        MAC         (_MAC_), 
        Nickname    (_Nickname_),
        CanBeDeleted(_CanBeDeleted_), 
        NeedToPing  (_NeedToPing_), 
        Inputs      (_Inputs_), 
        Outputs     (_Outputs_)
    {
        for (const auto& IP_v4 : _IPv4_) IPsV4.emplace_back(IP_v4);
        for (const auto& IP_v6 : _IPv6_) IPsV6.emplace_back(IP_v6);
    }
    template<Utils::container_of<ip_address_v4> Container1, Utils::container_of<ip_address_v6> Container2, Utils::container_of<mac_address> Container3>
    Device_t(const Container1& _IPv4_, const Container2& _IPv6_, const Container3& _MAC_, const std::string& _Nickname_, bool _CanBeDeleted_, bool _NeedToPing_, std::initializer_list<Device_port_t> _Inputs_, std::initializer_list<Device_port_t> _Outputs_) noexcept :
        MAC         (_MAC_), 
        Nickname    (_Nickname_),
        CanBeDeleted(_CanBeDeleted_), 
        NeedToPing  (_NeedToPing_), 
        Inputs      (_Inputs_), 
        Outputs     (_Outputs_)
    {
        for (const auto& IP_v4 : _IPv4_) IPsV4.emplace_back(IP_v4);
        for (const auto& IP_v6 : _IPv6_) IPsV6.emplace_back(IP_v6);
    }    
    template<Utils::container_of<ip_address_v4> Container1, Utils::container_of<ip_address_v6> Container2>
    Device_t(const Container1& _IPv4_, const Container2& _IPv6_, std::initializer_list<mac_address> _MAC_, const std::string& _Nickname_, bool _CanBeDeleted_, bool _NeedToPing_, std::initializer_list<Device_port_t> _Inputs_, std::initializer_list<Device_port_t> _Outputs_) noexcept :
        MAC         (_MAC_), 
        Nickname    (_Nickname_),
        CanBeDeleted(_CanBeDeleted_), 
        NeedToPing  (_NeedToPing_), 
        Inputs      (_Inputs_ ), 
        Outputs     (_Outputs_)
    {
        for (const auto& IP_v4 : _IPv4_) IPsV4.emplace_back(IP_v4);
        for (const auto& IP_v6 : _IPv6_) IPsV6.emplace_back(IP_v6);
    }

    [[nodiscard]] json to_json() const
    {
		json j;
        j["IPv4"] = json::array();
        for (const auto& ip : IPsV4)
            j["IPv4"].push_back(ip.get_ip().to_string().c_str());
        j["IPv6"] = json::array();
        for (const auto& ip : IPsV6)
            j["IPv6"].push_back(ip.get_ip().to_string());
        j["MAC"] = json::array();
        for (const auto& mac_addr : MAC)
            j["MAC"].push_back(mac_addr.to_string().c_str());
        j["Nickname"]   = Nickname.c_str();
        j["NeedToPing"] = NeedToPing;
        j["NAT"]        = NAT;
        j["Inputs"] = json::array();
        for (const auto& input  : Inputs ) j["Inputs" ].push_back(input .to_json());
        j["Outputs"] = json::array();
        for (const auto& output : Outputs) j["Outputs"].push_back(output.to_json());
        j["id"] = id;
		return j;
    }
    [[nodiscard]] bool from_json(json& j)
    {
        bool ret = false;

        if (j.contains("Nickname") && j["Nickname"].is_string()) { Nickname = j["Nickname"].get<std::string>(); ret = true; }
        if (j.contains("IPv4") && j["IPv4"].is_array())
        {
            IPsV4.clear();
            for (const auto& ip_str : j["IPv4"])
                if (ip_str.is_string()) { IPsV4.push_back(boost::asio::ip::make_address_v4(ip_str.get<std::string>())); ret = true; }
        }
        if (j.contains("IPv6") && j["IPv6"].is_array())
        {
            IPsV6.clear();
            for (const auto& ip_str : j["IPv6"])
                if (ip_str.is_string()) { IPsV6.push_back(boost::asio::ip::make_address_v6(ip_str.get<std::string>())); ret = true; }
        }
        if (j.contains("MAC" ) && j["MAC" ].is_array())
        {
            MAC.clear();
            boost::system::error_code ec;
            for (const auto& mac_str : j["MAC"])
                if (mac_str.is_string()) { MAC.push_back(boost::asio::ip::make_mac(mac_str.get<std::string>(), ec)); ret = true; }
        }
        CanBeDeleted = true;
        if (j.contains("NeedToPing") && j["NeedToPing"].is_boolean()) { NeedToPing = j["NeedToPing"].get<bool>(); ret = true; }
        if (j.contains("NAT"       ) && j["NAT"       ].is_boolean()) { NAT        = j["NAT"       ].get<bool>(); ret = true; }
        if (j.contains("Inputs" ) && j["Inputs" ].is_array())
        {
            Inputs.clear();
            for (const auto& input_json : j["Inputs"])
            {
                Device_port_t input;
                if (bool valid = input.from_json(const_cast<json&>(input_json))) { Inputs.push_back(input); ret = true; }
            }
        }
        if (j.contains("Outputs") && j["Outputs"].is_array())
        {
            Outputs.clear();
            for (const auto& output_json : j["Outputs"])
            {
                Device_port_t output;
                if (bool valid = output.from_json(const_cast<json&>(output_json))) { Outputs.push_back(output); ret = true; }
            }
        }
        if (j.contains("id") && j["id"].is_number_integer()) { id = j["id"].get<int >(); ret = true; }

        return ret;
    }

	void set_id(int _id_) { id = _id_; }
	int  get_id() const { return id; }

    void clear_statistics_v4() { for (auto& IPv4_ : IPsV4) IPv4_.clear_statistics(); }
    void clear_statistics_v6() { for (auto& IPv6_ : IPsV6) IPv6_.clear_statistics(); }
    void clear_statistics() { clear_statistics_v4(); clear_statistics_v6(); }

    int16_t get_min_ping_v4() const noexcept
    {
        const auto ret = std::min_element(IPsV4.begin(), IPsV4.end(), [](const IP_info<ip_address_v4>& _first_, const IP_info<ip_address_v4>& _second_)
            { return _first_.get_ping() < _second_.get_ping(); });
        if (ret == IPsV4.end())
            return 0;
        else
            return ret->get_ping();
    }
    int16_t get_min_ping_v6() const noexcept
    {
        const auto ret = std::min_element(IPsV6.begin(), IPsV6.end(), [](const IP_info<ip_address_v6>& _first_, const IP_info<ip_address_v6>& _second_)
            { return _first_.get_ping() < _second_.get_ping(); });
        if (ret == IPsV6.end())
            return 0;
        else
            return ret->get_ping();
    }
    int16_t get_max_ping_v4() const noexcept
    {
        const auto ret = std::max_element(IPsV4.begin(), IPsV4.end(), [](const IP_info<ip_address_v4>& _first_, const IP_info<ip_address_v4>& _second_)
            { return _first_.get_ping() < _second_.get_ping(); });
        if (ret == IPsV4.end())
            return 0;
        else
            return ret->get_ping();
    }
    int16_t get_max_ping_v6() const noexcept
    {
        const auto ret = std::max_element(IPsV6.begin(), IPsV6.end(), [](const IP_info<ip_address_v6>& _first_, const IP_info<ip_address_v6>& _second_)
            { return _first_.get_ping() < _second_.get_ping(); });
        if (ret == IPsV6.end())
            return 0;
        else
            return ret->get_ping();
    }
    std::pair<int16_t, int16_t> get_minmax_ping_v4() const noexcept
    {
        const auto ret = std::minmax_element(IPsV4.begin(), IPsV4.end(), [](const IP_info<ip_address_v4>& _first_, const IP_info<ip_address_v4>& _second_)
            { return _first_.get_ping() < _second_.get_ping(); });
        if (ret.first == IPsV4.end() || ret.second == IPsV4.end())
            return std::make_pair(0, 0);
        else
            return std::make_pair(ret.first->get_ping(), ret.second->get_ping());
    }
    std::pair<int16_t, int16_t> get_minmax_ping_v6() const noexcept
    {
        const auto ret = std::minmax_element(IPsV6.begin(), IPsV6.end(), [](const IP_info<ip_address_v6>& _first_, const IP_info<ip_address_v6>& _second_)
            { return _first_.get_ping() < _second_.get_ping(); });
        if (ret.first == IPsV6.end() || ret.second == IPsV6.end())
            return std::make_pair(0, 0);
        else
            return std::make_pair(ret.first->get_ping(), ret.second->get_ping());
    }

    bool operator==(const Device_t& _other_) const noexcept = default;
    bool operator!=(const Device_t& _other_) const noexcept { return !(*this == _other_); }

    std::vector<IP_info< ip_address_v4>> IPsV4;
    std::vector<IP_info< ip_address_v6>> IPsV6;
    std::vector<        mac_address    > MAC;
    std::string Nickname    { ""    };
    bool        CanBeDeleted{ true  };
    bool        NeedToPing  { true  };
    bool        NAT         { false };
    std::vector<Device_port_t> Outputs;
    std::vector<Device_port_t> Inputs;
    Device_type Type{ Device_type::Device_type_Unknown };

private:
    int id{ 0 };
};

#endif // !DEVICE_HPP