#include "../iclient/ic/ic.h"

/* Progress bar */
bool g_curl_xfer_info(const ic::Request& request,
                      curl_off_t download_total_bytes, curl_off_t download_now_bytes,
                      curl_off_t upload_total_bytes, curl_off_t upload_now_bytes);
void reset_progress_bar();

// defined in test_download_range.cpp
void download_range(const std::string& url, const std::string& local_file, size_t bytes_start, size_t bytes_end);

// defined in the end of this file
void download_resume(const std::string& url, const std::string& local_file);


void test_download_resume() {
    /* Download first 1999 bytes */
    download_range("https://s3.ax1x.com/2020/11/27/DrtEm8.jpg", "test_download_resume.jpg", 0, 1999);

    /* resume: breakpoint continuation */
    /* download data remain */
    download_resume("https://s3.ax1x.com/2020/11/27/DrtEm8.jpg", "test_download_resume.jpg");
}
 

void download_resume(const std::string& url, const std::string& local_file) {
    reset_progress_bar();
    printf("downloading %s\n", url.c_str());
    printf("local file: %s\n", local_file.c_str());
    printf("resume from: %ld bytes\n", ic::util::get_file_size(local_file));

    ic::Request request(url);
    request.setVerifySsl(false);
    request.setDownloadFile(local_file, true);
    request.setTransferProgressHandler(g_curl_xfer_info);

    auto response = request.perform();

    printf("iclient status: %s\n", ic::to_string(response.getStatus()).c_str());
    printf("\n\n");
}
