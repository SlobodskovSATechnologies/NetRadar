#ifndef LINK_HPP
#define LINK_HPP

#include <backend_include.hpp>
#include <utilib/si.hpp>

using namespace Utils::si;

enum Link_type : uint8_t
{
    Link_type_Unknown = 0,

    Link_type_Copper,
    Link_type_Optic,
    Link_type_WiFi,
    Link_type_Radio,
    Link_type_Virtual,
    Link_type_VPN,

    Link_type_Count
};
inline const char* type_to_str(const Link_type& _type_)
{
    switch (_type_)
    {
    case Link_type_Unknown: return "Не указан";

    case Link_type_Copper : return "Медный кабель";
    case Link_type_Optic  : return "Оптика";
    case Link_type_WiFi   : return "WiFi";
    case Link_type_Radio  : return "Радиоканал";
    case Link_type_Virtual: return "Виртуальный";
    case Link_type_VPN    : return "VPN";

    default: return "<ОШИБКА>";
    }
}

enum Link_copper_type : uint8_t
{
    Link_copper_type_Unknown = 0,
    
    Link_copper_type_CAT3,   // 10/100 Mb/s
    Link_copper_type_CAT5,   // 1  Gb/s
    Link_copper_type_CAT5e,
    Link_copper_type_CAT6,   // 10 Gb/s
    Link_copper_type_CAT6A,
    Link_copper_type_CAT7,
    Link_copper_type_CAT7A,
    Link_copper_type_CAT8,   // 40 Gb/s
    Link_copper_type_CAT8_1,
    Link_copper_type_CAT8_2,

    Link_copper_type_Count
};
enum Link_shield_type : uint8_t
{
    Link_shield_type_Unknown = 0,
    
    //U  = unshielded, без экрана
    //F  = foil, фольга
    //S  = braided screening, оплётка из проволоки (только внешний экран)
    //TP = twisted pair, витая пара

    Link_shield_type_U_UTP, //нет экрана
    
    Link_shield_type_U_FTP, //индивидуальный экран
    
    Link_shield_type_F_UTP, //общий экран
    Link_shield_type_S_UTP,
    Link_shield_type_SF_UTP,

    Link_shield_type_F_FTP, //индивидуальный и общий экран
    Link_shield_type_S_FTP,
    Link_shield_type_SF_FTP,

    Link_shield_type_Count
};
inline const char* type_to_str(const Link_copper_type& _type_)
{
    switch (_type_)
    {
    case Link_copper_type_Unknown: return "Не указан";

    case Link_copper_type_CAT3  : return "CAT3";
    case Link_copper_type_CAT5  : return "CAT5";
    case Link_copper_type_CAT5e : return "CAT5e";
    case Link_copper_type_CAT6  : return "CAT6";
    case Link_copper_type_CAT6A : return "CAT6A";
    case Link_copper_type_CAT7  : return "CAT7";
    case Link_copper_type_CAT7A : return "CAT7A";
    case Link_copper_type_CAT8  : return "CAT8";
    case Link_copper_type_CAT8_1: return "CAT8.1";
    case Link_copper_type_CAT8_2: return "CAT8.2";

    default: return "<ОШИБКА>";
    }
}
inline const char* type_to_str(const Link_shield_type& _type_)
{
    switch (_type_)
    {
    case Link_shield_type_Unknown: return "Не указан";

    case Link_shield_type_U_UTP: return "U/UTP";

    case Link_shield_type_U_FTP: return "U/FTP";
    
    case Link_shield_type_F_UTP : return "F/UTP";
    case Link_shield_type_S_UTP : return "S/UTP";
    case Link_shield_type_SF_UTP: return "SF/UTP";

    case Link_shield_type_F_FTP : return "F/FTP";
    case Link_shield_type_S_FTP : return "S/FTP";
    case Link_shield_type_SF_FTP: return "SF/FTP";
    
    default: return "<ОШИБКА>";
    }
}

enum Link_optic_connector_type : uint8_t
{
    Link_optic_connector_type_Unknown = 0,

