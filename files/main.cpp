#define COLOR_EN
#define ID_K 1000
#define LEGEND_NODE_ID (2147483647 - 10)

#define TRANSPONENT_K 0.75f

#define BACKGROUND_COLOR ImVec4(0.0625f, 0.0625f, 0.0625f, 1.f          )
#define      GREEN_COLOR ImVec4(0.1719f, 0.8359f, 0.f    , TRANSPONENT_K)
#define     YELLOW_COLOR ImVec4(1.f    , 0.8f   , 0.f    , TRANSPONENT_K)
#define        RED_COLOR ImVec4(0.9f   , 0.16f  , 0.0256f, TRANSPONENT_K)
#define       GRAY_COLOR ImVec4(0.5f   , 0.5f   , 0.5f   , 1.f          )

#define    RJ45_COLOR IM_COL32(108, 255, 115, uint8_t(TRANSPONENT_K * 255))
#define     SFP_COLOR IM_COL32(  0, 143,  22, uint8_t(TRANSPONENT_K * 255))
#define    WIFI_COLOR IM_COL32(185, 155, 255, uint8_t(TRANSPONENT_K * 255))
#define   RADIO_COLOR IM_COL32( 63,  66, 167, uint8_t(TRANSPONENT_K * 255))
#define VIRTUAL_COLOR IM_COL32(  0, 106, 255, uint8_t(TRANSPONENT_K * 255))
#define     VPN_COLOR IM_COL32(255,   0, 230, uint8_t(TRANSPONENT_K * 255))

#define    RJ45_COLOR_SELECTED    RJ45_COLOR | (0xFF << IM_COL32_A_SHIFT)
#define     SFP_COLOR_SELECTED     SFP_COLOR | (0xFF << IM_COL32_A_SHIFT)
#define    WIFI_COLOR_SELECTED    WIFI_COLOR | (0xFF << IM_COL32_A_SHIFT)
#define   RADIO_COLOR_SELECTED   RADIO_COLOR | (0xFF << IM_COL32_A_SHIFT)
#define VIRTUAL_COLOR_SELECTED VIRTUAL_COLOR | (0xFF << IM_COL32_A_SHIFT)
#define     VPN_COLOR_SELECTED     VPN_COLOR | (0xFF << IM_COL32_A_SHIFT)

//#include "Raw.hpp"
#include <backend_include.hpp>
#include <backend_loader.hpp>
#include <frontlib_include.hpp>
#include <frontlib_loader.hpp>
#include <utilib/si.hpp>
#include <utilib/Json.hpp>

using namespace Connections::Ethernet::Utils;
using nlohmann::json;

typedef boost::asio::ip::network_v4  ip_network_v4;
typedef boost::asio::ip::network_v6  ip_network_v6;

typedef boost::asio::ip::address_v4  ip_address_v4;
typedef boost::asio::ip::address_v6  ip_address_v6;
typedef boost::asio::ip::address     ip_address;

typedef boost::asio::ip::mac        mac_address;

#include "CustomInputs.hpp"
#include "Device.hpp"
#include "Link.hpp"
#include "DevicesList.hpp"
DevicesList_t DevicesList;
#include "Tools.hpp"
traceroute_tool_t traceroute_tool;
 edit_node_tool_t  edit_node_tool;
       DNS_tool_t        DNS_tool;
      rDNS_tool_t       rDNS_tool;
 edit_link_tool_t  edit_link_tool;
       WOL_tool_t        WOL_tool;
#include "ARP.hpp"

//Проверка на дебила, что кол-во типов связей равно кол-ву типов портов на нодах
static_assert(Link_type_Count == Device_port_type_Count);

//Функции перевода ENUMs в цвета и формы
static ImNodesPinShape_ translate     (const Device_port_type& _type_)
{
    IM_COL32(1, 2, 3, 4);
    switch (_type_)
    {
    case Device_port_type_Virtual:
    case Device_port_type_VPN    : return ImNodesPinShape_CircleFilled;
    case Device_port_type_SFP    :
    case Device_port_type_RJ45   : return ImNodesPinShape_TriangleFilled;
    case Device_port_type_WiFi   :
    case Device_port_type_Radio  : return ImNodesPinShape_QuadFilled;
    default: return ImNodesPinShape_CircleFilled;
    }
}
static ImU32            pin_color     (const Device_port_type& _type_)
{
    switch (_type_)
    {
    case Device_port_type_RJ45   : return    RJ45_COLOR;
    case Device_port_type_SFP    : return     SFP_COLOR;
    case Device_port_type_WiFi   : return    WIFI_COLOR;
    case Device_port_type_Radio  : return   RADIO_COLOR;
    case Device_port_type_Virtual: return VIRTUAL_COLOR;
    case Device_port_type_VPN    : return     VPN_COLOR;
    default: return IM_COL32(255, 255, 255, uint8_t(TRANSPONENT_K * 255));
    }
}
static ImU32            pin_color_bold(const Device_port_type& _type_)
{
    switch (_type_)
    {
    case Device_port_type_RJ45   : return    RJ45_COLOR_SELECTED;
    case Device_port_type_SFP    : return     SFP_COLOR_SELECTED;
    case Device_port_type_WiFi   : return    WIFI_COLOR_SELECTED;
    case Device_port_type_Radio  : return   RADIO_COLOR_SELECTED;
    case Device_port_type_Virtual: return VIRTUAL_COLOR_SELECTED;
    case Device_port_type_VPN    : return     VPN_COLOR_SELECTED;
    default: return IM_COL32(255, 255, 255, 255);
    }
}

//Для вкладки Настройки
bool Hide_IPv4s = false;
bool Hide_IPv6s = false;
bool Hide_MACs  = false;

