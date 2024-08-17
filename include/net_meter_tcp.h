#ifndef NET_METER_TCP_H
#define NET_METER_TCP_H

#include "net_meter.h"
#include <string>

class net_meter_tcp : public net_meter {
public:
    net_meter_tcp(const std::string& target_address_value, int num_requests_value, int timeout_value, int port_value, const std::string& message_value);
protected:
    void perform_measurement() override;
private:
    std::string message;
    int port;
};

#endif // NET_METER_TCP_H
