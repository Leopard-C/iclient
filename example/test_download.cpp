#include "../iclient/ic/ic.h"

/* Progress bar */
bool g_curl_xfer_info(const ic::Request& request,
                      curl_off_t download_total_bytes, curl_off_t download_now_bytes,
                      curl_off_t upload_total_bytes, curl_off_t upload_now_bytes);
void reset_progress_bar();

void test_download() {
    reset_progress_bar();
    ic::Request request("https://s3.ax1x.com/2020/11/27/DrtEm8.jpg");
    request.setVerifySsl(false);
    request.setDownloadFile("dl_test.jpg");
    request.setTransferProgressHandler(g_curl_xfer_info);

    auto response = request.perform();

    printf("iclient status: %s\n", ic::to_string(response.getStatus()).c_str());
    if (response.getStatus() == ic::Status::SUCCESS
        && response.getHttpStatus() == ic::http::StatusCode::HTTP_200_OK)
    {
        printf("------ Headers ------------------------------\n");
        auto& headers = response.getHeaders();
        for (auto& header : headers) {
            printf("%s: %s\n", header.first.c_str(), header.second.c_str());
        }
    }
    printf("\n\n");
}

