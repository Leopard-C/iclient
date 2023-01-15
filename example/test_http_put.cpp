#include "iclient/iclient.h"

void test_http_put() {
    using namespace ic::client;
    Request request("http://httpbin.org/put");
    request.set_http_method(http::Method::HTTP_PUT);
    request.set_header("accept", "application/json");
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
