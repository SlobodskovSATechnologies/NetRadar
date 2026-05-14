#ifndef DEVICES_LIST_HPP
#define DEVICES_LIST_HPP

#include "Device.hpp"
#include "Link.hpp"

struct DevicesList_t
{
public:
    DevicesList_t() : 
             background_pings_func_TF(&DevicesList_t::     background_pings_func, this),
        getting_an_ip_outside_func_TF(&DevicesList_t::getting_an_ip_outside_func, this)
    {
             background_pings_func_TF.Run();
        getting_an_ip_outside_func_TF.Run();
    }
    DevicesList_t(const Device_t& _device_) : 
             background_pings_func_TF(&DevicesList_t::     background_pings_func, this),
        getting_an_ip_outside_func_TF(&DevicesList_t::getting_an_ip_outside_func, this)
    { 
        devices.push_back(_device_); 
             background_pings_func_TF.Run();
        getting_an_ip_outside_func_TF.Run();
    }
    template<Utils::container Container>
        requires std::same_as<typename Container::value_type, Device_t>
    DevicesList_t(const Container& _container_) : 
             background_pings_func_TF(&DevicesList_t::     background_pings_func, this),
        getting_an_ip_outside_func_TF(&DevicesList_t::getting_an_ip_outside_func, this)
    { 
        devices.insert(devices.end(), _container_.begin(), _container_.end());
             background_pings_func_TF.Run();
        getting_an_ip_outside_func_TF.Run();
    }
    DevicesList_t(std::initializer_list<Device_t> _list_) : 
             background_pings_func_TF(&DevicesList_t::     background_pings_func, this),
        getting_an_ip_outside_func_TF(&DevicesList_t::getting_an_ip_outside_func, this), 
        devices(_list_.begin(), _list_.end())
    {
             background_pings_func_TF.Run();
        getting_an_ip_outside_func_TF.Run();
    }

    ~DevicesList_t()
    {
             background_pings_func_TF.Stop();
        getting_an_ip_outside_func_TF.Stop();
    }

    bool create_link(int _from_, int _to_) noexcept
    {
        std::lock_guard LG(links_mutex); 
        links.push_back({ static_cast<int>(_from_), static_cast<int>(_to_) });

        Device_port_t left, right;
        attr_ids_to_ports(_from_, _to_, left, right);
        std::vector<Link_t>::iterator link = (--links.end());
        if (left.Type != Device_port_type_Unknown || right.Type != Device_port_type_Unknown)
        {
            Device_port_type target_type;
            if      ( left.Type == Device_port_type_Unknown) target_type = right.Type;
            else if (right.Type == Device_port_type_Unknown) target_type =  left.Type;
            else if (left.Type != right.Type)
                return false;
            else target_type = left.Type;
            link->Type = static_cast<Link_type>(target_type);
            return true;
        }
        return true;
    }
    bool delete_link(int _from_, int _to_) noexcept
    {
        std::lock_guard LG(links_mutex);
        auto it = std::find_if(links.begin(), links.end(), [_from_, _to_](const Link_t& link) {
            return (link.left_id == static_cast<int>(_from_) && link.right_id == static_cast<int>(_to_  )) ||
                   (link.left_id == static_cast<int>(_to_  ) && link.right_id == static_cast<int>(_from_));
        });
        if (it != links.end())
        {
            links.erase(it);
            return true;
        }
        return false;
	}
    bool delete_links_of_node(int _id_) noexcept
    {
        std::lock_guard LG(links_mutex);

        auto original_size = links.size();
        links.erase(std::remove_if(links.begin(), links.end(), [_id_](const Link_t& link) {
			return link.left_id / ID_K == _id_ || link.right_id / ID_K == _id_;
            }), links.end());
        return links.size() != original_size;
    }
    bool delete_link(size_t _pos_) noexcept
    {
        std::lock_guard LG(links_mutex);
        if (_pos_ < links.size())
        {
            links.erase(links.begin() + _pos_);
            return true;
        }
        return false;
	}
    bool    are_linked(int _from_, int _to_) const noexcept
    {
        std::lock_guard LG(links_mutex);

        return std::any_of(links.begin(), links.end(), [_from_, _to_](const Link_t& link) {
            return (link.left_id == static_cast<int>(_from_) && link.right_id == static_cast<int>(_to_  )) ||
                   (link.left_id == static_cast<int>(_to_  ) && link.right_id == static_cast<int>(_from_));
        });
	}
    Link_t& get_link(size_t _pos_) noexcept
    {
        std::lock_guard LG(links_mutex);
        return links[_pos_];
	}

