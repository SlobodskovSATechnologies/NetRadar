# NetRadar
A useful cross-platform visual interface for managing large networks (using ping, traceroute, WoL, and many others), creating network diagrams with a large amount of diverse information in one application.

## Dependencies
C++20  
Boost 1.86.0 (ASIO, Beast).  
OpenSSL 3.5.2 LTSC.  
ImGui latest (GLFW, GLUT), ImPlot, ImNodes.  
Custom backlib.  
Custom frontlib.  

## Features

### Representation of the network in the form of connected nodes on a common field
Each node can be a representation of a network device (for example, a router, switch, WiFi access point, client device, etc.) or network technology (for example, a DHCP server, DNS server, virtual machine, etc.), which have many customized fields: MAC addresses, IPv4 addresses, IPv6 addresses, input ports and output ports.  
  
Ports can represent both physical interfaces (e.g. RJ45 Ethernet, SFP connectors, WiFi modem, etc.) and virtual interfaces (VPN services, virtual machine connections, etc.).
### Graphical connections
Each connection also has a distribution by type of physical or virtual connection (like ports on nodes), however, they have additional fields for useful information about the created connection (for example, the length of the RJ45 or SFP connection path, the frequency and channel of the WiFi connection, etc.).

### Color designations
All node colors depend on the state of the connections of this node itself. The node is colored green if ping results of all its connections are less than 100ms. As soon as at least one connection has a ping result of more than 100ms, the node is yellow. If at least one connection cannot receive a ping result, the node turns red. This allows you to quickly draw the user's attention to emerging problems on the network.  

The color of a link indicates the type of connection, not allowing them to be confused visually to evaluate the network switching structure without checking each port for its type.  

## TODO
1) Delete custom libs from dependencies.
2) Migration from ImNodes to ImFlowNodes.
3) Add English language support in the interface and download custom dictionaries.
