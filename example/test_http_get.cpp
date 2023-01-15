#include "iclient/iclient.h"

void test_http_get() {
    ic::client::Request request("http://httpbin.org/ip");
    ic::client::Response response = request.Perform();

    printf("---------------------------------------------\n");
    printf("%s\n", ic::client::to_string(response.status()));
    printf("---------------------------------------------\n");
    if (response.status() == ic::client::Status::SUCCESS
        && response.http_status_code() == ic::client::http::StatusCode::HTTP_200_OK)   // equal to response.ok()
    {
        // Header
        printf("------ Headers -------------------------------\n");
        auto& headers = response.headers();
        for (auto& header : headers) {
            printf("%s: %s\n", header.first.c_str(), header.second.c_str());
        }
        // Data
        printf("------ Body ----------------------------------\n");
        printf("%s\n", response.data().c_str());
        printf("---------------------------------------------\n");
    }
    printf("\n\n");
}