    size_t devices_size() const noexcept { std::lock_guard LG(  links_mutex); return devices.size(); }
    size_t   links_size() const noexcept { std::lock_guard LG(devices_mutex); return links  .size(); }
	void   push_back(const Device_t& _device_)
    {
        std::lock_guard LG(devices_mutex);

        auto __device__ = const_cast<Device_t&>(_device_);
        if (!is_id_vacant(_device_.get_id()))
            __device__.set_id(get_vacant_id());
        __device__.clear_statistics();
        devices.push_back(__device__);
    }
    bool   refresh  (const Device_t& _device_)
    {
        std::lock_guard LG(devices_mutex);

        if (is_id_vacant(_device_.get_id()))
            return false;

        devices[get_device_index_by_id(_device_.get_id())] = _device_;
        return true;
    }
	bool   erase    (const size_t _pos_)
    {
        std::lock_guard LG(devices_mutex);

        if (_pos_ < devices.size())
        {
			delete_links_of_node(get_id_by_device_index(_pos_));
			devices.erase(devices.begin() + _pos_);
            return true;
        }
        return false;
    }

    void devices_clear() noexcept
    {
        std::lock_guard LG(devices_mutex);

        for (size_t i = 0; i < devices.size(); i++)
            if (devices[i].CanBeDeleted)
            {
                delete_links_of_node(get_id_by_device_index(i));
                devices.erase(devices.begin() + i);
                i--;
            }
    }
    void   links_clear() noexcept { std::lock_guard LG(links_mutex); links.clear(); }

    Device_t& get_device_by_id(int _id_)
    {
        std::lock_guard LG(devices_mutex);
        return devices[get_device_index_by_id(_id_)];
    }

    void set_device_id(size_t _pos_, int _id_) noexcept
    {
        std::lock_guard LG(devices_mutex);

        if (_pos_ < devices.size())
            devices[_pos_].set_id(_id_);
    }
	int  get_vacant_id() const noexcept
    {
        std::lock_guard LG(devices_mutex);

        std::vector<bool> id_map(devices.size(), false);
        for (const auto& device : devices)
        {
            int id = device.get_id();
            if (id >= 0 && static_cast<size_t>(id) < id_map.size())
                id_map[id] = true;
        }
        for (size_t i = 0; i < id_map.size(); i++)
            if (!id_map[i])
                return static_cast<int>(i);
		return static_cast<int>(id_map.size()); // Return the next ID if all current IDs are occupied
    }
    bool is_id_vacant(int _id_) const noexcept
    {
        std::lock_guard LG(devices_mutex);

        for (const auto& device : devices)
            if (device.get_id() == _id_)
                return false;
        return true;
	}
    int get_device_index_by_id(int _id_) const noexcept
    {
        std::lock_guard LG(devices_mutex);

        for (size_t i = 0; i < devices.size(); i++)
            if (devices[i].get_id() == _id_)
                return static_cast<int>(i);
        return -1; // Return an invalid index if ID is not found
	}
    int get_id_by_device_index(size_t _index_) const noexcept
    {
        std::lock_guard LG(devices_mutex);

        if (_index_ < devices.size())
            return devices[_index_].get_id();
        return -1; // Return an invalid ID if index is out of range
    }

