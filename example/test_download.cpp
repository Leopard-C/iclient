#include "../iclient/ic/ic.h"

/* Progress bar */
bool g_curl_xfer_info(const ic::Request& request,
                      curl_off_t download_total_bytes, curl_off_t download_now_bytes,
                      curl_off_t upload_total_bytes, curl_off_t upload_now_bytes);
void reset_progress_bar();

void test_download() {
    reset_progress_bar();
    ic::Request request("http://public.icrystal.top/1.pdf");
    request.setVerifySsl(false);
    request.setDownloadFile("Z:/1.pdf");
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

