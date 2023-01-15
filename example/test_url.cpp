#include "iclient/iclient.h"

void test_url() {
    using namespace ic::client;
    Url url("https://api.icrystal.top/util/random");
    url.SetParam("min", "1");
    url.SetParam("max", "100");
    url.SetParam("count", "20");
    printf("GET: %s\n", url.ToString().c_str());

    Request request(url.ToString());
    request.set_verify_ssl(false);
    auto response = request.Perform();

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
    else {
        printf("Request Error: %s %s\n", to_string(response.status()), http::to_string(response.http_status_code()));
    }
}