    //Тип разъёма
    Link_optic_connector_type_SC,
    Link_optic_connector_type_LC,
    Link_optic_connector_type_FC,
    Link_optic_connector_type_ST,

    Link_optic_connector_type_Count
};
enum Link_optic_polishing_type : uint8_t
{
    Link_optic_polishing_type_Unknown = 0,

    //Полировка
    Link_optic_polishing_type_PC,
    Link_optic_polishing_type_SPC,
    Link_optic_polishing_type_UPC,
    Link_optic_polishing_type_APC,

    Link_optic_polishing_type_Count
};
enum Link_optic_mod_type : uint8_t
{
    Link_optic_mod_type_Unknown = 0,
    
    //Модовость
    Link_optic_mod_type_OS,
    Link_optic_mod_type_OM,

    Link_optic_mod_type_Count
};
enum Link_optic_line_type : uint8_t
{
    Link_optic_line_type_Unknown = 0,
    
    //Кол-во линий
    Link_optic_line_type_Simplex,
    Link_optic_line_type_Duplex,

    Link_optic_line_type_Count
};
inline const char* type_to_str(const Link_optic_connector_type& _type_)
{
    switch (_type_)
    {
    case Link_optic_connector_type_Unknown: return "Не указан";

    case Link_optic_connector_type_SC: return "SC";
    case Link_optic_connector_type_LC: return "LC";
    case Link_optic_connector_type_FC: return "FC";
    case Link_optic_connector_type_ST: return "ST";

    default: return "<ОШИБКА>";
    }
}
inline const char* type_to_str(const Link_optic_polishing_type& _type_)
{
    switch (_type_)
    {
    case Link_optic_polishing_type_Unknown: return "Не указан";

    case Link_optic_polishing_type_PC : return "PC";
    case Link_optic_polishing_type_SPC: return "SPC";
    case Link_optic_polishing_type_UPC: return "UPC";
    case Link_optic_polishing_type_APC: return "APC";

    default: return "<ОШИБКА>";
    }
}
inline const char* type_to_str(const Link_optic_mod_type      & _type_)
{
    switch (_type_)
    {
    case Link_optic_mod_type_Unknown: return "Не указан";

    case Link_optic_mod_type_OS: return "Одномодовое";
    case Link_optic_mod_type_OM: return "Многомодовое";

    default: return "<ОШИБКА>";
    }
}
inline const char* type_to_str(const Link_optic_line_type     & _type_)
{
    switch (_type_)
    {
    case Link_optic_line_type_Unknown: return "Не указан";

    case Link_optic_line_type_Simplex: return "Симплекс";
    case Link_optic_line_type_Duplex : return "Дуплекс";

    default: return "<ОШИБКА>";
    }
}

enum Link_WiFi_type
{
    Link_WiFi_type_Unknown = 0,

    Link_WiFi_type_2_4G,
    Link_WiFi_type_5G,
    Link_WiFi_type_6G,
    
    Link_WiFi_type_Count
};
inline const char* type_to_str(const Link_WiFi_type& _type_)
{
    switch (_type_)
    {
    case Link_WiFi_type_Unknown: return "Не указан";

    case Link_WiFi_type_2_4G: return "2.4 GHz";
    case Link_WiFi_type_5G  : return "5 GHz";
    case Link_WiFi_type_6G  : return "6 GHz";

    default: return "<ОШИБКА>";
    }
}

enum Link_Radio_type
{
    Link_Radio_type_Unknown = 0,

    Link_Radio_type_900M,
    Link_Radio_type_2_4G,
    Link_Radio_type_5G,
    Link_Radio_type_6G,
    Link_Radio_type_24_60G,

    Link_Radio_type_Count
};
inline const char* type_to_str(const Link_Radio_type& _type_)
{
    switch (_type_)
    {
    case Link_Radio_type_Unknown: return "Не указан";

    case Link_Radio_type_900M  : return "900 MHz";
    case Link_Radio_type_2_4G  : return "2.4 GHz";
    case Link_Radio_type_5G    : return "5 GHz";
    case Link_Radio_type_6G    : return "6 GHz";
    case Link_Radio_type_24_60G: return "24-60 GHz";

    default: return "<ОШИБКА>";
    }
}