    void save(const char* _data_, size_t _size_, std::string _file_name_)
    {
        std::lock_guard LG1(devices_mutex);
        std::lock_guard LG2(  links_mutex);

        json j;
        j["GridData"] = std::string(_data_, _size_);
        j["Devices"] = json::array();
        for (const auto& device : devices)
            j["Devices"].push_back(device.to_json());
        j["Links"] = json::array();
        for (const auto& link : links)
            j["Links"].push_back(link.to_json());

        std::fstream SaveTool(_file_name_ + ".cfg", std::ios::out);
        if (!SaveTool.is_open())
            return; // Handle file opening error as needed
        else
            SaveTool << j.dump(4); // Write JSON to file with indentation
        SaveTool.close();
    }
    bool load(std::string& _data_, std::string _file_name_)
    {
        json j;

        std::fstream LoadTool(_file_name_ + ".cfg", std::ios::in);
        if (!LoadTool.is_open())
        {
            ERR_SYS_S("Не удаётсяоткрыть файл");
            return false; // Return empty JSON if file cannot be opened
        }
        else
        {
            try
            {
                LoadTool >> j;
            }
            catch (const std::exception& e)
            {
                ERR_SYS_S("Ошибка чтения JSON: " << e.what());
                return false; // Return empty JSON if parsing fails
            }
        }
        LoadTool.close();

        if (j.contains("GridData") && j["GridData"].is_string())
            _data_ = j["GridData"].get<std::string>();
        if (j.contains("Devices") && j["Devices"].is_array())
        {
            devices.clear();
            for (const auto& device_json : j["Devices"])
            {
                Device_t device;
                if (bool valid = device.from_json(const_cast<json&>(device_json)))
                    push_back(device);
            }
        }
        if (j.contains("Links"  ) && j["Links"  ].is_array())
        {
            links.clear();
            for (const auto& link_json : j["Links"])
            {
                Link_t link;
                if (bool valid = link.from_json(const_cast<json&>(link_json)))
                    links.push_back(link);
            }
        }
        return true;
    }

    void clear_statistics()
    {
        std::lock_guard LG(devices_mutex);
        for (auto& device : devices)
            device.clear_statistics();
    }
    void clear_statistics(const size_t _pos_)
    {
        std::lock_guard LG(devices_mutex);
        devices[_pos_].clear_statistics();
    }

    std::recursive_mutex* get_devices_lock() { return &devices_mutex; }
    std::recursive_mutex* get_links_lock  () { return &  links_mutex; }

    Device_t  operator[](const size_t _pos_) const { return devices[_pos_]; }
    Device_t& operator[](const size_t _pos_)       { return devices[_pos_]; }

    void attr_ids_to_ports(int _left_id_, int _right_id_, Device_port_t& _left_, Device_port_t& _right_)
    {
        std::lock_guard LG1(devices_mutex);
        std::lock_guard LG2(  links_mutex);

        const Device_t&  left_device = get_device_by_id( _left_id_ / ID_K);
        const Device_t& right_device = get_device_by_id(_right_id_ / ID_K);
        const bool  left_is_output =  _left_id_ % 2;
        const bool right_is_output = _right_id_ % 2;
        const int   left_attribute_number = ( _left_id_ % ID_K -  left_is_output) / 2;
        const int  right_attribute_number = (_right_id_ % ID_K - right_is_output) / 2;
        _left_  =  left_is_output ?  left_device.Outputs[ left_attribute_number] :  left_device.Inputs[ left_attribute_number];
        _right_ = right_is_output ? right_device.Outputs[right_attribute_number] : right_device.Inputs[right_attribute_number];
    }
    bool  link_id_to_ports(int _id_,                      Device_port_t& _left_, Device_port_t& _right_)
    {
        std::lock_guard LG1(devices_mutex);
        std::lock_guard LG2(  links_mutex);

        const Link_t& link = get_link(_id_);
        if (is_id_vacant(link.left_id / ID_K) || is_id_vacant(link.right_id / ID_K))
            return false;
        attr_ids_to_ports(link.left_id, link.right_id, _left_, _right_);
        return true;
    }

