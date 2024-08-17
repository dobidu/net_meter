#include "net_meter_udp.h"
#include <chrono>
#include <stdexcept>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

net_meter_udp::net_meter_udp(const std::string& target_address_value, int num_requests_value, int timeout_value, int port_value, const std::string& message_value)
    : net_meter(target_address_value, num_requests_value, timeout_value), port(port_value), message(message_value) { }

void net_meter_udp::perform_measurement() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        throw std::runtime_error("Failed to create UDP socket");
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

    for (int i = 0; i < num_requests; ++i) {
        auto start = std::chrono::steady_clock::now();
        ssize_t sent_bytes = sendto(sockfd, message.c_str(), message.length(), 0, (sockaddr*)&addr, sizeof(addr));
        if (sent_bytes < 0) {
            close(sockfd);
            throw std::runtime_error("Failed to send UDP packet");
        }

        char* buffer = (char*)malloc(message.length()+1);
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(sockfd, &fds);
        timeval tv{};
        tv.tv_sec = timeout / 1000;
        tv.tv_usec = (timeout % 1000) * 1000;

        int result = select(sockfd + 1, &fds, nullptr, nullptr, &tv);
        if (result > 0) {
            ssize_t received_bytes = recvfrom(sockfd, buffer, sizeof(buffer), 0, nullptr, nullptr);
            if (received_bytes > 0) {
                auto end = std::chrono::steady_clock::now();
                auto latency = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
                latencies.push_back(latency);
            }
        } else {
            latencies.push_back(timeout);
        }
    }

    close(sockfd);
}
