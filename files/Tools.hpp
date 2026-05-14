#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <frontlib_include.hpp>
#include <frontlib/input.hpp>
#include "CustomInputs.hpp"
#include "Device.hpp"

struct traceroute_tool_t
{
public:
    void show()
    {
        static Device_t new_target;
        static bool use_rDNS = false;
        ImGui::Text("Трассировка %s", target.Nickname.c_str());
        ImGui::Separator();

        if (target == Device_t{})
        {
            if (new_target.IPsV4.size() != 1)
                new_target.IPsV4.resize(1);
            InputIPv4("IPv4 для трассировки", new_target.IPsV4[0].IP);
            new_target.Nickname = new_target.IPsV4[0].IP.to_string();
            ImGui::Checkbox("разыменовывать IP до доменов (rDNS)", &use_rDNS);
            if (ImGui::Button("Закрыть", ImVec2(120, 0)) || ImGui::IsKeyPressed(ImGuiKey_Escape, false))
            {
                result.clear();
                target       = Device_t{};
                new_target   = Device_t{};
                show_popup   = false;
                result_ready = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Начать", ImVec2(120,0 )) || ImGui::IsKeyPressed(ImGuiKey_Enter, false))
            {
                result.clear();
                result_ready = false;
                target       = new_target;
                if (use_rDNS)
                    std::thread(&traceroute_tool_t::do_traceroute_with_rDNS, this).detach();
                else
                    std::thread(&traceroute_tool_t::do_traceroute          , this).detach();
            }
        }
        else if (!result_ready)
            ImGui::Text("Загрузка...");
        else
        {
            size_t j = 0;
            for (auto& hops_ : result)
            {
                size_t i = 0;
                for (auto& hop_ : hops_)
                {
                    //TODO : add IPv6 support (currently only IPv4 is displayed, and IPv6 hops are ignored)

                    ImGui::Text(("#" + std::to_string(i + 1)).c_str()); ImGui::SameLine(50);
                    std::string buf = hop_.IPv4 == ip_address_v4() ? "неизвестный узел" : hop_.IPv4.to_string();

                    if (hop_.IPv4 == ip_address_v4()) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 0.8f));
                    ImGui::Text(buf.c_str()); ImGui::SameLine(200);
                    if (hop_.IPv4 == ip_address_v4()) ImGui::PopStyleColor();

                    ImGui::Text((std::to_string(hop_.ping) + " ms").c_str());

                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 0.5f));
                    for (auto& domain_ : hop_.domains)
                        ImGui::Text("\t\t%s", domain_.data());
                    ImGui::PopStyleColor();

                    i++;
                }
                auto res = std::find_if(hops_.begin(), hops_.end(), [j, this](const trace_t::value_type& hop) { return hop.IPv4 == this->target.IPsV4[j].IP; }) != hops_.end();
                if (!res)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                    ImGui::Text("ЦЕЛЬ НЕ ДОСТИГНУТА");
                }
                else
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
                    ImGui::Text("ЦЕЛЬ ДОСТИГНУТА");
                }
                ImGui::PopStyleColor();
                ImGui::Separator();
                j++;
            }
            if (ImGui::Button("Закрыть", ImVec2(120, 0)))
            {
                result.clear();
                target       = Device_t{};
                new_target   = Device_t{};
                show_popup   = false;
                result_ready = false;
                ImGui::CloseCurrentPopup();
            }
        }
    }

    void do_traceroute()
    {
        //TODO: Add IPv6 traceroute support
        for (size_t i = 0; i < target.IPsV4.size(); i++)
            result.push_back(Connections::Ethernet::Utils::traceroute_v4(target.IPsV4[i].IP, false, 1000, 30));
        result_ready = true;
    }
    void do_traceroute_with_rDNS()
    {
        //TODO: Add IPv6 traceroute support
        for (size_t i = 0; i < target.IPsV4.size(); i++)
            result.push_back(Connections::Ethernet::Utils::traceroute_v4(target.IPsV4[i].IP, true, 1000, 30));
        result_ready = true;
    }

    std::atomic_bool     show_popup   = false;
    std::atomic_bool     result_ready = false;
    std::vector<trace_t> result;
    Device_t             target;
};
struct  edit_node_tool_t
{
public:
    void show(const int _id_ = -1)
    {
        Device_t buffer;
        if (_id_ != -1)
        {
            if (DevicesList.is_id_vacant(_id_))
            {
                ERR_SYS_S("Node is not founded");
                ImGui::CloseCurrentPopup();
                return;
            }
            buffer = DevicesList.get_device_by_id(_id_);
        }
        else
            buffer = Device_t{ DevicesList.get_vacant_id() };

        static Device_t editing_device = buffer;
        if (editing_device.get_id() != buffer.get_id()) editing_device = buffer;

        const float avail_width = ImGui::GetContentRegionAvail().x - 20;

        ImGui::InputText("Имя", &editing_device.Nickname);

        Frontlib::ShowComboList(("Тип устройства##" + std::to_string(editing_device.get_id())).c_str(), Device_type_Unknown, Device_type_Count, &editing_device.Type,
            [](const Device_type& _type_) { return type_to_str(_type_); });

        ImGui::Checkbox("Тут есть NAT", &editing_device.NAT);

        ImGui::SeparatorText("IP v4");
		size_t IPv4_size = editing_device.IPsV4.size();
        if (Frontlib::InputClamped("Кол-во##v4", &IPv4_size, (size_t)0, (size_t)100, (size_t)1, (size_t)10))
            editing_device.IPsV4.resize(IPv4_size);
        for (size_t i = 0; i < editing_device.IPsV4.size(); i++)
            InputIPv4(("#" + std::to_string(i + 1) + "##" + std::to_string(editing_device.get_id())).c_str(), editing_device.IPsV4[i].IP);

        ImGui::SeparatorText("IP v6");
        size_t IPv6_size = editing_device.IPsV6.size();
        if (Frontlib::InputClamped("Кол-во##v6", &IPv6_size, (size_t)0, (size_t)100, (size_t)1, (size_t)10))
            editing_device.IPsV6.resize(IPv6_size);
        for (size_t i = 0; i < editing_device.IPsV6.size(); i++)
            InputIPv6(("#" + std::to_string(i + 1) + "##" + std::to_string(editing_device.get_id())).c_str(), editing_device.IPsV6[i].IP);

        ImGui::SeparatorText("MAC");
        size_t MAC_size = editing_device.MAC.size();
        if (Frontlib::InputClamped("Кол-во##mac", &MAC_size, (size_t)0, (size_t)100, (size_t)1, (size_t)10)) editing_device.MAC.resize(MAC_size);
        for (size_t i = 0; i < editing_device.MAC.size(); i++)
            InputMAC(("#" + std::to_string(i + 1) + "##" + std::to_string(editing_device.get_id())).c_str(), editing_device.MAC[i]);

        if (ImGui::BeginTable("Коммутация", 2, ImGuiTableFlags_SizingFixedSame))
        {
            ImGui::TableSetupColumn("Входы");
            ImGui::TableSetupColumn("Выходы");
            ImGui::TableHeadersRow();

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);

            size_t inputs_size = editing_device.Inputs.size();
            if (Frontlib::InputClamped("Кол-во##inputs", &inputs_size, (size_t)0, (size_t)100, (size_t)1, (size_t)10)) editing_device.Inputs.resize(inputs_size);
            for (size_t i = 0; i < editing_device.Inputs.size(); i++)
            {
                ImGui::InputText(("#" + std::to_string(i + 1) + "##in" + std::to_string(editing_device.get_id())).c_str(), &editing_device.Inputs[i].Name);
                Frontlib::ShowComboList(("тип##" + std::to_string(i + 1) + "in" + std::to_string(editing_device.get_id())).c_str(), Device_port_type_Unknown, Device_port_type_Count, &editing_device.Inputs[i].Type,
                    [](const Device_port_type& _type_) { return type_to_str(_type_); });
                if (editing_device.Inputs[i].Type == Device_port_type_RJ45)
                    Frontlib::ShowComboList(("подтип##" + std::to_string(i + 1) + "in" + std::to_string(editing_device.get_id())).c_str(), Device_RJ45_port_type_Unknown, Device_RJ45_port_type_Count, &editing_device.Inputs[i].RJ45_Type,
                        [](const Device_RJ45_port_type& _type_) { return type_to_str(_type_); });
                else if (editing_device.Inputs[i].Type == Device_port_type_SFP)
                    Frontlib::ShowComboList(("подтип##" + std::to_string(i + 1) + "in" + std::to_string(editing_device.get_id())).c_str(), Device_SFP_port_type_Unknown, Device_SFP_port_type_Count, &editing_device.Inputs[i].SFP_Type,
                        [](const Device_SFP_port_type& _type_) { return type_to_str(_type_); });
                else if (editing_device.Inputs[i].Type == Device_port_type_WiFi)
                    Frontlib::ShowComboList(("подтип##" + std::to_string(i + 1) + "in" + std::to_string(editing_device.get_id())).c_str(), Device_WiFi_port_type_Unknown, Device_WiFi_port_type_Count, &editing_device.Inputs[i].WiFi_Type,
                        [](const Device_WiFi_port_type& _type_) { return type_to_str(_type_); });
            }

            ImGui::TableNextColumn();

            size_t outputs_size = editing_device.Outputs.size();
            if (Frontlib::InputClamped("Кол-во##outputs", &outputs_size, (size_t)0, (size_t)100, (size_t)1, (size_t)10)) editing_device.Outputs.resize(outputs_size);
            for (size_t i = 0; i < editing_device.Outputs.size(); i++)
            {
                ImGui::InputText(("#" + std::to_string(i + 1) + "##out" + std::to_string(editing_device.get_id())).c_str(), &editing_device.Outputs[i].Name);
                Frontlib::ShowComboList(("тип##" + std::to_string(i + 1) + "out" + std::to_string(editing_device.get_id())).c_str(), Device_port_type_Unknown, Device_port_type_Count, &editing_device.Outputs[i].Type,
                    [](const Device_port_type& _type_) { return type_to_str(_type_); });
                if (editing_device.Outputs[i].Type == Device_port_type_RJ45)
                    Frontlib::ShowComboList(("подтип##" + std::to_string(i + 1) + "out" + std::to_string(editing_device.get_id())).c_str(), Device_RJ45_port_type_Unknown, Device_RJ45_port_type_Count, &editing_device.Outputs[i].RJ45_Type,
                        [](const Device_RJ45_port_type& _type_) { return type_to_str(_type_); });
                else if (editing_device.Outputs[i].Type == Device_port_type_SFP)
                    Frontlib::ShowComboList(("подтип##" + std::to_string(i + 1) + "out" + std::to_string(editing_device.get_id())).c_str(), Device_SFP_port_type_Unknown, Device_SFP_port_type_Count, &editing_device.Outputs[i].SFP_Type,
                        [](const Device_SFP_port_type& _type_) { return type_to_str(_type_); });
                else if (editing_device.Outputs[i].Type == Device_port_type_WiFi)
                    Frontlib::ShowComboList(("подтип##" + std::to_string(i + 1) + "out" + std::to_string(editing_device.get_id())).c_str(), Device_WiFi_port_type_Unknown, Device_WiFi_port_type_Count, &editing_device.Outputs[i].WiFi_Type,
                        [](const Device_WiFi_port_type& _type_) { return type_to_str(_type_); });
            }

            ImGui::EndTable();
        }

