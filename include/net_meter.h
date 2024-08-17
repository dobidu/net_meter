/*
net_meter - Base class for network latency and jitter measurement
*/
#pragma once

#include <string>
#include <vector>
#include <mutex>

//TODO: Generalizar, colocar string address e port, valendo para todos

class net_meter {
public:
    net_meter(const std::string& target_address_value, int num_requests_value, int timeout_value);
    virtual ~net_meter() = default;

    void test();

    virtual long long get_average_latency() const;
    virtual long long get_median_latency() const;
    virtual long long get_average_jitter() const;
    const std::vector<long long>& get_latencies() const;

    // Getters and setters
    const std::string& get_target_address() const;
    void set_target_address(const std::string& target_address_value);

    int get_num_requests() const;
    void set_num_requests(int num_requests_value);

    int get_timeout() const;
    void set_timeout(int timeout_value);

protected:
    virtual void perform_measurement() = 0;

    std::string target_address;
    int num_requests;
    int timeout;
    std::vector<long long> latencies;
    long long jitter;
    mutable std::mutex mutex;

private:
    void calculate_jitter();
};