    std::optional<ip_address_v4> outside_ip_v4;
    std::optional<ip_address_v6> outside_ip_v6;
    std::mutex                   outside_ips_mutex;

private:
    void sorting_devices_by_id() noexcept
    {
        std::lock_guard LG(devices_mutex);

        std::sort(devices.begin(), devices.end(), [](const Device_t& a, const Device_t& b)
            {
                return a.get_id() < b.get_id();
            }
        );
	}
    void background_pings_func()
    {
        uint16_t Sequence = 0;

        std::unique_lock UL(devices_mutex);
        std::vector<std::pair<ip_address_v4, int16_t>> IPsV4;
        std::vector<std::pair<ip_address_v6, int16_t>> IPsV6;
        for (auto& device : devices)
            if (device.NeedToPing)
            {
                for (auto& ipv4 : device.IPsV4)
                    IPsV4.push_back({ ipv4.get_ip(), -5 });
                for (auto& ipv6 : device.IPsV6)
                    IPsV6.push_back({ ipv6.get_ip(), -5 });
            }
        UL.unlock();

        IPs_v4_t ips_v4 = DNS_v4(boost::asio::ip::host_name());
        IPs_v6_t ips_v6 = DNS_v6(boost::asio::ip::host_name());
        bool we_have_ip_v4 = false;
        bool we_have_ip_v6 = false;
        for (auto& ip_v4 : ips_v4)
            if (!get_ip_type(ip_v4).has_value() || (get_ip_type(ip_v4).has_value() && (get_ip_type(ip_v4).value() == ip_address_v4_type_Link_local_addresses || 
                                                                                       get_ip_type(ip_v4).value() == ip_address_v4_type_Local_communications)))
            {
                we_have_ip_v4 = true;
                break;
            }
        for (auto& ip_v6 : ips_v6)
            if (!get_ip_type(ip_v6).has_value())
            {
                we_have_ip_v6 = true;
                break;
            }

        for (auto& ip_v4 : IPsV4)
        {
            if (we_have_ip_v4)
                ip_v4.second = ping_v4(ip_v4.first, 1000, Sequence);
            else
                ip_v4.second = -5;
            Sequence++;
        }
        for (auto& ip_v6 : IPsV6)
        {
            if (we_have_ip_v6)
                ip_v6.second = ping_v6(ip_v6.first, 1000, Sequence);
            else
                ip_v6.second = -5;
            Sequence++;
        }

        UL.lock();
        for (auto& device : devices)
            if (device.NeedToPing)
            {
                for (auto& device_ip_info : device.IPsV4)
                {
                    auto iter = std::find_if(IPsV4.begin(), IPsV4.end(), [device_ip_info](const std::pair<ip_address_v4, int16_t>& _pair_)
                        { return device_ip_info.get_ip() == _pair_.first; });
                    if (iter != IPsV4.end())
                        device_ip_info.add_new_ping(iter->second);
                }
                for (auto& device_ip_info : device.IPsV6)
                {
                    auto iter = std::find_if(IPsV6.begin(), IPsV6.end(), [device_ip_info](const std::pair<ip_address_v6, int16_t>& _pair_)
                        { return device_ip_info.get_ip() == _pair_.first; });
                    if (iter != IPsV6.end())
                        device_ip_info.add_new_ping(iter->second);
                }
            }
        UL.unlock();

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    void getting_an_ip_outside_func()
    {
        std::this_thread::sleep_for(std::chrono::seconds(3));

        auto ip_v4 = Connections::Ethernet::Utils::get_external_IPv4();
        std::unique_lock UL(outside_ips_mutex); outside_ip_v4 = ip_v4; UL.unlock();

        auto ip_v6 = Connections::Ethernet::Utils::get_external_IPv6();
        UL.lock(); outside_ip_v6 = ip_v6; UL.unlock();
    }

    std::vector<Device_t>        devices;
    mutable std::recursive_mutex devices_mutex;
    std::vector<Link_t>          links;
    mutable std::recursive_mutex links_mutex;

    ClassThreadedFunc<DevicesList_t>      background_pings_func_TF;
    ClassThreadedFunc<DevicesList_t> getting_an_ip_outside_func_TF;
};

#endif // !DEVICES_LIST_HPP
