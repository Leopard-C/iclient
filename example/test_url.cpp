#include "../iclient/ic/ic.h"

void test_url() {
    ic::Url url("https://api.icrystal.top/random");
    url.setParam("min", "1");
    url.setParam("max", "100");
    url.setParam("count", "20");
    printf("GET: %s\n", url.toString().c_str());

    ic::Request request(url.toString());
    request.setVerifySsl(false);
    ic::Response res = request.perform();

    if (res.getStatus() == ic::Status::SUCCESS && res.getHttpStatus() == ic::http::StatusCode::HTTP_200_OK) {
        // Header
        printf("------ Headers -------------------------------\n");
        auto& headers = res.getHeaders();
        for (auto& header : headers) {
            printf("%s: %s\n", header.first.c_str(), header.second.c_str());
        }
        // Data
        printf("------ Body ----------------------------------\n");
        printf("%s\n", res.getData().c_str());
        printf("---------------------------------------------\n");
    }
}