static void show()
{
    static std::string file_name;

    ImNodes::PushStyleVar(ImNodesStyleVar_PinCircleRadius      ,  6);
    ImNodes::PushStyleVar(ImNodesStyleVar_PinQuadSideLength    , 10);
    ImNodes::PushStyleVar(ImNodesStyleVar_PinTriangleSideLength, 14);

    const bool shift_key{ ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift) };
    const bool  ctrl_key{ ImGui::IsKeyDown(ImGuiKey_LeftCtrl ) || ImGui::IsKeyDown(ImGuiKey_RightCtrl ) };
    const bool   alt_key{ ImGui::IsKeyDown(ImGuiKey_LeftAlt  ) || ImGui::IsKeyDown(ImGuiKey_RightAlt  ) };

    //Обновляем данные нашего компьютера
    //TODO: Add MAC address retrieval for local interfaces and use it here instead of empty vector
    //TODO: Автоматически добавлять интерфейсы и их типы
    DevicesList[0] = Device_t{ EthernetServices::getLocalIP_v4(), EthernetServices::getLocalIP_v6(), {}, "этот компьютер", false, false,
        {
            {"Eth"     , Device_port_type_RJ45   },
            {"Optic"   , Device_port_type_SFP    },
            {"WiFi 2.4", Device_port_type_WiFi   },
            {"WiFi 5"  , Device_port_type_WiFi   },
            {"VPN"     , Device_port_type_VPN    },
            {"Virtual" , Device_port_type_Virtual}
        }, {} };

    //Инициализация главного окна
    ImGui::SetNextWindowPos({ 0, 0 });
    ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);
    ImGui::Begin("main", 0, ImGuiWindowFlags_NoResize   |
                            ImGuiWindowFlags_NoMove     | 
                            ImGuiWindowFlags_NoCollapse | 
                            ImGuiWindowFlags_NoTitleBar | 
                            ImGuiWindowFlags_MenuBar);
	auto current_window = ImGui::GetCurrentWindow();
    
    //Меню сверху окна
	bool show_save_as_popup = false;
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Файл"       ))
        {
            if (ImGui::BeginMenu("Загрузить"))
            {
                struct PathTooltip {
                    void operator () (const std::filesystem::path& path) const {
                        using namespace std::chrono;

                        const auto sys_time = utc_clock::to_sys(file_clock::to_utc(last_write_time(path)));
                        const auto modified = time_point_cast<seconds>(current_zone()->to_local(sys_time));
                        const auto size = std::filesystem::file_size(path);

                        if (ImGui::BeginTable("info", 2))
                        {
                            ImGui::TableNextRow();

                            if (ImGui::TableSetColumnIndex(0))
                                ImGui::Text("Изменено");

                            if (ImGui::TableNextColumn())
                                ImGui::Text(std::format("{}", modified).c_str());

                            if (ImGui::TableNextColumn())
                                ImGui::Text("Размер");

                            if (ImGui::TableNextColumn())
                                ImGui::Text("%.2f KB", size / 1024.f);
                            
                            ImGui::EndTable();
                        }
                    }
                };

                static std::filesystem::path selected_path;
                if (Frontlib::ShowFiles("файлы", "./", &selected_path, ".cfg", true, true, PathTooltip{}))
                {
                    std::string grid_data;
					file_name = selected_path.filename().string();
					file_name.erase(file_name.find_last_of('.')); // Remove file extension
                    json j = DevicesList.load(grid_data, file_name);
                    ImNodes::LoadCurrentEditorStateFromIniString(grid_data.c_str(), grid_data.size());
                    Frontlib::PushWindowMessage(current_window, "Загружено", std::chrono::seconds(5), Frontlib::Colors::Lime);
                }
                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Сохранить", "Ctrl+S"))
            {
                if (file_name.empty()) show_save_as_popup = true;
                else
                {
                    std::string grid_data(ImNodes::SaveCurrentEditorStateToIniString());
                    DevicesList.save(grid_data.c_str(), grid_data.size(), file_name);
                    Frontlib::PushWindowMessage(current_window, "Сохранено", std::chrono::seconds(5), Frontlib::Colors::Lime);
                }
            }
            if (ImGui::MenuItem("Сохранить как", "Ctrl+Shift+S")) show_save_as_popup = true;
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Настройки"  ))
        {
            if (ImGui::BeginMenu("Вид"))
            {
                ImGui::Checkbox("Скрывать IPv4 на нодах", &Hide_IPv4s);
                ImGui::Checkbox("Скрывать IPv6 на нодах", &Hide_IPv6s);
                ImGui::Checkbox("Скрывать MAC  на нодах", &Hide_MACs );
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("WakeOnLan"))
            {
                ImGui::Checkbox("UDP  протокол", &WOL_tool. UDP_using);
                ImGui::Checkbox("ICMP протокол", &WOL_tool.ICMP_using);
                if (WOL_tool.UDP_using)
                {
                    ImGui::SetNextItemWidth(128);
                    Frontlib::InputClamped("UDP порт", &WOL_tool.Port, (uint16_t)0, (uint16_t)65535, (uint16_t)1, (uint16_t)10, 0);
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Ноды"       ))
        {
            if (ImGui::MenuItem("Новая нода",          "Ctrl+N")) { edit_node_tool.show_popup_new = true; }
            if (ImGui::MenuItem("Сбросить статистику", "Ctrl+R")) { DevicesList.clear_statistics(); }
            if (ImGui::MenuItem("Очистить все связи")) { DevicesList.links_clear  (); }
            if (ImGui::MenuItem("Удалить  все ноды" )) { DevicesList.devices_clear(); }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Инструменты"))
        {
            if (ImGui::MenuItem("Traceroute", "Ctrl+T"      , false, true )) { traceroute_tool.show_popup = true; }
            if (ImGui::MenuItem("DNS"       , "Ctrl+D"      , false, true )) {        DNS_tool.show_popup = true; }
            if (ImGui::MenuItem("rDNS"      , "Ctrl+Shift+D", false, true )) {       rDNS_tool.show_popup = true; }
            if (ImGui::MenuItem("WakeOnLan" , "Ctrl+W"      , false, true )) {        WOL_tool.show_popup = true; }
            if (ImGui::MenuItem("Анализ"    , "TODO"        , false, false)) { }
            ImGui::EndMenu();
		}
        if (ImGui::BeginMenu("Справка"    ))
        {
			if (ImGui::BeginMenu("IPv4"       )) { ImGui::Text("Протокол IPv4 (Internet Protocol version 4)\n\nЧетвёртая версия протокола IP, использующая 32-битные адреса.\nНа данный момент является наиболее распространённой версией IP в мире."                                                                                         ); 
                ImGui::TextLinkOpenURL("wiki", "https://ru.wikipedia.org/wiki/IPv4"); ImGui::EndMenu(); }
			if (ImGui::BeginMenu("IPv6"       )) { ImGui::Text("Протокол IPv6 (Internet Protocol version 6)\n\nШестая версия протокола IP, использующая 128-битные адреса.\nРазработан для замены IPv4 и решения проблемы исчерпания адресного пространства."                                                                                 ); 
                ImGui::TextLinkOpenURL("wiki", "https://ru.wikipedia.org/wiki/IPv6"); ImGui::EndMenu(); }
            if (ImGui::BeginMenu("MAC"        )) { ImGui::Text("MAC-адрес (Media Access Control address)\n\nУникальный идентификатор, присвоенный сетевому интерфейсу.\nИспользуется в качестве сетевого адреса в коммуникациях внутри сети."                                                                                                 ); 
                ImGui::TextLinkOpenURL("wiki", "https://ru.wikipedia.org/wiki/MAC-адрес"); ImGui::EndMenu(); }
			if (ImGui::BeginMenu("TCP"        )) { ImGui::Text("TCP (Transmission Control Protocol)\n\nНабор протоколов связи, используемых для межсетевого взаимодействия в интернете и других компьютерных сетях.\nTCP обеспечивает надёжную передачу данных,\nв то время как IP отвечает за маршрутизацию и адресацию пакетов данных."     ); 
                ImGui::TextLinkOpenURL("wiki", "https://ru.wikipedia.org/wiki/TCP"); ImGui::EndMenu(); }
			if (ImGui::BeginMenu("UDP"        )) { ImGui::Text("UDP (User Datagram Protocol)\n\nПротокол транспортного уровня, обеспечивающий передачу данных без установления соединения.\nОбеспечивает более быструю передачу данных по сравнению с TCP,\nно не гарантирует доставку и порядок доставки пакетов."                           ); 
                ImGui::TextLinkOpenURL("wiki", "https://ru.wikipedia.org/wiki/UDP"); ImGui::EndMenu(); }
			if (ImGui::BeginMenu("ICMP"       )) { ImGui::Text("ICMP (Internet Control Message Protocol)\n\nПротокол сетевого уровня, используемый для обмена сообщениями об\nошибках и диагностической информации в компьютерных сетях.\nЧасто используется для утилит, таких как ping и traceroute."                                        ); 
                ImGui::TextLinkOpenURL("wiki", "https://ru.wikipedia.org/wiki/ICMP"); ImGui::EndMenu(); }
            if (ImGui::BeginMenu("Ping"       )) { ImGui::Text("Ping\n\nУтилита для проверки доступности узла в сети и измерения времени отклика.\nРаботает на основе протокола ICMP,\nотправляя эхо-запросы и ожидая эхо-ответы от целевого узла."                                                                                           ); 
                ImGui::TextLinkOpenURL("wiki", "https://ru.wikipedia.org/wiki/Ping"); ImGui::EndMenu(); }
			if (ImGui::BeginMenu("Трассировка")) { ImGui::Text("Трассировка (traceroute)\n\nУтилита для определения маршрута,\nпо которому пакеты данных проходят от источника к назначению в сети.\nОтправляет серии пакетов с увеличивающимся временем жизни (TTL) и\nзаписывает адреса промежуточных узлов, через которые проходят пакеты."); 
                ImGui::TextLinkOpenURL("wiki", "https://ru.wikipedia.org/wiki/Traceroute"); ImGui::EndMenu(); }
			if (ImGui::BeginMenu("DNS"        )) { ImGui::Text("DNS (Domain Name System)\n\nСистема доменных имён,\nпереводит удобочитаемые доменные имена (например, www.example.com) в IP-адреса."                                                                                                                                          ); 
                ImGui::TextLinkOpenURL("wiki", "https://ru.wikipedia.org/wiki/DNS"); ImGui::EndMenu(); }
			if (ImGui::BeginMenu("rDNS"       )) { ImGui::Text("rDNS (reverse DNS)\n\nОбратное разрешение доменных имён\n(процесс определения доменного имени, связанного с IP-адресом)\nИспользуется для диагностики сети,\nфильтрации спама и обеспечения безопасности."                                                                    ); 
                ImGui::TextLinkOpenURL("wiki", "https://ru.wikipedia.org/wiki/Обратный_просмотр_DNS"); ImGui::EndMenu(); }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("О программе"))
        {
			ImGui::Text("Программа для визуализации компьютерных сетей и диагностики сетевых устройств.\n\nПозволяет создавать и редактировать ноды, отображающие устройства в сети,\nустанавливать связи между ними и выполнять сетевые инструменты, такие как ping и traceroute.\n\nРазработано с использованием библиотеки ImGui для интерфейса и nlohmann/json для сохранения данных.");
            ImGui::Text("Авторы: ");
            ImGui::TextLinkOpenURL("Моджахед    (backend)", "https://gitverse.ru/SSlob");
            ImGui::TextLinkOpenURL("NightPlanet (frontend)", "https://gitverse.ru/Night_Planet");
            ImGui::Text("Версия 1.0.0");
            ImGui::Text("Сборка от " __DATE__ " " __TIME__);
            ImGui::EndMenu();
        }

        std::lock_guard LG(DevicesList.outside_ips_mutex);
        ImGui::TextColored(GRAY_COLOR, "Внешние адреса:");
        ImGui::TextColored(
            DevicesList.outside_ip_v4.has_value() ? GREEN_COLOR : RED_COLOR,
            DevicesList.outside_ip_v4.has_value() ? DevicesList.outside_ip_v4.value().to_string().c_str() : "нет" );
        ImGui::TextColored(GRAY_COLOR, ",");
        ImGui::TextColored(
            DevicesList.outside_ip_v6.has_value() ? GREEN_COLOR : RED_COLOR,
            DevicesList.outside_ip_v6.has_value() ? DevicesList.outside_ip_v6.value().to_string().c_str() : "нет");
        ImGui::EndMenuBar();
    }
    
    //Обработка клавиш
    if (ImGui::IsKeyPressed(ImGuiKey_S, false) && !shift_key &&  ctrl_key && !alt_key)
    {
        if (file_name.empty())
            show_save_as_popup = true;
        else
        {
            std::string grid_data(ImNodes::SaveCurrentEditorStateToIniString());
            DevicesList.save(grid_data.c_str(), grid_data.size(), file_name);
            Frontlib::PushWindowMessage(current_window, "Сохранено", std::chrono::seconds(5), Frontlib::Colors::Lime);
        }
    }
	if (show_save_as_popup || (
        ImGui::IsKeyPressed(ImGuiKey_S, false) &&  shift_key &&  ctrl_key && !alt_key)) ImGui::OpenPopup("Сохранить как");
    if (ImGui::IsKeyPressed(ImGuiKey_N, false) && !shift_key &&  ctrl_key && !alt_key)   edit_node_tool.show_popup_new = true;
    if (ImGui::IsKeyPressed(ImGuiKey_R, false) && !shift_key &&  ctrl_key && !alt_key) DevicesList.clear_statistics();
    if (ImGui::IsKeyPressed(ImGuiKey_T, false) && !shift_key &&  ctrl_key && !alt_key)  traceroute_tool.show_popup   = true;
    if (ImGui::IsKeyPressed(ImGuiKey_D, false) && !shift_key &&  ctrl_key && !alt_key)         DNS_tool.show_popup   = true;
    if (ImGui::IsKeyPressed(ImGuiKey_D, false) &&  shift_key &&  ctrl_key && !alt_key)        rDNS_tool.show_popup   = true;
    if (ImGui::IsKeyPressed(ImGuiKey_W, false) && !shift_key &&  ctrl_key && !alt_key)         WOL_tool.show_popup   = true;
    if (ImGui::IsKeyPressed(ImGuiKey_R, false) && !shift_key && !ctrl_key &&  alt_key)   edit_link_tool.keys_pressed = true;
    else                                                                                 edit_link_tool.keys_pressed = false;

    //Начало отрисовки поля нод
    ImNodes::PushColorStyle(ImNodesCol_GridBackground, Frontlib::Colors::float_to_uint(BACKGROUND_COLOR));
    ImNodes::BeginNodeEditor();
    
    //Легенда
    ImNodes::SetNodeScreenSpacePos(LEGEND_NODE_ID, ImVec2{ 20,40 });
    ImNodes::BeginNode(LEGEND_NODE_ID);
    ImGui::PushStyleColor(ImGuiCol_Text,  GREEN_COLOR); ImGui::Text("ping <  100ms"); ImGui::PopStyleColor();
    ImGui::PushStyleColor(ImGuiCol_Text, YELLOW_COLOR); ImGui::Text("ping >= 100ms"); ImGui::PopStyleColor();
    ImGui::PushStyleColor(ImGuiCol_Text,    RED_COLOR); ImGui::Text("   ошибка    "); ImGui::PopStyleColor();
    ImGui::PushStyleColor(ImGuiCol_Text,   GRAY_COLOR); ImGui::Text(" нет данных  "); ImGui::PopStyleColor();
    for (int i = Device_port_type_Unknown; i < Device_port_type_Count; i++)
    {
        Device_port_type _type_ = static_cast<Device_port_type>(i);
        ImNodes::PushColorStyle(ImNodesCol_Pin       , pin_color     (_type_));
        ImNodes::PushColorStyle(ImNodesCol_PinHovered, pin_color_bold(_type_));
        ImNodes::BeginOutputAttribute(LEGEND_NODE_ID + i + 1, translate(_type_));
        ImGui::Text(type_to_str(_type_));
        ImNodes::EndOutputAttribute();
        ImNodes::PopColorStyle();
    }
    ImNodes::EndNode();

    // Отрисовка нод
    for (size_t i = 0; i < DevicesList.devices_size(); i++)
    {
        const int id = DevicesList[i].get_id() * ID_K;

        //Считаем минимальный и максимальный пинги на всех IP адресах ноды, чтобы выставить цвет шапки
        if (!DevicesList[i].IPsV4.empty() || !DevicesList[i].IPsV6.empty())
        {
            const int16_t min_ping_v4 = DevicesList[i].get_min_ping_v4();
            const int16_t min_ping_v6 = DevicesList[i].get_min_ping_v6();
            const int16_t min_ping    = min_ping_v4 < min_ping_v6 ? min_ping_v4 : min_ping_v6;
            const int16_t max_ping_v4 = DevicesList[i].get_max_ping_v4();
            const int16_t max_ping_v6 = DevicesList[i].get_max_ping_v6();
            const int16_t max_ping    = max_ping_v4 > max_ping_v6 ? max_ping_v4 : max_ping_v6;
            const int16_t ping_color  = min_ping < 0 && min_ping != -5 ? min_ping : max_ping;

            //Выставляем цвет шапки
            if      (ping_color >= 100)
            {
                ImNodes::PushColorStyle(ImNodesCol_TitleBar        , Frontlib::Colors::float_to_uint(YELLOW_COLOR));
                ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered , Frontlib::Colors::float_to_uint(YELLOW_COLOR));
                ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, Frontlib::Colors::float_to_uint(YELLOW_COLOR));
            }
            else if (ping_color >=   0)
            {
                ImNodes::PushColorStyle(ImNodesCol_TitleBar        , Frontlib::Colors::float_to_uint(GREEN_COLOR));
                ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered , Frontlib::Colors::float_to_uint(GREEN_COLOR));
                ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, Frontlib::Colors::float_to_uint(GREEN_COLOR));
            }
            else if (ping_color ==  -5)
            {
                ImNodes::PushColorStyle(ImNodesCol_TitleBar        , Frontlib::Colors::float_to_uint(GRAY_COLOR));
                ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered , Frontlib::Colors::float_to_uint(GRAY_COLOR));
                ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, Frontlib::Colors::float_to_uint(GRAY_COLOR));
            }
            else
            {
                ImNodes::PushColorStyle(ImNodesCol_TitleBar        , Frontlib::Colors::float_to_uint(RED_COLOR));
                ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered , Frontlib::Colors::float_to_uint(RED_COLOR));
                ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, Frontlib::Colors::float_to_uint(RED_COLOR));
            }
        }
        else
        {
            ImNodes::PushColorStyle(ImNodesCol_TitleBar        , Frontlib::Colors::float_to_uint(GRAY_COLOR));
            ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered , Frontlib::Colors::float_to_uint(GRAY_COLOR));
            ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, Frontlib::Colors::float_to_uint(GRAY_COLOR));
        }

        ImNodes::BeginNode(id);

        //Шапка
        ImNodes::BeginNodeTitleBar();
        ImGui::Text(DevicesList[i].Nickname.data());
        ImNodes::EndNodeTitleBar();

        //Если есть NAT - показываем
        if (DevicesList[i].NAT)
            ImGui::TextColored(Frontlib::Colors::uint_to_float(VPN_COLOR_SELECTED), "NAT");

        //Отрисовываем IPv4 и IPv6 адресов
        if (!Hide_IPv4s)
            for (size_t j = 0; j < DevicesList[i].IPsV4.size(); j++)
            {
                if (DevicesList[i].NeedToPing)
                {
                    int16_t ping_value = DevicesList[i].IPsV4[j].get_ping();
                    if (ping_value >= 100)
                        ImGui::PushStyleColor(ImGuiCol_Text, YELLOW_COLOR);
                    else if (ping_value >= 0)
                        ImGui::PushStyleColor(ImGuiCol_Text,  GREEN_COLOR);
                    else if (ping_value == -5)
                        ImGui::PushStyleColor(ImGuiCol_Text,   GRAY_COLOR);
                    else
                        ImGui::PushStyleColor(ImGuiCol_Text,    RED_COLOR);
                }
                ImGui::Text(DevicesList[i].IPsV4[j].get_ip().to_string().data());
                if (DevicesList[i].NeedToPing)
                    ImGui::PopStyleColor();
            }
        if (!Hide_IPv6s)
            for (size_t j = 0; j < DevicesList[i].IPsV6.size(); j++)
            {
                if (DevicesList[i].NeedToPing)
                {
                    int16_t ping_value = DevicesList[i].IPsV6[j].get_ping();
                    if (ping_value >= 100)
                        ImGui::PushStyleColor(ImGuiCol_Text, YELLOW_COLOR);
                    else if (ping_value >= 0)
                        ImGui::PushStyleColor(ImGuiCol_Text,  GREEN_COLOR);
                    else if (ping_value == -5)
                        ImGui::PushStyleColor(ImGuiCol_Text,   GRAY_COLOR);
                    else
                        ImGui::PushStyleColor(ImGuiCol_Text,    RED_COLOR);
                }
                ImGui::Text(DevicesList[i].IPsV6[j].get_ip().to_string().data());
                if (DevicesList[i].NeedToPing)
                    ImGui::PopStyleColor();
            }

        //Отисовка MAC адресов
        if (!Hide_MACs)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, GRAY_COLOR);
            for (size_t j = 0; j < DevicesList[i].MAC.size(); j++)
                ImGui::Text((std::string("MAC  ") + DevicesList[i].MAC[j].to_string()).data());
            ImGui::PopStyleColor();
        }

        //Отрисовка входов и выходов
        const size_t  in_size = DevicesList[i].Inputs .size();
        const size_t out_size = DevicesList[i].Outputs.size();
        const ImVec2 NodeSize = ImNodes::GetNodeDimensions(id);
        if (ImGui::BeginTable(("##inputs_outputs" + std::to_string(i)).c_str(), 2, 0, { NodeSize.x > 220 ? NodeSize.x - 20 : 200, 0}))
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            
            ImNodes::PushAttributeFlag(ImNodesAttributeFlags_EnableLinkDetachWithDragClick);
            for (size_t n = 0; n < in_size || n < out_size; n++)
            {
                if (DevicesList[i].Inputs .size() > n)
                {
                    auto& _input_ = DevicesList[i].Inputs[n];
                    ImNodes::PushColorStyle(ImNodesCol_Pin       , pin_color     (_input_.Type));
                    ImNodes::PushColorStyle(ImNodesCol_PinHovered, pin_color_bold(_input_.Type));
                    ImNodes::BeginInputAttribute(int(id + 2 * n), translate(_input_.Type));
                    ImGui::Text(_input_.Name.c_str());
                    ImNodes::EndInputAttribute();
                    ImNodes::PopColorStyle();
                }
                ImGui::TableNextColumn();
                
                if (DevicesList[i].Outputs.size() > n)
                {
                    auto& _output_ = DevicesList[i].Outputs[n];
                    ImNodes::PushColorStyle(ImNodesCol_Pin       , pin_color     (_output_.Type));
                    ImNodes::PushColorStyle(ImNodesCol_PinHovered, pin_color_bold(_output_.Type));
                    ImNodes::BeginOutputAttribute(int(id + 2 * n + 1), translate(_output_.Type));
                    ImGui::Text(_output_.Name.c_str());
                    ImNodes::EndOutputAttribute();
                    ImNodes::PopColorStyle();
                }
                ImGui::TableNextColumn();
            }
            ImNodes::PopAttributeFlag();

            ImGui::EndTable();
        }

        ImNodes::EndNode();
        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();
    }
    
    //Отрисовка связей
    std::unique_lock UL1(*DevicesList.get_devices_lock());
    std::unique_lock UL2(*DevicesList.get_links_lock());
    for (int i = 0; i < DevicesList.links_size(); i++)
    {
		const Link_t& link = DevicesList.get_link(i);
        ImU32 link_color;
        ImU32 link_color_bold;

        //Подготовка к отрисовке и проверка связи на валидность
        {
            Device_port_t left_port, right_port;
            if (!DevicesList.link_id_to_ports(i, left_port, right_port))
            {
                DevicesList.delete_link(i);
                continue;
            }
            if      ( left_port.Type == Device_port_type_Unknown) { link_color = pin_color(right_port.Type); link_color_bold = pin_color_bold(right_port.Type); }
            else if (right_port.Type == Device_port_type_Unknown) { link_color = pin_color( left_port.Type); link_color_bold = pin_color_bold( left_port.Type); }
            else if (left_port.Type != right_port.Type)
            {
                DevicesList.delete_link(i);
                continue;
            }
            else { link_color = pin_color(left_port.Type); link_color_bold = pin_color_bold(left_port.Type); }
        }
        //Непосредственно отрисовка
        {
            ImNodes::PushColorStyle(ImNodesCol_Link        , link_color     );
            ImNodes::PushColorStyle(ImNodesCol_LinkHovered , link_color_bold);
            ImNodes::PushColorStyle(ImNodesCol_LinkSelected, link_color_bold);
            ImNodes::Link(i, link.left_id, link.right_id);
            ImNodes::PopColorStyle();
            ImNodes::PopColorStyle();
            ImNodes::PopColorStyle();
        }
        
    }
    UL2.unlock();
    UL1.unlock();
    
    //Миникарта
    ImNodes::MiniMap(0.1f, ImNodesMiniMapLocation_BottomLeft);
    ImNodes::EndNodeEditor();
    ImNodes::PopColorStyle();

    // Обработка правого клика по ноде: показываем popup
    // Контекстное меню: хранит индекс выбранной ноды (index в DevicesList) и позицию popup
    static int    node_hovered_id    = -1;
    static int    link_hovered_id    = -1;
    static int    node_hovered_index = -1;
    static ImVec2 node_pos           = { 0.0f, 0.0f };

    //Проверка на то, что мы навели мышкой на ноду
    if      (ImNodes::IsNodeHovered(&node_hovered_id) && node_hovered_id != -1 && node_hovered_id != LEGEND_NODE_ID)
    {
        node_hovered_index = DevicesList.get_device_index_by_id(node_hovered_id / ID_K);
        node_pos           = ImGui::GetMousePos();
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) ImGui::OpenPopup("Контекстное меню ноды");
        else if ((!DevicesList[node_hovered_index].IPsV4.empty() || !DevicesList[node_hovered_index].IPsV6.empty()) && ImGui::BeginTooltip())
        {
            for (size_t j = 0; j < DevicesList[node_hovered_index].IPsV4.size(); j++)
            {
                const auto& ip_info = DevicesList[node_hovered_index].IPsV4[j];
                
                ImGui::PushStyleColor(ImGuiCol_Text, ip_info.get_ping() >= 100 ? ImVec4(1.0f, 1.0f, 0.0f, 1.0f) :
                                                     ip_info.get_ping() >=   0 ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) :
                                                     ip_info.get_ping() ==  -5 ? ImVec4(1.0f, 1.0f, 1.0f, 0.5f) :
                                                                                 ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                switch (ip_info.get_ping())
                {
                case -1: ImGui::Text("УЗЕЛ НЕДОСТУПЕН");          break;
                case -2: ImGui::Text("ПРЕВЫШЕНО ВРЕМЯ ОЖИДАНИЯ"); break;
                case -3: ImGui::Text("ПАКЕТ ПОВРЕЖДЁН");          break;
                case -4: ImGui::Text("ОШИБКА ПЕРЕДАЧИ");          break;
                case -5: ImGui::Text("НЕТ ДАННЫХ");               break;
                default: ImGui::Text((std::to_string(ip_info.get_ping()) + "ms").c_str()); break;
                }
                ImGui::PopStyleColor();

                if (DevicesList[node_hovered_index].NeedToPing)
                {
				    ImGui::SameLine();
                    ImGui::Text("потери %.0f%%", (float  )ip_info.get_loss());
                    ImGui::SameLine();
                    ImGui::Text("среднее %d"   , (int16_t)ip_info.get_avg ());
                }

                auto buffer = get_ip_type(ip_info.get_ip());
                if (buffer.has_value())
                {
                    ImGui::SameLine();
                    ImGui::TextColored(GRAY_COLOR, type_to_str(buffer.value()));
                }
            }
            for (size_t j = 0; j < DevicesList[node_hovered_index].IPsV6.size(); j++)
            {
                const auto& ip_info = DevicesList[node_hovered_index].IPsV6[j];
                ImGui::PushStyleColor(ImGuiCol_Text, ip_info.get_ping() >= 100 ? ImVec4(1.0f, 1.0f, 0.0f, 1.0f) :
                                                     ip_info.get_ping() >=   0 ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) :
                                                     ip_info.get_ping() ==  -5 ? ImVec4(1.0f, 1.0f, 1.0f, 0.5f) :
                                                                                 ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                switch (ip_info.get_ping())
                {
                case -1: ImGui::Text("УЗЕЛ НЕДОСТУПЕН");          break;
                case -2: ImGui::Text("ПРЕВЫШЕНО ВРЕМЯ ОЖИДАНИЯ"); break;
                case -3: ImGui::Text("ПАКЕТ ПОВРЕЖДЁН");          break;
                case -4: ImGui::Text("ОШИБКА ПЕРЕДАЧИ");          break;
                case -5: ImGui::Text("НЕТ ДАННЫХ");               break;
                default: ImGui::Text((std::to_string(ip_info.get_ping()) + "ms").c_str()); break;
                }
                ImGui::PopStyleColor();

                if (DevicesList[node_hovered_index].NeedToPing && ip_info.get_ping() != -5)
                {
                    ImGui::SameLine();
                    ImGui::Text("потери %.0f%%", (float  )ip_info.get_loss());
                    ImGui::SameLine();
                    ImGui::Text("среднее %d"   , (int16_t)ip_info.get_avg ());
                }

                auto buffer = get_ip_type(ip_info.get_ip());
                if (buffer.has_value())
                {
                    ImGui::SameLine();
                    ImGui::TextColored(GRAY_COLOR, type_to_str(buffer.value()));
                }
            }
            ImGui::EndTooltip();
        }
    }
    //Проверка на то, что мы навели мышкой на связь
    else if (ImNodes::IsLinkHovered(&link_hovered_id) && link_hovered_id != -1)
    {
        const Link_t& link = DevicesList.get_link(link_hovered_id);
        if (link.Type != Link_type_Unknown && ImGui::BeginTooltip())
        {
            if (!link.label.empty())
                ImGui::Text(('\"' + link.label + '\"').c_str());

            if (link.len > 1000)
                ImGui::Text("Длина %.2f км", link.len / 1000);
            else if (link.len > 0 && link.len < 1)
                ImGui::Text("Длина %.2f см", link.len * 100);
            else if (link.len)
                ImGui::Text("Длина %.2f м", link.len);

            if (link.Type != Link_type_Unknown)
            {
                ImGui::Text(type_to_str(link.Type));
                //DONE
                if      (link.Type == Link_type_Copper)
                {
                    if (link.Copper.CatType != Link_copper_type_Unknown)
                    {
                        ImGui::SameLine();
                        ImGui::Text(type_to_str(link.Copper.CatType));
                        if (link.Copper.ShieldType != Link_shield_type_Unknown)
                        {
                            ImGui::SameLine();
                            ImGui::Text(type_to_str(link.Copper.ShieldType));
                        }
                    }
                }
                //DONE
                else if (link.Type == Link_type_Optic)
                {
                    if (link.OpticLeft )
                    {
                        ImGui::Text("Левый  порт:");
                        if (link.OpticLeft.Connector)
                        {
                            ImGui::SameLine();
                            ImGui::Text(type_to_str(link.OpticLeft.Connector));
                            if (link.OpticLeft.Polishing)
                            {
                                ImGui::SameLine();
                                ImGui::Text("(%s)", type_to_str(link.OpticLeft.Polishing));
                            }
                        }
                        if (link.OpticLeft.Line)
                        {
                            ImGui::SameLine();
                            ImGui::Text(type_to_str(link.OpticLeft.Line));
                        }
                        if (link.OpticLeft.Mod)
                        {
                            ImGui::SameLine();
                            ImGui::Text(type_to_str(link.OpticLeft.Mod));
                        }
                    }
                    if (link.OpticRight)
                    {
                        ImGui::Text("Правый порт:");
                        if (link.OpticRight.Connector)
                        {
                            ImGui::SameLine();
                            ImGui::Text(type_to_str(link.OpticRight.Connector));
                            if (link.OpticRight.Polishing)
                            {
                                ImGui::SameLine();
                                ImGui::Text("(%s)", type_to_str(link.OpticRight.Polishing));
                            }
                        }
                        if (link.OpticRight.Line)
                        {
                            ImGui::SameLine();
                            ImGui::Text(type_to_str(link.OpticRight.Line));
                        }
                        if (link.OpticRight.Mod)
                        {
                            ImGui::SameLine();
                            ImGui::Text(type_to_str(link.OpticRight.Mod));
                        }
                    }
                }
                //DONE
                else if (link.Type == Link_type_WiFi)
                {
                    if (link.WiFi.Type != Link_WiFi_type_Unknown)
                    {
                        ImGui::SameLine();
                        ImGui::Text("(%s)", type_to_str(link.WiFi.Type));
                        ImGui::SameLine();
                        ImGui::Text("%d MHz", link.WiFi.freq);
                    }
                }
                //DONE
                else if (link.Type == Link_type_Radio)
                {
                    if (link.Radio.Type != Link_WiFi_type_Unknown)
                    {
                        ImGui::SameLine();
                        ImGui::Text("(%s)", type_to_str(link.Radio.Type));
                        ImGui::SameLine();
                        ImGui::Text("%d MHz", link.Radio.freq);
                    }
                }
            }
            ImGui::TextColored(GRAY_COLOR, "Нажми Alt+R для редактирования");
            if (edit_link_tool.keys_pressed)
            {
                edit_link_tool.target     = link_hovered_id;
                edit_link_tool.show_popup = true;
            }

            ImGui::EndTooltip();
        }
    }

    //Обработка событий создания и удаления связей
    int begin, end;
    int link;
    if (ImNodes::IsLinkCreated  (&begin, &end) && begin < LEGEND_NODE_ID && end < LEGEND_NODE_ID) DevicesList.create_link(begin, end);
    if (ImNodes::IsLinkDestroyed(&link       )                                                  ) DevicesList.delete_link(link      );

    //Popups
    ImGui::SetNextWindowPos(node_pos);
    if (ImGui::BeginPopup("Контекстное меню ноды"))
    {
        // Защита от выхода за диапазон (список мог измениться)
        if (node_hovered_index >= 0 && static_cast<size_t>(node_hovered_index) < DevicesList.devices_size())
        {
            // Показать имя ноды в заголовке меню
            ImGui::TextDisabled("%s", DevicesList[node_hovered_index].Nickname.c_str());
            ImGui::Separator();

            // Редактирование ноды
            if (DevicesList[node_hovered_index].CanBeDeleted)
                if (ImGui::Checkbox("Пинговать", &DevicesList[node_hovered_index].NeedToPing) && !DevicesList[node_hovered_index].NeedToPing)
                    DevicesList[node_hovered_index].clear_statistics();

            if (DevicesList[node_hovered_index].CanBeDeleted)
                if (ImGui::MenuItem("Изменить"))
                {
                    edit_node_tool.show_popup_edit = true;
                    ImGui::CloseCurrentPopup();
                }

            if (ImGui::MenuItem("Сбросить статистику")) DevicesList.clear_statistics(node_hovered_index);

            if (ImGui::MenuItem("Traceroute"))
            {
                traceroute_tool.target = DevicesList[node_hovered_index];
                std::thread(&traceroute_tool_t::do_traceroute, &traceroute_tool).detach();
                traceroute_tool.show_popup = true;
                ImGui::CloseCurrentPopup();
            }

            if (ImGui::MenuItem("Traceroute + rDNS"))
            {
                traceroute_tool.target = DevicesList[node_hovered_index];
                std::thread(&traceroute_tool_t::do_traceroute_with_rDNS, &traceroute_tool).detach();
                traceroute_tool.show_popup = true;
                ImGui::CloseCurrentPopup();
            }

            if (DevicesList[node_hovered_index].CanBeDeleted)
                if (ImGui::MenuItem("Удалить")) DevicesList.erase(node_hovered_index);
        }
        else ImGui::TextDisabled("Нода недоступна");

        ImGui::EndPopup();
    }
    if (traceroute_tool.show_popup     ) { traceroute_tool.show_popup      = false; ImGui::OpenPopup("Traceroute"          ); }
    if (       DNS_tool.show_popup     ) {        DNS_tool.show_popup      = false; ImGui::OpenPopup("DNS"                 ); }
    if (      rDNS_tool.show_popup     ) {       rDNS_tool.show_popup      = false; ImGui::OpenPopup("rDNS"                ); }
    if (       WOL_tool.show_popup     ) {        WOL_tool.show_popup      = false; ImGui::OpenPopup("WOL"                 ); }
    if ( edit_link_tool.show_popup     ) {  edit_link_tool.show_popup      = false; ImGui::OpenPopup("Редактирование связи"); }
    if ( edit_node_tool.show_popup_edit) {  edit_node_tool.show_popup_edit = false; ImGui::OpenPopup("Редактирование ноды" ); }
    if ( edit_node_tool.show_popup_new ) {  edit_node_tool.show_popup_new  = false; ImGui::OpenPopup("Создание ноды"       ); }
    if (ImGui::BeginPopupModal("Traceroute"          , nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        traceroute_tool.show();
        ImGui::EndPopup();
    }
    if (ImGui::BeginPopupModal("DNS"                 , nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        DNS_tool.show();
        ImGui::EndPopup();
    }
    if (ImGui::BeginPopupModal("rDNS"                , nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        rDNS_tool.show();
        ImGui::EndPopup();
    }
    if (ImGui::BeginPopupModal("WOL"                 , nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        WOL_tool.show();
        ImGui::EndPopup();
    }
    if (ImGui::BeginPopupModal("Редактирование связи", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        edit_link_tool.show();
        ImGui::EndPopup();
    }
    if (ImGui::BeginPopupModal("Редактирование ноды" , nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        edit_node_tool.show(node_hovered_id / ID_K);
        ImGui::EndPopup();
    }
    if (ImGui::BeginPopupModal("Создание ноды"       , nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        edit_node_tool.show();
        ImGui::EndPopup();
    }
    if (ImGui::BeginPopupModal("Сохранить как"       , nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
		if (ImGui::InputText("Имя файла", &file_name)) {}
        if (ImGui::Button("Отмена") || ImGui::IsKeyPressed(ImGuiKey_Escape, false))
			ImGui::CloseCurrentPopup();
		ImGui::SameLine();
        if (ImGui::Button("Сохранить") || ImGui::IsKeyPressed(ImGuiKey_Enter, false))
        {
            std::string grid_data(ImNodes::SaveCurrentEditorStateToIniString());
            DevicesList.save(grid_data.c_str(), grid_data.size(), file_name);
            Frontlib::PushWindowMessage(current_window, "Сохранено", std::chrono::seconds(5), Frontlib::Colors::Lime);
            ImGui::CloseCurrentPopup();
		}
        ImGui::EndPopup();
    }

    ImNodes::PopStyleVar(3);
    Frontlib::ShowWindowMessages();
    ImGui::End();
}



class SimpleSum :
    public ImFlow::BaseNode
{
public:
    SimpleSum()
    {
        setTitle("Simple sum");
        setStyle(ImFlow::NodeStyle::green());
        ImFlow::BaseNode::addIN<int>("In", 0, ImFlow::ConnectionFilter::SameType());
        ImFlow::BaseNode::addOUT<int>("Out", nullptr)->behaviour([this]() { return getInVal<int>("In") + m_valB; });
    }

    void draw() override
    {
        ImGui::SetNextItemWidth(100.f);
        ImGui::InputInt("##ValB", &m_valB);
    }

private:
    int m_valB = 0;
};
class CollapsingNode :
    public ImFlow::BaseNode
{
public:
    CollapsingNode()
    {
        setTitle("Collapsing node");
        setStyle(ImFlow::NodeStyle::red());
        ImFlow::BaseNode::addIN<int>("A", 0, ImFlow::ConnectionFilter::SameType());
        ImFlow::BaseNode::addIN<int>("B", 0, ImFlow::ConnectionFilter::SameType());
        ImFlow::BaseNode::addOUT<int>("Out", nullptr)->behaviour([this]() { return getInVal<int>("A") + getInVal<int>("B"); });
    }

    void draw() override
    {
        if (ImFlow::BaseNode::isSelected()) {
            ImGui::SetNextItemWidth(100.f);
            ImGui::Text("You can only see me when the node is selected!");
        }
    }
};
class ResultNode :
    public ImFlow::BaseNode
{
public:
    ResultNode()
    {
        setTitle("Result node");
        setStyle(ImFlow::NodeStyle::brown());
        ImFlow::BaseNode::addIN<int>("A", 0, ImFlow::ConnectionFilter::SameType());
        ImFlow::BaseNode::addIN<int>("B", 0, ImFlow::ConnectionFilter::SameType());
    }

    void draw() override
    {
        ImGui::Text("Result: %d", getInVal<int>("A") + getInVal<int>("B"));
    }
};
class NodeEditor :
    public ImFlow::ImNodeFlow
{
public:
    NodeEditor() { }
    NodeEditor(                           const ImVec2& _size_)                              { ImFlow::ImNodeFlow::setSize(_size_); }
    NodeEditor(const std::string& _name_                      ) : ImFlow::ImNodeFlow(_name_) {                                      }
    NodeEditor(const std::string& _name_, const ImVec2& _size_) : ImFlow::ImNodeFlow(_name_) { ImFlow::ImNodeFlow::setSize(_size_); }

    void render()
    {
        ImFlow::ImNodeFlow::update();
        
        // Update states
        auto selected_links = getSelectedLinks();
        auto  hovered_links = getHoveredLinks();
        auto selected_nodes = getSelectedNodes();
        auto  hovered_nodes = getHoveredNodes();

        if (!rightClickPopUpContent_State)
        {
            if (!selected_links.empty() && !selected_nodes.empty()) ImGui::OpenPopup("multiSelectedPopUpContent_CrossFunc");
            else if (!selected_links.empty() || !selected_nodes.empty())
                if (selected_links.size() > 1 || selected_nodes.size() > 1)
                {
                    if      (!selected_links.empty()) ImGui::OpenPopup("multiSelectedPopUpContent_LinkFunc");
                    else if (!selected_nodes.empty()) ImGui::OpenPopup("multiSelectedPopUpContent_NodeFunc");
                }
                else
                {
                    if      (!selected_links.empty()) ImGui::OpenPopup("singleSelectedPopUpContent_LinkFunc");
                    else if (!selected_nodes.empty()) ImGui::OpenPopup("singleSelectedPopUpContent_NodeFunc");
                }
            else
            {
                if      (!hovered_links.empty()) ImGui::OpenPopup("HoveredPopUpContent_LinkFunc");
                else if (!hovered_nodes.empty()) ImGui::OpenPopup("HoveredPopUpContent_NodeFunc");
            }
        }

        if (ImGui::BeginPopup("multiSelectedPopUpContent_CrossFunc"))
        {
            if (selected_nodes.empty() || selected_links.empty()) ImGui::CloseCurrentPopup();
            else multiSelectedPopUpContent_CrossFunc(selected_nodes, selected_links);
            ImGui::EndPopup();
        }
        if (ImGui::BeginPopup( "multiSelectedPopUpContent_LinkFunc"))
        {
            if (selected_links.empty()) ImGui::CloseCurrentPopup();
            else multiSelectedPopUpContent_LinkFunc(selected_links);
            ImGui::EndPopup();
        }
        if (ImGui::BeginPopup( "multiSelectedPopUpContent_NodeFunc"))
        {
            if (selected_nodes.empty()) ImGui::CloseCurrentPopup();
            else multiSelectedPopUpContent_NodeFunc(selected_nodes);
            ImGui::EndPopup();
        }
        if (ImGui::BeginPopup("singleSelectedPopUpContent_LinkFunc"))
        {
            if (selected_links.empty()) ImGui::CloseCurrentPopup();
            else singleSelectedPopUpContent_LinkFunc(*selected_links.begin());
            ImGui::EndPopup();
        }
        if (ImGui::BeginPopup("singleSelectedPopUpContent_NodeFunc"))
        {
            if (selected_nodes.empty()) ImGui::CloseCurrentPopup();
            else singleSelectedPopUpContent_NodeFunc(*selected_nodes.begin());
            ImGui::EndPopup();
        }
        if (ImGui::BeginPopup(       "HoveredPopUpContent_LinkFunc"))
        {
            if (hovered_links.empty()) ImGui::CloseCurrentPopup();
            else HoveredPopUpContent_LinkFunc(*hovered_links.begin());
            ImGui::EndPopup();
        }
        if (ImGui::BeginPopup(       "HoveredPopUpContent_NodeFunc"))
        {
            if (hovered_nodes.empty()) ImGui::CloseCurrentPopup();
            else HoveredPopUpContent_NodeFunc(*hovered_nodes.begin());
            ImGui::EndPopup();
        }

        rightClickPopUpContent_State = false;
    }

    ImFlow::Link* getSelectedLink() noexcept
    {
        auto& _links_ = this->getLinks();
        for (auto& _link_ : _links_)
            if (!_link_.expired())
                if (_link_.lock()->isSelected())
                    return _link_.lock().get();
        return nullptr;
    }
    ImFlow::Link* getHoveredLink() noexcept
    {
        auto& _links_ = this->getLinks();
        for (auto& _link_ : _links_)
            if (!_link_.expired())
                if (_link_.lock()->isHovered())
                    return _link_.lock().get();
        return nullptr;
    }
    std::vector<ImFlow::Link*> getSelectedLinks() noexcept
    {
        std::vector<ImFlow::Link*> ret;
        auto& _links_ = this->getLinks();
        for (auto& _link_ : _links_)
            if (!_link_.expired())
                if (_link_.lock()->isSelected())
                    ret.push_back(_link_.lock().get());
        return ret;
    }
    std::vector<ImFlow::Link*> getHoveredLinks() noexcept
    {
        std::vector<ImFlow::Link*> ret;
        auto& _links_ = this->getLinks();
        for (auto& _link_ : _links_)
            if (!_link_.expired())
                if (_link_.lock()->isHovered())
                    ret.push_back(_link_.lock().get());
        return ret;
    }

    ImFlow::BaseNode* getSelectedNode() noexcept
    {
        auto& _nodes_ = this->getNodes();
        for (auto& _node_ : _nodes_)
            if (_node_.second->isSelected())
                return _node_.second.get();
        return nullptr;
    }
    ImFlow::BaseNode* getHoveredNode() noexcept
    {
        auto& _nodes_ = this->getNodes();
        for (auto& _node_ : _nodes_)
            if (_node_.second->isHovered())
                return _node_.second.get();
        return nullptr;
    }
    std::vector<ImFlow::BaseNode*> getSelectedNodes() noexcept
    {
        std::vector<ImFlow::BaseNode*> ret;
        auto& _nodes_ = this->getNodes();
        for (auto& _node_ : _nodes_)
            if (_node_.second->isSelected())
                ret.push_back(_node_.second.get());
        return ret;
    }
    std::vector<ImFlow::BaseNode*> getHoveredNodes() noexcept
    {
        std::vector<ImFlow::BaseNode*> ret;
        auto& _nodes_ = this->getNodes();
        for (auto& _node_ : _nodes_)
            if (_node_.second->isHovered())
                ret.push_back(_node_.second.get());
        return ret;
    }

    void rightClickPopUp(std::function<void(ImFlow::BaseNode* _node_)> _content_)
    {
        rightClickPopUpContent_Func = _content_;
        this->rightClickPopUpContent([this, _content_](ImFlow::BaseNode* _node_)
            {
                rightClickPopUpContent_State = true;
                _content_(_node_);
            });
    }
    void hoveredPopUp   (std::function<void(ImFlow::BaseNode* _node_)> _content_) {        HoveredPopUpContent_NodeFunc = _content_; }
    void hoveredPopUp   (std::function<void(ImFlow::Link    * _link_)> _content_) {        HoveredPopUpContent_LinkFunc = _content_; }
    void selectedPopUp  (std::function<void(ImFlow::BaseNode* _node_)> _content_) { singleSelectedPopUpContent_NodeFunc = _content_; }
    void selectedPopUp  (std::function<void(ImFlow::Link    * _link_)> _content_) { singleSelectedPopUpContent_LinkFunc = _content_; }
    void selectedPopUp  (std::function<void(std::vector<ImFlow::BaseNode*>& _nodes_)> _content_) { multiSelectedPopUpContent_NodeFunc = _content_; }
    void selectedPopUp  (std::function<void(std::vector<ImFlow::Link    *>& _links_)> _content_) { multiSelectedPopUpContent_LinkFunc = _content_; }
    void selectedPopUp  (std::function<void(std::vector<ImFlow::BaseNode*>& _nodes_, std::vector<ImFlow::Link*>& _links_)> _content_) { multiSelectedPopUpContent_CrossFunc = _content_; }

private:
    using ImFlow::ImNodeFlow::update;

    std::function<void(ImFlow::BaseNode* _node_)> rightClickPopUpContent_Func  = [](ImFlow::BaseNode* _node_) {};
    bool                                          rightClickPopUpContent_State = false;
    std::function<void(ImFlow::BaseNode* _node_)> HoveredPopUpContent_NodeFunc = [](ImFlow::BaseNode* _node_) {};
    std::function<void(ImFlow::Link    * _link_)> HoveredPopUpContent_LinkFunc = [](ImFlow::Link    * _link_) {};
    std::function<void(ImFlow::BaseNode* _node_)> singleSelectedPopUpContent_NodeFunc = [](ImFlow::BaseNode* _node_) {};
    std::function<void(ImFlow::Link    * _link_)> singleSelectedPopUpContent_LinkFunc = [](ImFlow::Link    * _link_) {};
    std::function<void(std::vector<ImFlow::BaseNode*>& _nodes_)> multiSelectedPopUpContent_NodeFunc = [](std::vector<ImFlow::BaseNode*>& _nodes_) {};
    std::function<void(std::vector<ImFlow::Link    *>& _links_)> multiSelectedPopUpContent_LinkFunc = [](std::vector<ImFlow::Link    *>& _links_) {};
    std::function<void(std::vector<ImFlow::BaseNode*>& _nodes_, std::vector<ImFlow::Link*>& _links_)> multiSelectedPopUpContent_CrossFunc = [](std::vector<ImFlow::BaseNode*>& _nodes_, std::vector<ImFlow::Link*>& _links_) {};
} NodeSpace;
static void rightClickPopUp     (            ImFlow::BaseNode*   _node_ )
{
    if (_node_)
    {
        ImGui::Text(_node_->getName().c_str());
        ImGui::Separator();
        if (ImGui::Button("Удалить"))
        {
            NodeSpace.getNodes().erase(_node_->getUID());
            ImGui::CloseCurrentPopup();
        }
    }
    else
    {
        ImGui::Text("Just a popup");
        //TODO
    }
}
static void    hoveredPopUpNode (            ImFlow::BaseNode*   _node_ ) { ImGui::Text("Node hovered!"        ); }
static void    hoveredPopUpLink (            ImFlow::Link    *   _link_ ) { ImGui::Text("Link hovered!"        ); }
static void   selectedPopUpNode (            ImFlow::BaseNode*   _node_ ) { ImGui::Text("Node selected!"       ); }
static void   selectedPopUpLink (            ImFlow::Link    *   _link_ ) { ImGui::Text("Link selected!"       ); }
static void   selectedPopUpNodes(std::vector<ImFlow::BaseNode*>& _nodes_) { ImGui::Text("Multi nodes selected!"); }
static void   selectedPopUpLinks(std::vector<ImFlow::Link    *>& _links_) { ImGui::Text("Multi links selected!"); }
static void show_test()
{
    const auto window_size = ImGui::GetMainViewport()->Size - ImVec2(1, 1);
    const auto window_pos = ImVec2(1, 1);
    ImGui::SetNextWindowSize(window_size);
    ImGui::SetNextWindowPos(window_pos);
    ImGui::Begin("Node Editor", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    
    // Render grid space with nodes & links
    const auto node_editor_size = window_size - ImVec2(16, 16);
    NodeSpace.setSize(node_editor_size);
    NodeSpace.render();

    ImGui::End();
}



int main()
{
    //setlocale(LC_ALL, "Russian");
    //system("chcp 65001");

#   if !defined DEBUG && !defined _DEBUG
        //ConsoleHide();
#   else
        //ConsoleShow();
#   endif

    /*
    using namespace boost::asio::ip;

    udp::socket receiver(Connections::IOservice, udp::endpoint(udp::v4(), 0));
    raw::socket sender  (Connections::IOservice, raw::endpoint(raw::v4(), 0));
    
    const udp::endpoint receiver_endpoint = receiver.local_endpoint();
    const udp::endpoint  spoofed_endpoint(address_v4::from_string("8.8.8.8"), 12345);

    INF_SYS_S("receiver_endpoint : " << receiver_endpoint);
    INF_SYS_S(" spoofed_endpoint : " <<  spoofed_endpoint);

    std::string payload = "Hello World";

    udp_header udp_header;
    udp_header.     source_port( spoofed_endpoint.port());
    udp_header.destination_port(receiver_endpoint.port());
    udp_header.length(uint16_t(sizeof(udp_header) + payload.size()));
    udp_header.checksum(0); // Optioanl for IPv4

    IPv4_header ip_header;
    ip_header.version(4);
    ip_header.header_size(sizeof(ip_header) / 4);
    ip_header.code_point(0);
    ip_header.size(uint16_t(sizeof(ip_header) + sizeof(udp_header) + payload.size()));
    ip_header.id(0);
    ip_header.dont_fragment (true );
    ip_header.more_fragments(false);
    ip_header.fragment_offset(0);
    ip_header.ttl(128);
    ip_header.     source_addr( spoofed_endpoint.address().to_v4());
    ip_header.destination_addr(receiver_endpoint.address().to_v4());
    ip_header.protocol(IPPROTO_UDP);
    ip_header.checksum(ip_header.generate_checksum(ip_header));

    boost::system::error_code ec;
    boost::asio::streambuf sender_buffer; std::ostream os(&sender_buffer); os << ip_header << udp_header << payload;

    sender.send_to(sender_buffer.data(), raw::endpoint(receiver_endpoint.address(), receiver_endpoint.port()), 0, ec);
    if (ec)
        ERR_SYS_S(ec);

    std::vector<char> buffer(payload.size(), '\0');
    boost::asio::ip::udp::endpoint sender_endpoint;
    receiver.receive_from(boost::asio::buffer(buffer), sender_endpoint, 0, ec);
    if (ec)
        ERR_SYS_S(ec);

    //Print endpoints.
    std::cout <<
        "Actual sender endpoint    : " << sender  .local_endpoint() << "\n"
        "Receiver endpoint         : " << receiver.local_endpoint() << "\n"
        "Receiver's remote endpoint: " << sender_endpoint << std::endl;

    return 0;
    */

    //ARP_table.update();
    //interfaces_update1();
    //interfaces_update2();
    //return 0;

    DevicesList.push_back({});
    DevicesList.push_back(Device_t{{ip_address_v4::from_string("8.8.8.8"       ), ip_address_v4::from_string("8.8.4.4"        )}, {ip_address_v6::from_string("2001:4860:4860::8888"), ip_address_v6::from_string("2001:4860:4860::8844"  )}, {}, "DNS Google"       , true, true, {}, {{"интернет"}}});
    DevicesList.push_back(Device_t{{ip_address_v4::from_string("77.88.8.1"     ), ip_address_v4::from_string("77.88.8.8"      )}, {ip_address_v6::from_string("2a02:6b8::feed:0ff"  ), ip_address_v6::from_string("2a02:6b8:0:1::feed:0ff")}, {}, "DNS Yandex"       , true, true, {}, {{"интернет"}}});
    DevicesList.push_back(Device_t{{ip_address_v4::from_string("1.1.1.1"       ), ip_address_v4::from_string("1.0.0.1"        )}, {ip_address_v6::from_string("2606:4700:4700::1111"), ip_address_v6::from_string("2606:4700:4700::1001"  )}, {}, "DNS Cloudfare"    , true, true, {}, {{"интернет"}}});
    DevicesList.push_back(Device_t{{ip_address_v4::from_string("9.9.9.9"       ), ip_address_v4::from_string("149.112.112.112")}, {ip_address_v6::from_string("2620:fe::fe"         ), ip_address_v6::from_string("2620:fe::9"            )}, {}, "DNS Quad9"        , true, true, {}, {{"интернет"}}});
    DevicesList.push_back(Device_t{{ip_address_v4::from_string("208.67.222.222"), ip_address_v4::from_string("208.67.220.220" )}, {ip_address_v6::from_string("2620:119:35::35"     ), ip_address_v6::from_string("2620:119:53::53"       )}, {}, "DNS OpenDNS"      , true, true, {}, {{"интернет"}}});
    DevicesList.push_back(Device_t{{ip_address_v4::from_string("216.87.84.211" ), ip_address_v4::from_string("23.90.4.6"      )}, {                                                                                                        }, {}, "DNS OpenNIC"      , true, true, {}, {{"интернет"}}});
    DevicesList.push_back(Device_t{{ip_address_v4::from_string("185.228.168.9" ), ip_address_v4::from_string("185.228.169.9"  )}, {ip_address_v6::from_string("2a0d:2a00:1::1"      ), ip_address_v6::from_string("2a0d:2a00:2::2"        )}, {}, "DNS CleanBrowsing", true, true, {}, {{"интернет"}}});
    DevicesList.push_back(Device_t{{ip_address_v4::from_string("76.76.19.19"   ), ip_address_v4::from_string("76.223.122.150" )}, {ip_address_v6::from_string("2602:fcbc::ad"       ), ip_address_v6::from_string("2602:fcbc:2::ad"       )}, {}, "DNS AlternateDNS" , true, true, {}, {{"интернет"}}});
    DevicesList.push_back(Device_t{{ip_address_v4::from_string("94.140.14.14"  ), ip_address_v4::from_string("94.140.15.15"   )}, {ip_address_v6::from_string("2a10:50c0::ad1:ff"   ), ip_address_v6::from_string("2a10:50c0::ad2:ff"     )}, {}, "DNS AdGuardDNS"   , true, true, {}, {{"интернет"}}});
    DevicesList.push_back(Device_t{{ip_address_v4::from_string("76.76.2.0"     ), ip_address_v4::from_string("76.76.10.0"     )}, {ip_address_v6::from_string("2606:1a40::"         ), ip_address_v6::from_string("2606:1a40:1::"         )}, {}, "DNS ControlD"     , true, true, {}, {{"интернет"}}});
    DevicesList.push_back(Device_t{{ip_address_v4::from_string("64.6.64.6"     ), ip_address_v4::from_string("64.6.65.6"      )}, {ip_address_v6::from_string("2620:74:1b::1:1"     ), ip_address_v6::from_string("2620:74:1c::2:2"       )}, {}, "DNS Verisign"     , true, true, {}, {{"интернет"}}});

    auto n1     = NodeSpace.addNode<SimpleSum >({  40,  40 });
    auto n2     = NodeSpace.addNode<SimpleSum >({  40, 150 });
    auto result = NodeSpace.addNode<ResultNode>({ 250,  80 });

    // Add links between nodes
    n1->outPin("Out")->createLink(result->inPin("A"));
    n2->outPin("Out")->createLink(result->inPin("B"));

    // Add a collapsing node
    auto collapsingNode = NodeSpace.addNode<CollapsingNode>({ 300, 300 });

    NodeSpace.rightClickPopUp(rightClickPopUp     );
    NodeSpace.   hoveredPopUp(   hoveredPopUpNode );
    NodeSpace.   hoveredPopUp(   hoveredPopUpLink );
    NodeSpace.  selectedPopUp(  selectedPopUpNode );
    NodeSpace.  selectedPopUp(  selectedPopUpLink );
    NodeSpace.  selectedPopUp(  selectedPopUpNodes);
    NodeSpace.  selectedPopUp(  selectedPopUpLinks);

    Frontlib::CreateContext("NetRadar", 60, 1500, 500);
	Frontlib::LoadFont("C:\\Windows\\Fonts\\consola.ttf", 18);
    Frontlib::Show(&show, false);
    //Frontlib::Show(&show_test, false);
    Frontlib::DestroyContext();

    return 0;
}