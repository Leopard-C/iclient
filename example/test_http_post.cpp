#include "iclient/iclient.h"

void test_http_post() {
    using namespace ic::client;
    Request request("http://httpbin.org/post");
    request.set_http_method(http::Method::HTTP_POST);
    request.set_header("accept", "application/json");

    auto response = request.Perform();

    printf("----------------------------------------------\n");
    printf("%s\n", to_string(response.status()));
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
