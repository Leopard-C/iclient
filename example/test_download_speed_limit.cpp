#include "../iclient/ic/ic.h"

/* Progress bar */
bool g_curl_xfer_info(const ic::Request& request,
                      curl_off_t download_total_bytes, curl_off_t download_now_bytes,
                      curl_off_t upload_total_bytes, curl_off_t upload_now_bytes);
void reset_progress_bar();

static void test_download_speed_limit_KBps(int speed) {
    reset_progress_bar();
    ic::Request request("http://i0.hdslb.com/bfs/album/6eb6910163703053ba5e79dc45baa829a0e2f5e4.jpg");
    request.setVerifySsl(false);
    char filename[128];
    sprintf(filename, "test_download_speed_limit_%d.jpg", speed);
    request.setDownloadFile(filename);
    request.setTransferProgressHandler(g_curl_xfer_info);

    request.setMaxDownloadSpeed(speed * 1024);
    printf("Speed Limit: %dKB/s\n", speed);

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

void test_download_speed_limit() {
    test_download_speed_limit_KBps(128);
    test_download_speed_limit_KBps(256);
    test_download_speed_limit_KBps(512);
}

