# net_meter

`net_meter` is a C++ library for measuring network latency and jitter over different protocols. It currently supports HTTP, ICMP (ping), TCP, and UDP.

## Features

- **HTTP Latency Measurement:** Measures the time taken to receive a response from a specified HTTP endpoint.
- **ICMP (Ping) Latency Measurement:** Measures the round-trip time to a specified host using the ICMP protocol.
- **TCP Latency Measurement:** Measures the time taken to establish a TCP connection to a specified host and receive a response.
- **UDP Latency Measurement:** Measures the round-trip time for sending a UDP packet to a specified host and receiving a response.

## Dependencies

- **C++17** or later
- **cpr** - A C++ HTTP client library.

## Installation

1. **Clone the repository:**

   ```sh
   git clone https://github.com/yourusername/net_meter.git
   cd net_meter
   ```

2. **Build the project:**

   The `CMakeLists.txt` is configured to automatically fetch the `cpr` library. Just run the following commands to build the project:

   ```sh
   mkdir build
   cd build
   cmake ..
   make
   ```

## Suggested Echo Servers

### TCP Echo Server

To utilize `net_meter_tcp` effectively, it's essential to have a TCP echo server running on the host that will act as the counterpart for latency and jitter measurements. Here's a simple implementation you can use:

```cpp
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }

    int port = std::stoi(argv[1]);
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server_addr = {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_fd, 5);

    while (true) {
        int client_fd = accept(server_fd, nullptr, nullptr);
        char buffer[1024] = {0};
        ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
        send(client_fd, buffer, bytes_received, 0);
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
```

### UDP Echo Server

For `net_meter_udp` the same logic applies, here’s a simple UDP echo server implementation:

```cpp
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }

    int port = std::stoi(argv[1]);
    int server_fd = socket(AF_INET, SOCK_DGRAM, 0);

    sockaddr_in server_addr = {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr));

    while (true) {
        char buffer[1024] = {0};
        sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        ssize_t bytes_received = recvfrom(server_fd, buffer, sizeof(buffer), 0, (sockaddr*)&client_addr, &client_addr_len);
        sendto(server_fd, buffer, bytes_received, 0, (sockaddr*)&client_addr, client_addr_len);
    }

    close(server_fd);
    return 0;
}
```

## Usage

### Examples

#### HTTP Latency Measurement

```cpp
#include "net_meter_http.h"

int main() {
    net_meter_http http_tester("https://example.com/api/endpoint", 10, 5000);
    http_tester.test();
    std::cout << "Average HTTP Latency: " << http_tester.get_average_latency() << " ms" << std::endl;

    return 0;
}
```

#### ICMP (Ping) Latency Measurement

```cpp
#include "net_meter_ping.h"

int main() {
    net_meter_ping ping_tester("8.8.8.8", 10, 5000);
    ping_tester.test();
    std::cout << "Average Ping Latency: " << ping_tester.get_average_latency() << " ms" << std::endl;

    return 0;
}
```

#### TCP Latency Measurement

```cpp
#include "net_meter_tcp.h"

int main() {
    net_meter_tcp tcp_tester("your.server.address", 10, 5000, 12345, "Hello, TCP!");
    tcp_tester.test();
    std::cout << "Average TCP Latency: " << tcp_tester.get_average_latency() << " ms" << std::endl;

    return 0;
}
```

#### UDP Latency Measurement

```cpp
#include "net_meter_udp.h"

int main() {
    net_meter_udp udp_tester("your.server.address", 10, 5000, 12345, "Hello, UDP!");
    udp_tester.test();
    std::cout << "Average UDP Latency: " << udp_tester.get_average_latency() << " ms" << std::endl;

    return 0;
}
```

### Running the Tests

To measure latency and jitter using the provided subclasses:

1. **Start the TCP or UDP echo server** on the target machine using the provided implementations if using `net_meter_tcp` or `net_meter_udp`.
2. **Run the net_meter tool** using one of the provided examples or your own implementation.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