        if (ImGui::Button("Отменить" , ImVec2(avail_width / 2, 0)))
            ImGui::CloseCurrentPopup();
        ImGui::SameLine();
        if (ImGui::Button("Сохранить", ImVec2(avail_width / 2, 0)))
        {
            if (_id_ == -1)
                DevicesList.push_back(editing_device);
            else
                DevicesList.refresh(editing_device); 
            ImGui::CloseCurrentPopup(); 
        }
    }

    std::atomic_bool show_popup_new  = false;
    std::atomic_bool show_popup_edit = false;
};
struct        DNS_tool_t
{
public:
    void show()
    {
        static std::string new_target;
        static bool use_rDNS = false;
        ImGui::Text("DNS %s", target.c_str());
        ImGui::Separator();

        if (target.empty())
        {
            ImGui::InputText("домен", &new_target);
            ImGui::SameLine();
            if (ImGui::Button("Закрыть", ImVec2(120, 0)) || ImGui::IsKeyPressed(ImGuiKey_Escape, false))
            {
                result.clear();
                target.clear();
                new_target.clear();
                show_popup   = false;
                result_ready = false;
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::Button("Начать", ImVec2(120, 0)) || ImGui::IsKeyPressed(ImGuiKey_Enter, false))
            {
                result.clear();
                result_ready = false;
                target = new_target;
                std::thread(&DNS_tool_t::do_DNS, this).detach();
            }
        }
        else if (!result_ready)
            ImGui::Text("Загрузка...");
        else
        {
            size_t i = 0;
            for (auto& ip_ : result)
            {
                ImGui::Text(("#" + std::to_string(i + 1)).c_str()); ImGui::SameLine(50);
                ImGui::Text(ip_.to_string().c_str());
                i++;
            }
            if (ImGui::Button("Закрыть", ImVec2(120, 0)))
            {
                result    .clear();
                target    .clear();
                new_target.clear();
                show_popup   = false;
                result_ready = false;
                ImGui::CloseCurrentPopup();
            }
        }
    }

