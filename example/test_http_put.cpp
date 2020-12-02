#include "../iclient/ic/ic.h"

void test_http_put() {
    ic::Request request("http://httpbin.org/put");
    request.setMethod(ic::http::Method::PUT);
    request.setHeader("accept", "application/json");
    ic::Response response = request.perform();

    printf("----------------------------------------------\n");
    printf("%s\n", ic::to_string(response.getStatus()).c_str());
    printf("----------------------------------------------\n");
    if (response.getStatus() == ic::Status::SUCCESS && response.getHttpStatus() == ic::http::StatusCode::HTTP_200_OK) {
        // Header
        printf("------ Headers -------------------------------\n");
        auto& headers = response.getHeaders();
        for (auto& header : headers) {
            printf("%s: %s\n", header.first.c_str(), header.second.c_str());
        }
        // Data
        printf("------ Body ----------------------------------\n");
        printf("%s\n", response.getData().c_str());
        printf("---------------------------------------------\n");
    }
    printf("\n\n");
}
