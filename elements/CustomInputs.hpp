#ifndef CUSTOM_INPUTS_HPP
#define CUSTOM_INPUTS_HPP

#include < backend_include.hpp>
#include <frontlib_include.hpp>

inline bool InputIPv4(const char* _label_, ip_address_v4& _ip_ )
{

    static std::map<std::string, std::string> ips; // label / ip
    if (ips.find(_label_) == ips.end()) ips[_label_] = _ip_.to_string();

    boost::system::error_code ec;
    ip_address_v4 ip_buf = boost::asio::ip::make_address_v4(ips[_label_], ec);
    bool invalid_color = bool(ec) || ip_buf == boost::asio::ip::make_address_v4("0.0.0.0");

    if (invalid_color)
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    if (ImGui::InputText((std::string(_label_) + "##IPv4str").c_str(), &ips[_label_]))
    {
        if (invalid_color)
            ImGui::PopStyleColor();

        ip_address_v4 ip_buf = boost::asio::ip::make_address_v4(ips[_label_], ec);
        bool invalid_input = bool(ec) || ip_buf == boost::asio::ip::make_address_v4("0.0.0.0");

        if (invalid_input)
            return false;
        _ip_ = ip_buf;
        return true;
    }
    else if (invalid_color)
    {
        ImGui::PopStyleColor();
        return false;
    }
    else
    {
        _ip_ = ip_buf;
        return false;
    }
}
inline bool InputIPv6(const char* _label_, ip_address_v6& _ip_ )
{
    static std::map<std::string, std::string> ips; // label / ip
    if (ips.find(_label_) == ips.end()) ips[_label_] = _ip_.to_string();

    boost::system::error_code ec;
    auto ip_buf = boost::asio::ip::make_address_v6(ips[_label_], ec);
    bool invalid_color = bool(ec) || ip_buf == boost::asio::ip::make_address_v6("::");

    if (invalid_color)
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    if (ImGui::InputText((std::string(_label_) + "##IPv6str").c_str(), &ips[_label_]))
    {
        if (invalid_color)
            ImGui::PopStyleColor();

        auto ip_buf = boost::asio::ip::make_address_v6(ips[_label_], ec);
        bool invalid_input = bool(ec) || ip_buf == boost::asio::ip::make_address_v6("::");

        if (invalid_input)
            return false;
        _ip_ = ip_buf;
        return true;
    }
    else if (invalid_color)
    {
        ImGui::PopStyleColor();
        return false;
    }
    else
    {
        _ip_ = ip_buf;
        return false;
    }
}
inline bool InputIP  (const char* _label_, ip_address   & _ip_ )
{
    static std::map<std::string, std::string> ips; // label / ip
    if (ips.find(_label_) == ips.end()) ips[_label_] = _ip_.to_string();

    boost::system::error_code ec4, ec6;
    ip_address_v4 ip_buf_4 = boost::asio::ip::make_address_v4(ips[_label_], ec4);
    ip_address_v6 ip_buf_6 = boost::asio::ip::make_address_v6(ips[_label_], ec6);
    bool invalid_color_4 = bool(ec4) || ip_buf_4 == boost::asio::ip::make_address_v4("0.0.0.0");
    bool invalid_color_6 = bool(ec6) || ip_buf_6 == boost::asio::ip::make_address_v6("::");

    if (invalid_color_4 && invalid_color_6)
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    if (ImGui::InputText((std::string(_label_) + "##IPv4/6str").c_str(), &ips[_label_]))
    {
        if (invalid_color_4 && invalid_color_6)
            ImGui::PopStyleColor();

        ip_address_v4 ip_buf_4 = boost::asio::ip::make_address_v4(ips[_label_], ec4);
        ip_address_v6 ip_buf_6 = boost::asio::ip::make_address_v6(ips[_label_], ec6);
        bool invalid_input_4 = bool(ec4) || ip_buf_4 == boost::asio::ip::make_address_v4("0.0.0.0");
        bool invalid_input_6 = bool(ec6) || ip_buf_6 == boost::asio::ip::make_address_v6("::");

        if (invalid_input_4 && invalid_input_6)
            return false;
        else if (invalid_input_4)
            _ip_ = ip_buf_6;
        else if (invalid_input_6)
            _ip_ = ip_buf_4;
        else
            assert("unknown error");

        return true;
    }
    else if (invalid_color_4 && invalid_color_6)
    {
        ImGui::PopStyleColor();
        return false;
    }
    else if (invalid_color_4)
    {
        _ip_ = ip_buf_6;
        return false;
    }
    else if (invalid_color_6)
    {
        _ip_ = ip_buf_4;
        return false;
    }
    else
    {
        assert("unknown error");
        return false;
    }
}
inline bool InputMAC (const char* _label_, mac_address  & _mac_)
{
    static std::map<std::string, std::string> macs; // label / ip
    if (macs.find(_label_) == macs.end()) macs[_label_] = _mac_.to_string();

    boost::system::error_code ec;
    auto mac_buf = boost::asio::ip::make_mac(macs[_label_], ec);
    bool invalid_color = bool(ec);

    if (invalid_color)
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    if (ImGui::InputText((std::string(_label_) + "##MACstr").c_str(), &macs[_label_]))
    {
        if (invalid_color)
            ImGui::PopStyleColor();

        auto ip_buf = boost::asio::ip::make_mac(macs[_label_], ec);
        bool invalid_input = bool(ec);

        if (invalid_input)
            return false;
        _mac_ = ip_buf;
        return true;
    }
    else if (invalid_color)
    {
        ImGui::PopStyleColor();
        return false;
    }
    else
    {
        _mac_ = mac_buf;
        return false;
    }
}

#endif // !CUSTOM_INPUTS_HPP