    void do_DNS()
    {
        result = DNS(target);
        result_ready = true;
    }

    std::atomic_bool show_popup = false;
    std::atomic_bool result_ready = false;
    IPs_t            result;
    std::string      target;
};
struct       rDNS_tool_t
{
public:
    void show()
    {
        static ip_address_v4 new_target;
        static bool use_rDNS = false;
        ImGui::Text("rDNS %s", target.to_string().c_str());
        ImGui::Separator();

        if (target == ip_address{})
        {
            InputIPv4("IPv4", new_target);
            if (ImGui::Button("Закрыть", ImVec2(120, 0)) || ImGui::IsKeyPressed(ImGuiKey_Escape, false))
            {
                result.clear();
                target       = ip_address_v4{};
                new_target   = ip_address_v4{};
                show_popup   = false;
                result_ready = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Начать", ImVec2(120, 0)) || ImGui::IsKeyPressed(ImGuiKey_Enter, false))
            {
                result.clear();
                result_ready = false;
                target       = new_target;
                std::thread(&rDNS_tool_t::do_rDNS, this).detach();
            }
        }
        else if (!result_ready)
            ImGui::Text("Загрузка...");
        else
        {
            size_t i = 0;
            for (auto& domain_ : result)
            {
                ImGui::Text(("#" + std::to_string(i + 1)).c_str()); ImGui::SameLine(50);
                ImGui::Text(domain_.c_str());
                i++;
            }
            if (ImGui::Button("Закрыть", ImVec2(120, 0)))
            {
                result.clear();
                target       = ip_address_v4{};
                new_target   = ip_address_v4{};
                show_popup   = false;
                result_ready = false;
                ImGui::CloseCurrentPopup();
            }
        }
    }

    void do_rDNS()
    {
        result = rDNS(target);
        result_ready = true;
    }

    std::atomic_bool show_popup = false;
    std::atomic_bool result_ready = false;
    domains_t        result;
    ip_address_v4    target;
};
struct  edit_link_tool_t
{
public:
    void show()
    {
        if (target < 0 || target >= DevicesList.links_size())
        {
            target = -1;
            ImGui::CloseCurrentPopup();
            return;
        }
        Link_t& link = DevicesList.get_link(target);

        const float avail_width = ImGui::GetContentRegionAvail().x;

        ImGui::InputText ("Маркировка", &link.label);
        ImGui::InputFloat("Длина [м]" , &link.len, 1, 100);
        
        switch (link.Type)
        {
        case Link_type_Copper :
        {
            Frontlib::ShowComboList("Категория", Link_copper_type_Unknown, Link_copper_type_Count, &link.Copper.CatType, [](const Link_copper_type& _type_) { return type_to_str(_type_); });
            if (link.Copper.CatType)
                Frontlib::ShowComboList("Экран", Link_shield_type_Unknown, Link_shield_type_Count, &link.Copper.ShieldType, [](const Link_shield_type& _type_) { return type_to_str(_type_); });
        } break;
        case Link_type_WiFi   :
        {
            Frontlib::ShowComboList("Поколение", Link_WiFi_type_Unknown, Link_WiFi_type_Count, &link.WiFi.Type, [](const Link_WiFi_type& _type_)
                { return type_to_str(_type_); });
            Frontlib::InputClamped("Частота [MHz]", &link.WiFi.freq, 2000, 8000, 1, 100);
        } break;
        case Link_type_Radio  :
        {
            Frontlib::ShowComboList("Диапазон", Link_Radio_type_Unknown, Link_Radio_type_Count, &link.Radio.Type, [](const Link_Radio_type& _type_)
                { return type_to_str(_type_); });
            Frontlib::InputClamped("Частота [MHz]", &link.Radio.freq, 2000, 8000, 1, 100);
        } break;
        case Link_type_Optic  :
        {
            if (ImGui::BeginTable("Порты оптики", 2, ImGuiTableFlags_SizingFixedSame, ImVec2{ avail_width, 0 }))
            {
                ImGui::TableSetupColumn("Левый порт" , 0, avail_width / 2);
                ImGui::TableSetupColumn("Правый порт", 0, avail_width / 2);
                ImGui::TableHeadersRow();

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                
                Frontlib::ShowComboList("Разъём##left"   , Link_optic_connector_type_Unknown, Link_optic_connector_type_Count, &link.OpticLeft.Connector, [](const Link_optic_connector_type& _type_)
                    { return type_to_str(_type_); });
                Frontlib::ShowComboList("Полировка##left", Link_optic_polishing_type_Unknown, Link_optic_polishing_type_Count, &link.OpticLeft.Polishing, [](const Link_optic_polishing_type& _type_)
                    { return type_to_str(_type_); });
                Frontlib::ShowComboList("Модовость##left", Link_optic_mod_type_Unknown      , Link_optic_mod_type_Count      , &link.OpticLeft.Mod      , [](const Link_optic_mod_type      & _type_)
                    { return type_to_str(_type_); });
                Frontlib::ShowComboList("Связь##left"    , Link_optic_line_type_Unknown     , Link_optic_line_type_Count     , &link.OpticLeft.Line     , [](const Link_optic_line_type     & _type_)
                    { return type_to_str(_type_); });

                ImGui::TableNextColumn();
                
                Frontlib::ShowComboList("Разъём##right"   , Link_optic_connector_type_Unknown, Link_optic_connector_type_Count, &link.OpticRight.Connector, [](const Link_optic_connector_type& _type_)
                    { return type_to_str(_type_); });
                Frontlib::ShowComboList("Полировка##right", Link_optic_polishing_type_Unknown, Link_optic_polishing_type_Count, &link.OpticRight.Polishing, [](const Link_optic_polishing_type& _type_)
                    { return type_to_str(_type_); });
                Frontlib::ShowComboList("Модовость##right", Link_optic_mod_type_Unknown      , Link_optic_mod_type_Count      , &link.OpticRight.Mod      , [](const Link_optic_mod_type      & _type_)
                    { return type_to_str(_type_); });
                Frontlib::ShowComboList("Связь##right"    , Link_optic_line_type_Unknown     , Link_optic_line_type_Count     , &link.OpticRight.Line     , [](const Link_optic_line_type     & _type_)
                    { return type_to_str(_type_); });

                ImGui::EndTable();
            }
        } break;
        case Link_type_Virtual: break;
        case Link_type_VPN    : break;

        case Link_type_Unknown: break;
        default: target = -1; ImGui::CloseCurrentPopup(); return;
        }

        if (ImGui::Button("Закрыть", ImVec2(avail_width, 0)))
        {
            target = -1;
            ImGui::CloseCurrentPopup();
        }
    }

