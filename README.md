📘 OSI-MULTI-PLATFORM-PROJECT

A complete multi-platform OSI model simulation involving:

Client (C program)

Server (C program)

User Application (Python)

The Client and Server each implement a full 7-layer OSI stack (encapsulation + decapsulation).
The Python user application visualizes the communication by printing OSI layer progress after every 2 layers are completed (zip-zag style).

📂 Project Structure
OSI-MULTI-PLATFORM-PROJECT/
│
├── user_app/
│   ├── python_app.py
│
├── server/
│   ├── main.c
│   ├── layers.c
│   ├── layers.h
│   ├── Makefile
│
└── client/
    ├── main.c
    ├── layers.c
    ├── layers.h
    ├── Makefile

🧠 Project Overview

This project demonstrates multi-platform communication using OSI principles:

✔ Client (C)

Encapsulates message through each OSI layer.

Sends structured data (JSON format) to the server.

Receives reply from server and decapsulates.

✔ Server (C)

Receives encapsulated message.

Decapsulates through 7 OSI layers.

Re-encapsulates reply and sends back.

✔ User Application (Python)

Connects to Client.

Receives OSI layer logs.

Prints OSI flow two layers at a time:

Example output:

[TX]
Application → Presentation
  ✔ Layers 1-2 completed

Session → Transport
  ✔ Layers 3-4 completed

Network → DataLink
  ✔ Layers 5-6 completed

Physical
  ✔ Layer 7 completed


This is your requested zip-zag visualization.

🧱 OSI Layers Implemented

The following layers are simulated:

Application

Presentation

Session

Transport

Network

DataLink

Physical

Each layer has:

LayerName_encapsulate()
LayerName_decapsulate()


Registered inside layers.c.

⚙️ How It Works
1️⃣ Client → Server (TX Flow)
Application
Presentation
Session
Transport
Network
DataLink
Physical


Each layer adds a header.

2️⃣ Server processing (RX → process → TX)

The server:

Decapsulates

Processes data

Re-encapsulates a response

Sends back to client

3️⃣ Python App displays ZIP-ZAG

The Python user app reads:

{
  "direction": "TX",
  "layers": ["Application", "Presentation", ...],
  "payload": "Hello"
}


And prints two layers at a time.

🛠 Build Instructions
Build Server
cd server
make

Build Client
cd client
make

▶️ Run Instructions
Start Server
./server

Start Client
./client

Start Python Application
python3 python_app.py

📡 Communication Flow
Python User App  ⇄  Client (C OSI stack) ⇄ Server (C OSI stack)


Python only receives logs and visualizes progress.

All OSI logic is performed in C on both ends.

🔧 Technologies Used

C (Server/Client networking + OSI stack)

Python 3 (User-level visualization)

POSIX Sockets

JSON text protocol

🚀 Future Enhancements

Color-coded terminal layers

Retransmission & packet corruption simulation

Multithreaded server

UDP mode

Real NIC info injection

👤 Author

Developed by Sebastian Ramesh S

Embedded Systems Developer
Embedded Linux Developer
Yocto BSP Engineer
Linux Device Driver Engineer

📜 License

This project is open and free to use in your GitHub repositories.
