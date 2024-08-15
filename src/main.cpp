#include <iostream>
#include <thread>
#include <vector>
#include "net_meter.h"
#include "net_meter_http.h"
#include "net_meter_ping.h"

void run_latency_test(net_meter& tester, int thread_id) {
    try {
        std::cout << "Thread " << thread_id << " starting latency test..." << std::endl;
        tester.test();
        std::cout << "Thread " << thread_id << " completed latency test." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Thread " << thread_id << " encountered an error: " << e.what() << std::endl;
    }
}

int main() {
    net_meter_http http_tester("https://jsonplaceholder.typicode.com/posts/1", 10, 5000);
    net_meter_ping ping_tester("8.8.8.8", 10, 5000);

    const int num_threads = 2;
    std::vector<std::thread> threads;

    threads.push_back(std::thread(run_latency_test, std::ref(http_tester), 0));
    threads.push_back(std::thread(run_latency_test, std::ref(ping_tester), 1));

    for (auto& thread : threads) {
        thread.join();
    }    

    std::cout << std::endl;
    std::cout << "All threads completed latency tests." << std::endl;
    std::cout << std::endl;
    std::cout << "Target address for ping test: " << ping_tester.get_target_address() << std::endl;
    std::cout << "Latencies for ping test: ";
    for (const auto& latency : ping_tester.get_latencies()) {
        std::cout << latency << " ";
    }
    std::cout << std::endl;
    std::cout << "Average latency for ping test: " << ping_tester.get_average_latency() << " ms" << std::endl;
    std::cout << "Median latency for ping test: " << ping_tester.get_median_latency() << " ms" << std::endl;
    std::cout << "Average jitter for ping test: " << ping_tester.get_average_jitter() << " ms" << std::endl;
    std::cout << std::endl;
    std::cout << "Endpoint URL for HTTP test: " << http_tester.get_endpoint_url() << std::endl;
    std::cout << "Latencies for HTTP test: ";
    for (const auto& latency : http_tester.get_latencies()) {
        std::cout << latency << " ";
    }
    std::cout << std::endl;
    std::cout << "Average latency for HTTP test: " << http_tester.get_average_latency() << " ms" << std::endl;
    std::cout << "Median latency for HTTP test: " << http_tester.get_median_latency() << " ms" << std::endl;
    std::cout << "Average jitter for HTTP test: " << http_tester.get_average_jitter() << " ms" << std::endl;

    return 0;
}
