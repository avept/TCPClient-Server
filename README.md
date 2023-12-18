# (WinApi) TCP Client and Server 

## Software requirements
(TODO)CMake and at least C++17 

## Overview
This project features a TCP client-server system using the Windows API (WinAPI). 
The client establishes a connection with the server, transmitting an array of numbers. The server, in turn, identifies the maximum number from the received array.

## Features
- **TCP Client**: Initiates a connection to the server and sends an array of numbers.
- **TCP Server**: Listens for incoming connections, receives arrays from clients, and determines the maximum number.
  If the client sends invalid data (non-array), the server responds accordingly.


## Build and Usage

Follow these steps to build and run TCPClient/Server:

**(TODO)**
1. Clone the TCPClient/Server repository to your local machine:
```bash
https://github.com/avept/TCPClient-Server.git
```

2. Navigate to the project directory:
```bash 
cd TCPClient/Server
```

3. Create a build directory:
```bash
mkdir build
cd build
```
   
4. Generate the build system using CMake:
```bash
cmake ..
```

5. Build the library:
```bash
make -j4
```
