# net_meter

The **net_meter** is a C++ project that measures network latency and jitter using different protocols. Currently, it supports HTTP requests through the `net_meter_http` subclass and ICMP ping through the `net_meter_ping` subclass.

## Features

- Measure latency and jitter for HTTP requests.
- Measure latency for ICMP ping requests.
- Configurable number of requests and timeout values.
- Simple and extensible design for future protocols.

## Table of Contents

- [Installation](#installation)
- [Usage](#usage)
  - [Net Meter HTTP](#net-meter-http)
  - [Net Meter Ping](#net-meter-ping)
- [Contributing](#contributing)
- [License](#license)

## Installation

### Prerequisites

Make sure you have the following dependencies installed:

- C++17 or later
- CMake
- [cpr](https://github.com/libcpr/cpr) for HTTP requests (submodule included)
- Permissions for raw socket usage (for ICMP ping)

### Clone the Repository

```bash
git clone --recurse-submodules https://github.com/dobidu/net_meter
cd net_meter
```

### Build the Project

```bash
mkdir build
cd build
cmake ..
make
```

## Usage

### Net Meter HTTP

To use the `net_meter_http` subclass to measure HTTP latency and jitter:

```cpp
#include "net_meter_http.h"

int main() {
    net_meter_http http_meter("http://example.com/api", 10, 5000);
    http_meter.perform_measurement();

    std::cout << "Average Latency: " << http_meter.get_average_latency() << " ms" << std::endl;
    std::cout << "Average Jitter: " << http_meter.get_average_jitter() << " ms" << std::endl;

    return 0;
}
```

### Net Meter Ping

To use the `net_meter_ping` subclass to measure ICMP ping latency:

```cpp
#include "net_meter_ping.h"

int main() {
    net_meter_ping ping_meter("8.8.8.8", 10, 1000);
    ping_meter.perform_measurement();

    std::cout << "Average Latency: " << ping_meter.get_average_latency() << " ms" << std::endl;
    
    return 0;
}
```

## Contributing

Contributions are welcome! Please follow these steps:

1. Fork the repository.
2. Create a new branch for your feature or bug fix.
3. Make your changes and commit them.
4. Push your changes to your forked repository.
5. Open a pull request describing your changes.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
