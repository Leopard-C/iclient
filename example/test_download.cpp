#include "iclient/iclient.h"

/* Progress bar */
bool g_curl_xfer_info(const ic::client::Request& request,
                      curl_off_t download_total_bytes, curl_off_t download_now_bytes,
                      curl_off_t upload_total_bytes, curl_off_t upload_now_bytes);
void reset_progress_bar();

void test_download() {
    reset_progress_bar();
    ic::client::Request request("http://i0.hdslb.com/bfs/album/73768e38c6f30480939fd14db5d888ab00afdcd8.jpg");
    request.set_verify_ssl(false);
    request.set_download_file("test_download.jpg");
    request.set_transfer_progress_handler(g_curl_xfer_info);

    auto response = request.Perform();

    printf("iclient status: %s\n", ic::client::to_string(response.status()));
    if (response.status() == ic::client::Status::SUCCESS
        && response.http_status_code() == ic::client::http::StatusCode::HTTP_200_OK) // equal to response.ok()
    {
        printf("------ Headers ------------------------------\n");
        auto& headers = response.headers();
        for (auto& header : headers) {
            printf("%s: %s\n", header.first.c_str(), header.second.c_str());
        }
    }
    printf("\n\n");
}

