#include "net_meter_tcp.h"
#include <chrono>
#include <stdexcept>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

net_meter_tcp::net_meter_tcp(const std::string& target_address_value, int num_requests_value, int timeout_value, int port_value, const std::string& message_value)
    : net_meter(target_address_value, num_requests_value, timeout_value), port(port_value), message(message_value) {
}

void net_meter_tcp::perform_measurement() {
    for (int i = 0; i < num_requests; ++i) {
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            throw std::runtime_error("Failed to create TCP socket");
        }

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);

        //Test whether the target address stores an IP address or a domain name
        //If it is a domain name, it is resolved to an IP address
        if (inet_addr(target_address.c_str()) == INADDR_NONE) {
            hostent* host = gethostbyname(target_address.c_str());
            if (host == nullptr) {
                close(sockfd);
                throw std::runtime_error("Failed to resolve hostname");
            }
            addr.sin_addr.s_addr = *reinterpret_cast<unsigned long*>(host->h_addr_list[0]);
        } else {
            addr.sin_addr.s_addr = inet_addr(target_address.c_str());
        }        

        auto start = std::chrono::steady_clock::now();
        if (connect(sockfd, (sockaddr*)&addr, sizeof(addr)) < 0) {
            close(sockfd);
            throw std::runtime_error("Failed to connect to TCP server");
        }

        ssize_t sent_bytes = send(sockfd, message.c_str(), message.length(), 0);
        if (sent_bytes < 0) {
            close(sockfd);
            throw std::runtime_error("Failed to send TCP packet");
        }

        char* buffer = (char*)malloc(message.length()+1);
        ssize_t received_bytes = recv(sockfd, buffer, sizeof(buffer), 0);
        if (received_bytes > 0) {
            auto end = std::chrono::steady_clock::now();
            auto latency = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            latencies.push_back(latency);
        } else {
            latencies.push_back(timeout);
        }

        close(sockfd);
    }
}
