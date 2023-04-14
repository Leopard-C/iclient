/**
 * @file response.h
 * @brief Http Response.
 * @author Leopard-C (leopard.c@outlook.com)
 * @version 0.1
 * @date 2023-01-12
 * 
 * @copyright Copyright (c) 2021-present, Jinbao Chen.
 */
#ifndef IC_CLIENT_RESPONSE_H_
#define IC_CLIENT_RESPONSE_H_

#include "curl_inc.h"
#include "http.h"
#include <string>
#include <map>

namespace ic {
namespace client {

/**
 * @brief iclient Status Code
 */
enum class Status {
    BUILDING,
    EXECUTING,
    INVALID_URL,
    SUCCESS,
    CONNECT_ERROR,
    CONNECT_DNS_ERROR,
    CONNECT_SSL_ERROR,
    ABORTED_BY_CALLBACK,
    TIMEOUT,
    RESPONSE_EMPTY,
    FAILED_TO_OPEN_OUTPUTFILE,
    OUTPUTFILE_BEING_USED,
    FAILED_TO_START,
    DOWNLOAD_ERROR,
    UNKNOWN_ERROR,
};

Status from_curl_status_code(int code);
const char* to_string(Status code);


/**
 * @brief Http Response.
 */
class Response {
    friend class Executor;
public:
    Response(Status status = Status::BUILDING);

    bool success() const { return status_ == Status::SUCCESS; }
    bool ok() const { return status_ == Status::SUCCESS && http_status_code_ == http::StatusCode::HTTP_200_OK; }
    Status status() const { return status_; }

    http::Version http_version() const { return http_version_; }
    http::StatusCode http_status_code() const { return http_status_code_; }

    const std::map<std::string, std::string>& headers() const { return headers_; }
    bool HasHeader(const std::string& name) const;
    const std::string& GetHeader(const std::string& name);

    const std::string& server_ip() const { return server_ip_; }
    const std::string& data() const { return data_; }
    double total_time() const { return total_time_; }
    int num_connects() const { return num_connects_; }
    int num_redirects() const { return num_redirects_; }

public:
    friend size_t curl_write_header(char* buffer, size_t size, size_t nitems, void* user_ptr);
    friend size_t curl_write_data(void* buffer, size_t size, size_t nitems, void* user_ptr);
    friend int curl_xfer_info(void* clientp, curl_off_t download_total_bytes, curl_off_t download_now_bytes,
        curl_off_t upload_total_bytes, curl_off_t upload_now_bytes);

private:
    void ParseFromCurl(CURL* curl);

private:
    Status status_{Status::BUILDING};
    http::Version http_version_{http::Version::V1_1};
    http::StatusCode http_status_code_{http::StatusCode::HTTP_UNKNOWN};
    int num_connects_{0};
    int num_redirects_{0};
    double total_time_{0};  /* total time (unit: seconds) */
    std::string server_ip_;
    std::string data_;
    std::map<std::string, std::string> headers_;
};

} // namespace client
} // namespace ic

#endif // IC_CLIENT_RESPONSE_H_