struct Link_t
{
public:
    Link_t() noexcept = default;
    Link_t(int _left_id_, int _right_id_) noexcept :
         left_id(_left_id_),
        right_id(_right_id_)
    {}
    Link_t(int _left_id_, int _right_id_   , float _len_) noexcept :
         left_id(_left_id_),
        right_id(_right_id_),
        len(_len_)
    {}
    Link_t(const std::pair<int, int>& _ids_) noexcept :
         left_id(_ids_.first ),
        right_id(_ids_.second)
    {}
    Link_t(const std::pair<int, int>& _ids_, float _len_) noexcept :
         left_id(_ids_.first ),
        right_id(_ids_.second),
        len(_len_)
    {}

    [[nodiscard]] json to_json() const
    {
        json ret;
        
        ret[ "left_id"] =  left_id;
        ret["right_id"] = right_id;
        ret["len"     ] = (float)len;
        ret["label"   ] = label;

        ret["Type"] = (int)Type;
        
        ret["Copper"]["CatType"   ] = (int)Copper.CatType;
        ret["Copper"]["ShieldType"] = (int)Copper.ShieldType;
        
        ret["OpticLeft" ]["Connector"] = (int)OpticLeft.Connector;
        ret["OpticLeft" ]["Line"     ] = (int)OpticLeft.Line;
        ret["OpticLeft" ]["Mod"      ] = (int)OpticLeft.Mod;
        ret["OpticLeft" ]["Polishing"] = (int)OpticLeft.Polishing;

        ret["OpticRight"]["Connector"] = (int)OpticRight.Connector;
        ret["OpticRight"]["Line"     ] = (int)OpticRight.Line;
        ret["OpticRight"]["Mod"      ] = (int)OpticRight.Mod;
        ret["OpticRight"]["Polishing"] = (int)OpticRight.Polishing;
        
        ret["WiFi"]["Type"] = (int)WiFi.Type;
        ret["WiFi"]["freq"] = (int)WiFi.freq;
        
        ret["Radio"]["Type"] = (int)Radio.Type;
        ret["Radio"]["freq"] = (int)Radio.freq;

        return ret;
    }
    [[nodiscard]] bool from_json(json& j)
    {
        bool ret = false;

        if (j.contains( "left_id") && j[ "left_id"].is_number_integer()) {  left_id = j[ "left_id"].get<int        >(); ret = true; }
        if (j.contains("right_id") && j["right_id"].is_number_integer()) { right_id = j["right_id"].get<int        >(); ret = true; }
        if (j.contains("len"     ) && j["len"     ].is_number_float  ()) { len      = j["len"     ].get<float      >(); ret = true; }
        if (j.contains("label"   ) && j["label"   ].is_string        ()) { label    = j["label"   ].get<std::string>(); ret = true; }

        if (j.contains("Type") && j["Type"].is_number_integer()) 
        { 
            Type = static_cast<Link_type>(j["Type"].get<int>()); 
            ret = true; 
        }
        if (j.contains("Copper"))
        {
            if (j["Copper"].contains(   "CatType") && j["Copper"][   "CatType"].is_number_integer())
            {
                Copper.CatType = static_cast<Link_copper_type>(j["Copper"]["CatType"].get<int>());
                ret = true;
            }
            if (j["Copper"].contains("ShieldType") && j["Copper"]["ShieldType"].is_number_integer())
            {
                Copper.ShieldType = static_cast<Link_shield_type>(j["Copper"]["ShieldType"].get<int>());
                ret = true;
            }
        }
        if (j.contains("OpticLeft" ))
        {
            if (j["OpticLeft"].contains("Connector") && j["OpticLeft"]["Connector"].is_number_integer())
            {
                OpticLeft.Connector = static_cast<Link_optic_connector_type>(j["OpticLeft"]["Connector"].get<int>());
                ret = true;
            }
            if (j["OpticLeft"].contains("Line"     ) && j["OpticLeft"]["Line"     ].is_number_integer())
            {
                OpticLeft.Line = static_cast<Link_optic_line_type>(j["OpticLeft"]["Line"].get<int>());
                ret = true;
            }
            if (j["OpticLeft"].contains("Mod"      ) && j["OpticLeft"]["Mod"      ].is_number_integer())
            {
                OpticLeft.Mod = static_cast<Link_optic_mod_type>(j["OpticLeft"]["Mod"].get<int>());
                ret = true;
            }
            if (j["OpticLeft"].contains("Polishing") && j["OpticLeft"]["Polishing"].is_number_integer())
            {
                OpticLeft.Polishing = static_cast<Link_optic_polishing_type>(j["OpticLeft"]["Polishing"].get<int>());
                ret = true;
            }
        }
        if (j.contains("OpticRight"))
        {
            if (j["OpticRight"].contains("Connector") && j["OpticRight"]["Connector"].is_number_integer())
            {
                OpticRight.Connector = static_cast<Link_optic_connector_type>(j["OpticRight"]["Connector"].get<int>());
                ret = true;
            }
            if (j["OpticRight"].contains("Line"     ) && j["OpticRight"]["Line"     ].is_number_integer())
            {
                OpticRight.Line = static_cast<Link_optic_line_type>(j["OpticRight"]["Line"].get<int>());
                ret = true;
            }
            if (j["OpticRight"].contains("Mod"      ) && j["OpticRight"]["Mod"      ].is_number_integer())
            {
                OpticRight.Mod = static_cast<Link_optic_mod_type>(j["OpticRight"]["Mod"].get<int>());
                ret = true;
            }
            if (j["OpticRight"].contains("Polishing") && j["OpticRight"]["Polishing"].is_number_integer())
            {
                OpticRight.Polishing = static_cast<Link_optic_polishing_type>(j["OpticRight"]["Polishing"].get<int>());
                ret = true;
            }
        }
        if (j.contains("WiFi"))
        {
            if (j["WiFi"].contains("Type") && j["WiFi"]["Type"].is_number_integer())
            {
                WiFi.Type = static_cast<Link_WiFi_type>(j["WiFi"]["Type"].get<int>());
                ret = true;
            }
            if (j["WiFi"].contains("freq") && j["WiFi"]["freq"].is_number_integer())
            {
                WiFi.freq = j["WiFi"]["freq"].get<int>();
                ret = true;
            }
        }
        if (j.contains("Radio"))
        {
            if (j["Radio"].contains("Type") && j["Radio"]["Type"].is_number_integer())
            {
                Radio.Type = static_cast<Link_Radio_type>(j["Radio"]["Type"].get<int>());
                ret = true;
            }
            if (j["Radio"].contains("freq") && j["Radio"]["freq"].is_number_integer())
            {
                Radio.freq = j["Radio"]["freq"].get<int>();
                ret = true;
            }
        }

        return ret;
    }

