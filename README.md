# NetRadar
![preview](https://github.com/SlobodskovSATechnologies/NetRadar/blob/main/github/readme/Preview.png)  

## Dependencies
C++20  
Boost 1.91.0 (ASIO, Beast)  
OpenSSL 4.0.0  
ImGui latest (GLFW, GLUT), ImPlot, ImNodes  
Custom backlib  
Custom frontlib  
[More info on wiki](https://github.com/SlobodskovSATechnologies/NetRadar/wiki/Dependencies-install)  

## Features

### Representation of the network in the form of connected nodes on a common field
Each node can be a representation of a network device (for example, a router, switch, WiFi access point, client device, etc.) or network technology (for example, a DHCP server, DNS server, virtual machine, etc.), which have many customized fields: MAC addresses, IPv4 addresses, IPv6 addresses, input ports and output ports.  
  
Ports can represent both physical interfaces (e.g. RJ45 Ethernet, SFP connectors, WiFi modem, etc.) and virtual interfaces (VPN services, virtual machine connections, etc.).  
![node](https://github.com/SlobodskovSATechnologies/NetRadar/blob/main/github/readme/Node.png)  

### Graphical connections
Each connection also has a distribution by type of physical or virtual connection (like ports on nodes), however, they have additional fields for useful information about the created connection (for example, the length of the RJ45 or SFP connection path, the frequency and channel of the WiFi connection, etc.).  
![link](https://github.com/SlobodskovSATechnologies/NetRadar/blob/main/github/readme/Link.png)  

### Color designations
All node colors depend on the state of the connections of this node itself. The node is colored green if ping results of all its connections are less than 100ms. As soon as at least one connection has a ping result of more than 100ms, the node is yellow. If at least one connection cannot receive a ping result, the node turns red. This allows you to quickly draw the user's attention to emerging problems on the network.  
![G](https://github.com/SlobodskovSATechnologies/NetRadar/blob/main/github/readme/Green.png)  
![Y](https://github.com/SlobodskovSATechnologies/NetRadar/blob/main/github/readme/Yellow.png)  
![R](https://github.com/SlobodskovSATechnologies/NetRadar/blob/main/github/readme/Red.png)  
The color of a link indicates the type of connection, not allowing them to be confused visually to evaluate the network switching structure without checking each port for its type.  
[More info on wiki](https://github.com/SlobodskovSATechnologies/NetRadar/wiki/Get-started)  

### Tools
Built-in simple diagnostic and management tools (for example, ping, tracerote, WoL, etc.).  
![t1](https://github.com/SlobodskovSATechnologies/NetRadar/blob/main/github/readme/Traceroute.png)
![t2](https://github.com/SlobodskovSATechnologies/NetRadar/blob/main/github/readme/WoL.png)
![t3](https://github.com/SlobodskovSATechnologies/NetRadar/blob/main/github/readme/DNS.png)
![t4](https://github.com/SlobodskovSATechnologies/NetRadar/blob/main/github/readme/rDNS.png)
  
[More info on wiki](https://github.com/SlobodskovSATechnologies/NetRadar/wiki/Tools)  

## TODO
1) Delete custom libs from dependencies.
2) Migration from ImNodes to ImFlowNodes.
3) Add English language support in the interface and download custom dictionaries.
