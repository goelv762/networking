# Networking
Simple socket based TCP networking

## Description
Using UDP for broadcasting for the server and TCP to send and receive packets, thhis project allows for simple client to client communication over a centeral server. At the moment, all devices must be connected to the same LAN connection for the connections to work.

## Compilation and Running
Only tested on linux. Create a build folder and compile using CMake. In the build folder you should find an executable for the server and client. Run the server on one device and connect to it over LAN from instances of the client (note: clients can be on the same device as the server!)
