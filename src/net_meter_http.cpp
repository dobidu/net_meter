#include "net_meter_http.h"
#include <cpr/cpr.h>
#include <chrono>
#include <stdexcept>

net_meter_http::net_meter_http(const std::string& endpoint_url, int num_requests_value, int timeout_value)
    : net_meter(num_requests_value, timeout_value), endpoint_url(endpoint_url) {}

void net_meter_http::perform_measurement() {
    auto start = std::chrono::steady_clock::now();
    cpr::Response r = cpr::Get(cpr::Url{ endpoint_url }, cpr::Timeout{ timeout });
    auto end = std::chrono::steady_clock::now();

    if (r.error) {
        throw std::runtime_error("Request failed: " + r.error.message);
    } else {
        auto latency = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        latencies.push_back(latency);
    }
}

std::string net_meter_http::get_endpoint_url() const {
    return endpoint_url;
}
