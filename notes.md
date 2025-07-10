- A VPN provides ethernet exchange services to devices connected to the
different ports of the switch. 
- A network switch is a device which uses packet switching to forward data to the destination device.
- a multiport(operates through multiple ports)
- device which uses MAC address which forwards the data to the next data layer
- If you include routing functionality it can also forward data on the third layer.(these are called multilayer switches)
- We are going to use switches which recognise and forward Ethernet frames.
- when forwarding packets, switches which can look up 
- We have the mac address table which can be used to look up exchange
- now what is a ethernet frame a data unit in the data link layer, it is preceded by preamble at start and SFD start frame delimeter at end and then we have payload at middle consisting of mac address of source and destination, payload, also which has headers for internet protocol, then we have FCS(32 bits ), which can tell us if its corrupted or not. Preamble is 56 bits of alternating 1 and 0.
 VPort connects computer through TAP  and connects UDP to switch
