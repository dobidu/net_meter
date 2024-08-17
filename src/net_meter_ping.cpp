#include "net_meter_ping.h"
#include <cstring>
#include <chrono>
#include <stdexcept>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

// Utility function to calculate checksum for ICMP packet
uint16_t checksum(void* data, int len) {
    uint16_t* buffer = static_cast<uint16_t*>(data);
    uint32_t sum = 0;

    for (; len > 1; len -= 2) {
        sum += *buffer++;
    }
    if (len == 1) {
        sum += *reinterpret_cast<uint8_t*>(buffer);
    }
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    return ~sum;
}

net_meter_ping::net_meter_ping(const std::string& target_address_value, int num_requests_value, int timeout_value)
    : net_meter(target_address_value,num_requests_value, timeout_value) {}

void net_meter_ping::perform_measurement() {
    if (geteuid() != 0) {
        throw std::runtime_error("ICMP ping requires root privileges");
    }
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        throw std::runtime_error("Socket creation failed");
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
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

    char packet[64];
    struct icmp* icmp_header = reinterpret_cast<struct icmp*>(packet);
    icmp_header->icmp_type = ICMP_ECHO;
    icmp_header->icmp_code = 0;
    icmp_header->icmp_id = getpid();
    icmp_header->icmp_seq = 0;
    icmp_header->icmp_cksum = 0;

    icmp_header->icmp_cksum = checksum(packet, sizeof(packet));

    auto start = std::chrono::steady_clock::now();
    ssize_t sent_bytes = sendto(sockfd, packet, sizeof(packet), 0, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
    if (sent_bytes < 0) {
        close(sockfd);
        throw std::runtime_error("Failed to send ICMP packet");
    }

    char buffer[1024];
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
        latencies.push_back(timeout); // Timeout considered as max latency
    }

    close(sockfd);
}
