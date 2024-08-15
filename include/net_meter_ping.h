#pragma once

#include "net_meter.h"

class net_meter_ping : public net_meter {
public:
    net_meter_ping(const std::string& target_ip, int num_requests_value, int timeout_value);
    std::string get_target_address() const;
    
protected:
    void perform_measurement() override;

private:
    std::string target_address;
};
