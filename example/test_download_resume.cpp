#include "iclient/iclient.h"

/* download file url */
extern const char* g_test_download_file_url;

/* Progress bar */
bool g_curl_xfer_info(const ic::client::Request& request,
                      curl_off_t download_total_bytes, curl_off_t download_now_bytes,
                      curl_off_t upload_total_bytes, curl_off_t upload_now_bytes);
void reset_progress_bar();

// defined in test_download_range.cpp
void download_range(const std::string& url, const std::string& local_file, size_t bytes_start, size_t bytes_end);

// defined in the end of this file
void download_resume(const std::string& url, const std::string& local_file);


void test_download_resume() {
    /* Download first 4096 bytes */
    download_range(g_test_download_file_url, "test_download_resume.jpg", 0, 4095);

    /* resume: breakpoint continuation */
    /* download data remain */
    download_resume(g_test_download_file_url, "test_download_resume.jpg");
}

/**
 * @brief Get local file size
 */
static long get_file_size(const std::string& filename) {
    FILE* fp = fopen(filename.c_str(), "rb");
    if (!fp) {
        return -1;
    }
    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);
    //fseek(fp, 0L, SEEK_SET);
    fclose(fp);
    return size;
}

void download_resume(const std::string& url, const std::string& local_file) {
    reset_progress_bar();
    printf("downloading %s\n", url.c_str());
    printf("local file: %s\n", local_file.c_str());
    printf("resume from: %ld bytes\n", get_file_size(local_file));

    ic::client::Request request(url);
    request.set_verify_ssl(false);
    request.set_download_file(local_file, true);
    request.set_transfer_progress_handler(g_curl_xfer_info);

    auto response = request.Perform();

    printf("iclient status: %s\n", ic::client::to_string(response.status()));
    printf("\n\n");
}
