#include "net_meter.h"
#include <numeric>
#include <cmath>
#include <algorithm>

net_meter::net_meter(const std::string& target_address_value, int num_requests_value, int timeout_value)
    : target_address(target_address_value), num_requests(num_requests_value), timeout(timeout_value), jitter(0) {}

void net_meter::test() {
    latencies.clear();
    for (int i = 0; i < num_requests; ++i) {
        perform_measurement();
    }
    calculate_jitter();
}

long long net_meter::get_average_jitter() const {
    std::lock_guard<std::mutex> lock(mutex);  // Ensure thread safety
    return jitter;
}

long long net_meter::get_average_latency() const {
    std::lock_guard<std::mutex> lock(mutex);  // Ensure thread safety
    long long sum = 0;
    for (const auto& latency : latencies) {
        sum += latency;
    }
    return latencies.empty() ? 0 : sum / latencies.size();
}

long long net_meter::get_median_latency() const {
    std::lock_guard<std::mutex> lock(mutex);  // Ensure thread safety
    if (latencies.empty()) return 0;
    std::vector<long long> sorted_latencies = latencies;
    std::sort(sorted_latencies.begin(), sorted_latencies.end());
    size_t middle = sorted_latencies.size() / 2;
    return sorted_latencies.size() % 2 == 0 ?
           (sorted_latencies[middle - 1] + sorted_latencies[middle]) / 2 :
           sorted_latencies[middle];
}

const std::vector<long long>& net_meter::get_latencies() const {
    std::lock_guard<std::mutex> lock(mutex);  // Ensure thread safety
    return latencies;
}

int net_meter::get_num_requests() const {
    return num_requests;
}

void net_meter::set_num_requests(int num_requests_value) {
    num_requests = num_requests_value;
}

int net_meter::get_timeout() const {
    return timeout;
}

void net_meter::set_timeout(int timeout_value) {
    timeout = timeout_value;
}

const std::string& net_meter::get_target_address() const {
    return target_address;
}

void net_meter::set_target_address(const std::string& target_address_value) {
    target_address = target_address_value;
}

void net_meter::calculate_jitter() {
    if (latencies.size() > 1) {
        double mean = std::accumulate(latencies.begin(), latencies.end(), 0.0) / latencies.size();
        double sq_sum = std::inner_product(latencies.begin(), latencies.end(), latencies.begin(), 0.0);
        jitter = std::sqrt(sq_sum / latencies.size() - mean * mean);
    } else {
        jitter = 0;
    }
}