    int  left_id{ 0 };
    int right_id{ 0 };

    float len { 0 };
    std::string label;

    Link_type Type{ Link_type_Unknown};

    struct Optic_type
    {
        Link_optic_connector_type Connector{ Link_optic_connector_type_Unknown };
        Link_optic_polishing_type Polishing{ Link_optic_polishing_type_Unknown };
        Link_optic_mod_type       Mod      { Link_optic_mod_type_Unknown       };
        Link_optic_line_type      Line     { Link_optic_line_type_Unknown      };

        operator bool() const noexcept { return Connector || Polishing || Mod || Line; }
    } OpticLeft, OpticRight;
    struct Copper_type
    {
        Link_copper_type CatType   { Link_copper_type_Unknown };
        Link_shield_type ShieldType{ Link_shield_type_Unknown };

        operator bool() const noexcept { return CatType || ShieldType; }
    } Copper;
    struct WiFi_type
    {
        Link_WiFi_type Type{ Link_WiFi_type_Unknown };
        int freq{ 0 };

        operator bool() const noexcept { return Type || freq; }
    } WiFi;
    struct Radio_type
    {
        Link_Radio_type Type{ Link_Radio_type_Unknown };
        int freq{ 0 };
        
        operator bool() const noexcept { return Type || freq; }
    } Radio;

private:
    int id{ 0 };
};

#endif // !LINK_HPP
