**Project Structure**

osi_project/
├── main.c
├── layers.c
├── layers.h
└── Makefile

**How It Works**

When the program runs:

Application layer starts with the raw message.

Each OSI layer adds its own header.

The message passes down to the Physical layer.

The Physical layer simulates transmission.

Receiver side begins decapsulation.

Each layer removes its header.

Final message reaches the receiving Application layer.

The project demonstrates full TX → RX communication.

**How To Build and Run**
1️⃣ Build the program

Run:

make
2️⃣ Run the binary
./osi

**OSI Layer Mapping in Code**

Each layer has two functions:

Send path (TX): Layer_Send()

Receive path (RX): Layer_Receive()


**Extend the Project**

You can enhance it by adding:

Real TCP sockets

Logging to files

Multi-threaded sender/receiver

Error simulation at layers

Packet corruption and retry logic

If you'd like these upgrades, just ask!

**Author**

Created to demonstrate complete OSI layer data flow using simple C code.

**License**

This project is open and free to use in your GitHub repositories.