    std::atomic_bool show_popup  { false };
    std::atomic_bool keys_pressed{ false };
    std::atomic_int  target      { -1 };
};
struct        WOL_tool_t
{
public:
    void show()
    {
        ImGui::Text("WOL (Wake On Lan)");
        ImGui::Separator();

        InputMAC("MAC", target);
        ImGui::Checkbox("UDP  протокол", & UDP_using);
        ImGui::Checkbox("ICMP протокол", &ICMP_using);
        if (UDP_using)
        {
            ImGui::SetNextItemWidth(128);
            Frontlib::InputClamped("UDP порт", &Port, (uint16_t)0, (uint16_t)65535, (uint16_t)1, (uint16_t)10, 0);
        }

        if (ImGui::Button("Закрыть", ImVec2(120, 0)) || ImGui::IsKeyPressed(ImGuiKey_Escape, false))
        {
            show_popup = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Начать", ImVec2(120, 0)) || ImGui::IsKeyPressed(ImGuiKey_Enter, false))
        {
            if (ICMP_using) Connections::Ethernet::Utils::WOL_ICMP(target);
            if ( UDP_using) Connections::Ethernet::Utils::WOL_UDP (target, Port);
            show_popup = false;
            ImGui::CloseCurrentPopup();
        }
    }

    bool              UDP_using{ true };
    bool             ICMP_using{ true };
    std::atomic_bool show_popup{ false };
    mac_address      target;
    uint16_t         Port      { 0 };
};

#endif // !TOOLS_HPP
