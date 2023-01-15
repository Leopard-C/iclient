#include "iclient/iclient.h"
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#  include <windows.h>
#else
#  include <sys/ioctl.h>
#  include <unistd.h>
#endif

int g_term_width;
int g_last_symbol_count = 0;
int g_total_symbol_count = 0;

void test_url();
void test_http_get();
void test_http_post();
void test_http_put();
void test_download();
void test_download_resume();
void test_download_range();
void test_download_speed_limit();

void get_terminal_info();


/* Progress bar */
bool g_curl_xfer_info(const ic::client::Request& request,
                      curl_off_t download_total_bytes, curl_off_t download_now_bytes,
                      curl_off_t upload_total_bytes, curl_off_t upload_now_bytes);
void resetProgressBar();


int main() {
    get_terminal_info();

    printf("sizeof(Request)  = %lu\n", sizeof(ic::client::Request));
    printf("sizeof(Response) = %lu\n", sizeof(ic::client::Response));

    printf("0. quit\n");
    printf("1. test_url\n");
    printf("2. test_http_get\n");
    printf("3. test_http_post\n");
    printf("4. test_http_put\n");
    printf("5. test_download\n");
    printf("6. test_download_resume\n");
    printf("7. test_download_range\n");
    printf("8. test_download_speed_limit\n");
    printf("Choose item(0-8): ");
    fflush(stdout);

    int id;
    scanf("%d", &id);
    printf("------------------------------\n\n");

    switch (id) {
    case 0: break;
    case 1: test_url(); break;
    case 2: test_http_get(); break;
    case 3: test_http_post(); break;
    case 4: test_http_put(); break;
    case 5: test_download(); break;
    case 6: test_download_resume(); break;
    case 7: test_download_range(); break;
    case 8: test_download_speed_limit(); break;
    default: printf("Error.\n"); break;
    }

#ifdef _WIN32
    system("pause");
#endif

    return 0;
}


void get_terminal_info() {
#ifdef _WIN32
    system("chcp 65001");
    HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    GetConsoleScreenBufferInfo(hOutput, &bInfo);
    g_term_width = bInfo.srWindow.Right - bInfo.srWindow.Left;
#else
    struct winsize size;
    ioctl(STDIN_FILENO, TIOCGWINSZ, &size);
    g_term_width = size.ws_col;
#endif // _WIN32

    if (g_term_width > 132) {
        g_total_symbol_count = 100;
    }
    else if (g_term_width > 52) {
        g_total_symbol_count = g_term_width - 32;
    }
    else {
        g_total_symbol_count = 0;
    }

    g_last_symbol_count = 0;
}

void format_data_size(long long data_size, char* output_str, int output_str_size) {
    const int KB = 1024;
    const int MB = 1024 * 1024;
    const int GB = 1024 * 1024 * 1024;
    if (data_size < KB) {
        snprintf(output_str, output_str_size, "%lldB", data_size);
    }
    else if (data_size < 1024 * 1024) {
        snprintf(output_str, output_str_size, "%.2lfKB", data_size / static_cast<double>(KB));
    }
    else if (data_size < 1024 * 1024 * 1024) {
        snprintf(output_str, output_str_size, "%.2lfMB", data_size / static_cast<double>(MB));
    }
    else {
        snprintf(output_str, output_str_size, "%.2lfGB", data_size / static_cast<double>(GB));
    }
}

/* Progress bar */
bool g_curl_xfer_info(const ic::client::Request& request,
                      curl_off_t download_total_bytes, curl_off_t download_now_bytes,
                      curl_off_t upload_total_bytes, curl_off_t upload_now_bytes)
{
    if (download_total_bytes > 0 && g_total_symbol_count > 0) {
        double percent = static_cast<double>(download_now_bytes) / download_total_bytes;
        int curr_symbol_count = static_cast<int>(g_total_symbol_count * percent);
        if (curr_symbol_count > g_last_symbol_count) {
            printf("\r[");
            for (int i = 0; i < curr_symbol_count - 1; ++i) {
                printf("=");
            }
            printf(">");
            for (int i = curr_symbol_count; i < g_total_symbol_count; ++i) {
                printf(" ");
            }
            char dl_now_bytes_str[32], dl_total_bytes_str[32];
            format_data_size(download_now_bytes, dl_now_bytes_str, sizeof(dl_now_bytes_str));
            format_data_size(download_total_bytes, dl_total_bytes_str, sizeof(dl_total_bytes_str));
            printf("] %9s/%-9s %5.2lf%%",  dl_now_bytes_str, dl_total_bytes_str, percent * 100);
            fflush(stdout);
            g_last_symbol_count = curr_symbol_count;
            if (download_now_bytes >= download_total_bytes) {
                printf("\n");
            }
        }
    }
    return true;
}

void reset_progress_bar() {
    g_last_symbol_count = 0;
}
