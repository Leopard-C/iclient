#include "../iclient/ic/ic.h"
#include <vector>


/* Progress bar */
bool g_curl_xfer_info(const ic::Request& request,
                      curl_off_t download_total_bytes, curl_off_t download_now_bytes,
                      curl_off_t upload_total_bytes, curl_off_t upload_now_bytes);
void reset_progress_bar();

void download_range(const std::string& url, const std::string& local_file, size_t bytes_start, size_t bytes_end);
bool merge_files(const std::vector<std::string>& files, const std::string& output_file);


void test_download_range() {
    std::vector<std::string> dl_parts = {
        "dl_part_0-200k.dat",
        "dl_part_200-400k.dat",
        "dl_part_400-600k.dat",
        "dl_part_600-800k.dat",
        "dl_part_800-.dat",
    };
    download_range("https://s3.ax1x.com/2020/11/27/DrtEm8.jpg", dl_parts[0],      0, 204799);
    download_range("https://s3.ax1x.com/2020/11/27/DrtEm8.jpg", dl_parts[1], 204800, 409599);
    download_range("https://s3.ax1x.com/2020/11/27/DrtEm8.jpg", dl_parts[2], 409600, 614399);
    download_range("https://s3.ax1x.com/2020/11/27/DrtEm8.jpg", dl_parts[3], 614400, 819199);
    download_range("https://s3.ax1x.com/2020/11/27/DrtEm8.jpg", dl_parts[4], 819200, ic::end);
    merge_files(dl_parts, "test_download_range.jpg");
}

void download_range(const std::string& url, const std::string& local_filename, size_t bytes_start, size_t bytes_end) {
    reset_progress_bar();
    printf("downloading %s\n", url.c_str());
    printf("local file: %s\n", local_filename.c_str());
    printf("range: %zu-%zu\n", bytes_start, bytes_end);

    ic::Request request(url);
    request.setVerifySsl(false);
    request.setDownloadFile(local_filename);
    request.setDownloadRange(bytes_start, bytes_end);
    request.setTransferProgressHandler(g_curl_xfer_info);

    auto response = request.perform();

    printf("iclient status: %s\n", ic::to_string(response.getStatus()).c_str());
    printf("\n");
}

void copy_data(FILE* fp_in, FILE* fp_out) {
    char buff[1024];
    while (!feof(fp_in)) {
        size_t rn = fread(buff, 1, 1024, fp_in);
        fwrite(buff, 1, rn, fp_out);
    }
}

bool merge_files(const std::vector<std::string>& input_files, const std::string& output_file) {
    printf("merge file: \n");
    FILE* fp_out = fopen(output_file.c_str(), "wb+");
    if (!fp_out) {
        printf("Open file %s failed\n", output_file.c_str());
        return false;
    }

    bool ret = true;
    for (const auto& input_file : input_files) {
        printf("    %s\n", input_file.c_str());
        FILE* fp_in = fopen(input_file.c_str(), "rb");
        if (!fp_in) {
            printf("Open file %s failed\n", input_file.c_str());
            ret = false;
            break;
        }
        copy_data(fp_in, fp_out);
        fclose(fp_in);
    }

    if (ret) {
        printf("output: %s\n", output_file.c_str());
        long size = ftell(fp_out);
        printf("size: %ld\n", size);
    }

    fclose(fp_out);
    return ret;
}

