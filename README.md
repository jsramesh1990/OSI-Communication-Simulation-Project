# OSI 7-Layer Network Model Simulation 

A complete implementation of the OSI 7-layer network model in C with real socket communication and Python visualization. This project demonstrates data encapsulation, transmission, and decapsulation across all seven layers with color-coded progress tracking.

[![C Language](https://img.shields.io/badge/C-Programming-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![OSI Model](https://img.shields.io/badge/OSI-7%20Layers-orange.svg)](https://en.wikipedia.org/wiki/OSI_model)
[![Socket Programming](https://img.shields.io/badge/Socket-Communication-green.svg)](https://en.wikipedia.org/wiki/Network_socket)
[![Python](https://img.shields.io/badge/Python-3.x-yellow.svg)](https://python.org)
[![Network Simulation](https://img.shields.io/badge/Network-Simulation-purple.svg)](https://en.wikipedia.org/wiki/Network_simulation)
[![Linux/Ubuntu](https://img.shields.io/badge/Linux-Ubuntu-red.svg)](https://ubuntu.com)
[![Educational](https://img.shields.io/badge/Educational-Project-8b0000.svg)](https://en.wikipedia.org/wiki/Educational_technology)
[![Makefile](https://img.shields.io/badge/Build-Makefile-lightgrey.svg)](https://en.wikipedia.org/wiki/Makefile)

##  Table of Contents
- [Overview](#-overview)
- [Project Structure](#-project-structure)
- [Architecture](#-architecture)
- [Installation](#-installation)
- [Usage](#-usage)
- [OSI Layer Implementation](#-osi-layer-implementation)
- [Visualization](#-visualization)
- [Extending the Project](#-extending-the-project)
- [License](#-license)

##  Overview

This project simulates the complete OSI 7-layer network model with:
- **C-based client/server** implementing full layer encapsulation/decapsulation
- **Real socket communication** between processes
- **Python visualization tool** showing the "zip-zag" data flow
- **Color-coded layer logs** for easy debugging
- **Error simulation** capabilities at various layers

Perfect for understanding network protocols, data encapsulation, and socket programming fundamentals.

##  Project Structure
```
osi-network-simulator/
│
├── src/
│   ├── main.c              # Main application entry point
│   ├── layers.h            # Layer function declarations
│   └── layers.c            # OSI layer implementations
│
├── python/
│   └── visualizer.py       # Python progress visualizer
│
├── tests/
│   └── test_communication.sh
│
├── Makefile                # Build system
└── README.md               # This file
```

##  Architecture

```mermaid
flowchart TD
    subgraph A[Sender (Encapsulation)]
        direction LR
        A1[Application Layer<br/>Raw Data] --> A2[Presentation<br/>Encryption/Format]
        A2 --> A3[Session<br/>Session Control]
        A3 --> A4[Transport<br/>TCP/UDP Headers]
        A4 --> A5[Network<br/>IP Headers]
        A5 --> A6[Data Link<br/>Frame Headers]
        A6 --> A7[Physical<br/>Bit Transmission]
    end
    
    subgraph B[Network Medium]
        B1[Socket Connection<br/>127.0.0.1:8080]
    end
    
    subgraph C[Receiver (Decapsulation)]
        direction LR
        C7[Physical<br/>Bit Reception] --> C6[Data Link<br/>Frame Removal]
        C6 --> C5[Network<br/>IP Removal]
        C5 --> C4[Transport<br/>TCP/UDP Removal]
        C4 --> C3[Session<br/>Session Management]
        C3 --> C2[Presentation<br/>Decryption/Format]
        C2 --> C1[Application Layer<br/>Final Data]
    end
    
    A7 --> B1
    B1 --> C7
    
    subgraph D[Python Visualizer]
        D1[Monitor Progress]
        D2[Color-coded Output]
        D3[Zip-Zag Display]
    end
    
    A1 -.-> D1
    C1 -.-> D1
```

##  Installation

### Prerequisites
```bash
# Install C compiler and Python
sudo apt update
sudo apt install gcc python3 python3-pip

# Install Python dependencies
pip3 install colorama termcolor
```

### Build from Source
```bash
# Clone the repository
git clone https://github.com/yourusername/osi-network-simulator.git
cd osi-network-simulator

# Build the C application
make

# Verify build
ls -la osi
```

##  Usage

### Basic Execution
```bash
# Run the simulation with default message
./osi

# Run with custom message
./osi "Hello from OSI layers!"

# Run with verbose output
./osi -v "Test message"
```

### Python Visualization
```bash
# Run the Python visualizer
python3 python/visualizer.py

# Visualize with custom data
python3 python/visualizer.py --message "Network test"
```

### Example Output
```
 OSI 7-Layer Simulation Started
──────────────────────────────────────

 Layer 7: Application
 Data: "Hello World!"
 Added Application header: AppHdr

 Layer 6: Presentation
 Encrypted data
 Added Presentation header: PresHdr

... [continues through all layers] ...

 Layer 1: Physical
 Transmitting bits: 0101010101...

 Transmission complete!
──────────────────────────────────────

 Starting Decapsulation...
 Layer 1: Physical → Bits received
 Layer 2: Data Link → Frame processed

... [continues decapsulation] ...

 Layer 7: Application
 Final data: "Hello World!"
 Transmission successful!
```

## 🔧 OSI Layer Implementation

Each layer implements two main functions:

| Layer | Function | Responsibility | Example Headers |
|-------|----------|----------------|-----------------|
| **7. Application** | `app_send()` / `app_recv()` | User data interface | HTTP, FTP, SMTP |
| **6. Presentation** | `present_send()` / `present_recv()` | Encryption, compression | SSL, TLS headers |
| **5. Session** | `session_send()` / `session_recv()` | Session management | Session tokens |
| **4. Transport** | `transport_send()` / `transport_recv()` | End-to-end communication | TCP/UDP headers |
| **3. Network** | `network_send()` / `network_recv()` | Routing, addressing | IP headers |
| **2. Data Link** | `datalink_send()` / `datalink_recv()` | Frame creation, error detection | Ethernet headers |
| **1. Physical** | `physical_send()` / `physical_recv()` | Bit transmission | Raw bits |

### Code Example
```c
// Simplified layer implementation
void transport_send(char* data) {
    printf("📦 [Transport] Adding TCP header to: %s\n", data);
    char* tcp_data = add_header(data, "TCP_HDR");
    network_send(tcp_data);
}

void transport_receive(char* data) {
    printf("📦 [Transport] Removing TCP header\n");
    char* original_data = remove_header(data, "TCP_HDR");
    session_receive(original_data);
}
```

##  Visualization Features

The Python visualizer provides:

### Color-Coded Output
```python
# Layer colors for easy identification
LAYER_COLORS = {
    'Application': Fore.GREEN,
    'Presentation': Fore.CYAN,
    'Session': Fore.MAGENTA,
    'Transport': Fore.YELLOW,
    'Network': Fore.BLUE,
    'DataLink': Fore.RED,
    'Physical': Fore.WHITE
}
```

### Zip-Zag Progress Display
```
┌──────────────────────────────────────┐
│      OSI Model Progress Flow         │
├──────────────────────────────────────┤
│ App     → Pres    → Sess    → Trans  │
│ Trans   → Network → DataLink→ Phys   │
│ Phys    → DataLink→ Network → Trans  │
│ Trans   → Sess    → Pres    → App    │
└──────────────────────────────────────┘
```

### Real-time Timestamps
```
[2024-01-15 14:30:45] Layer 3: Network processing
[2024-01-15 14:30:46] Layer 2: Frame created
```

##  Extending the Project

### Add Error Simulation
```c
// In layers.c
void simulate_error(int layer, int error_type) {
    switch(error_type) {
        case ERROR_CORRUPTION:
            printf("❌ Layer %d: Simulating data corruption\n", layer);
            break;
        case ERROR_DROP:
            printf("❌ Layer %d: Simulating packet drop\n", layer);
            break;
        case ERROR_DELAY:
            printf("⏱️ Layer %d: Simulating transmission delay\n", layer);
            sleep(2);
            break;
    }
}
```

### Add Command Line Options
```bash
# Future enhancement examples
./osi --error-layer 3 --error-type corruption "Test"
./osi --verbose --log-file network.log "Data"
./osi --port 9090 --ip 192.168.1.100 "Remote test"
```

### Add Multi-threading
```c
// Example for parallel layer processing
pthread_t layer_threads[7];
for (int i = 0; i < 7; i++) {
    pthread_create(&layer_threads[i], NULL, process_layer, &layer_data[i]);
}
```

## 🧪 Testing

```bash
# Run the test suite
cd tests
./test_communication.sh

# Test specific features
./osi "Short test"
./osi "$(cat large_file.txt)"  # Test with large data
./osi --stress-test 1000       # Send 1000 packets
```

##  Performance Metrics

The simulation tracks:
- **Encapsulation time** per layer
- **Transmission latency**
- **Decapsulation time** per layer
- **Total round-trip time**
- **Memory usage** per layer

##  Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/new-layer`)
3. Commit changes (`git commit -m 'Add new error simulation'`)
4. Push to branch (`git push origin feature/new-layer`)
5. Open a Pull Request

### Suggested Contributions
- Add IPv6 support
- Implement real encryption in Presentation layer
- Add Wireshark-compatible packet capture
- Create Docker container for easy deployment
- Add GUI visualization with Tkinter/PyQt

##  License

MIT License - See LICENSE file for details.

##  Acknowledgments

- Inspired by university networking courses
- Based on the OSI model standard (ISO/IEC 7498-1)
- Uses socket programming concepts from Beej's Guide

---

##  Quick Start Guide

```bash
# 1. Build and run
make && ./osi "Learning OSI layers!"

# 2. Visualize with Python
python3 python/visualizer.py --animate

# 3. Test with errors
./osi --simulate-errors "Error test"

# 4. Clean up
make clean
```

**Ready to explore networking? Start with `./osi "Your message"` and watch the data flow through all seven layers!**
