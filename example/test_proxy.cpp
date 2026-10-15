#include "iclient/iclient.h"

void test_proxy() {
    using namespace ic::client;
    Request request("http://httpbin.org/ip");

    ProxyData pd;
    pd.type = ProxyType::HTTP;
    pd.host = "127.0.0.1";
    pd.port = 10809;
    request.set_proxy(pd);

    printf("ProxyInfo: [%s] %s:%u\n", to_string(pd.type), pd.host.c_str(), pd.port);

    auto response = request.Perform();

    printf("----------------------------------------------\n");
    printf("%s\n", ic::client::to_string(response.status()));
    printf("----------------------------------------------\n");
    if (response.ok()) {
        // Header
        printf("------ Headers -------------------------------\n");
        for (auto& header : response.headers()) {
            printf("%s: %s\n", header.first.c_str(), header.second.c_str());
        }
        // Data
        printf("------ Body ----------------------------------\n");
        printf("%s\n", response.data().c_str());
        printf("---------------------------------------------\n");
    }
    printf("\n\n");
}
