#pragma once

#include "net_meter.h"

class net_meter_ping : public net_meter {
public:
    net_meter_ping(const std::string& target_address_value, int num_requests_value, int timeout_value);
protected:
    void perform_measurement() override;
};
