/*
net_meter - Base class for network latency and jitter measurement
*/
#pragma once

#include <string>
#include <vector>
#include <mutex>

class net_meter {
public:
    net_meter(int num_requests_value, int timeout_value);
    virtual ~net_meter() = default;

    void test();

    virtual long long get_average_latency() const;
    virtual long long get_median_latency() const;
    virtual long long get_average_jitter() const;
    const std::vector<long long>& get_latencies() const;

    // Getters and setters
    int get_num_requests() const;
    void set_num_requests(int num_requests_value);

    int get_timeout() const;
    void set_timeout(int timeout_value);

protected:
    virtual void perform_measurement() = 0;

    int num_requests;
    int timeout;
    std::vector<long long> latencies;
    long long jitter;
    mutable std::mutex mutex;

private:
    void calculate_jitter();
};
