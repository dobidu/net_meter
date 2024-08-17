#pragma once

#include "net_meter.h"

class net_meter_http : public net_meter {
public:
    net_meter_http(const std::string& endpoint_url, int num_requests_value, int timeout_value);
protected:
    void perform_measurement() override;
};
