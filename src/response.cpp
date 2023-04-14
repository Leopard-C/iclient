#include "iclient/response.h"
#include "iclient/http.h"
#include "util.h"

namespace ic {
namespace client {

extern std::string empty_string;

/**
 * @brief iclient Status Code
 */
Status from_curl_status_code(int code) {
    switch (code) {
        case CURLcode::CURLE_OK:                   return Status::SUCCESS;
        case CURLcode::CURLE_GOT_NOTHING:          return Status::RESPONSE_EMPTY;
        case CURLcode::CURLE_OPERATION_TIMEDOUT:   return Status::TIMEOUT;
        case CURLcode::CURLE_COULDNT_CONNECT:      return Status::CONNECT_ERROR;
        case CURLcode::CURLE_COULDNT_RESOLVE_HOST: return Status::CONNECT_DNS_ERROR;
        case CURLcode::CURLE_SSL_CONNECT_ERROR:    return Status::CONNECT_SSL_ERROR;
        case CURLcode::CURLE_WRITE_ERROR:          return Status::DOWNLOAD_ERROR;
        case CURLcode::CURLE_SEND_ERROR:           return Status::FAILED_TO_START;
        case CURLcode::CURLE_ABORTED_BY_CALLBACK:  return Status::ABORTED_BY_CALLBACK;
        default:                                   return Status::UNKNOWN_ERROR;
    }
}

static const char* STR_STATUS_BUILDING = "BUILDING";
static const char* STR_STATUS_EXECUTING = "EXECUTING";
static const char* STR_STATUS_INVALID_URL = "INVALID_URL";
static const char* STR_STATUS_SUCCESS = "SUCCESS";
static const char* STR_STATUS_CONNECT_ERROR = "CONNECT_ERROR";
static const char* STR_STATUS_CONNECT_DNS_ERROR = "CONNECT_DNS_ERROR";
static const char* STR_STATUS_CONNECT_SSL_ERROR = "CONNECT_SSL_ERROR";
static const char* STR_STATUS_TIMEOUT = "TIMEOUT";
static const char* STR_STATUS_RESPONSE_EMPTY = "RESPONSE_EMPTY";
static const char* STR_STATUS_FAILED_TO_START = "FAILED_TO_START";
static const char* STR_STATUS_UNKNOWN_ERROR = "UNKNOWN_ERROR";
static const char* STR_STATUS_DOWNLOAD_ERROR = "DOWNLOAD_ERROR";
static const char* STR_STATUS_FAILED_TO_OPEN_OUTPUTFILE = "FAILED_TO_OPEN_OUTPUTFILE";
static const char* STR_STATUS_OUTPUTFILE_BEING_USED = "OUTPUTFILE_BEING_USED";
static const char* STR_STATUS_ABORTED_BY_CALLBACK = "ABORTED_BY_CALLBACK";

#ifdef CASE
#undef CASE
#endif

#define CASE(code) case Status::code: return STR_STATUS_##code

const char* to_string(Status code) {
    switch (code) {
        CASE(BUILDING);
        CASE(EXECUTING);
        CASE(INVALID_URL);
        CASE(SUCCESS);
        CASE(CONNECT_ERROR);
        CASE(CONNECT_DNS_ERROR);
        CASE(CONNECT_SSL_ERROR);
        CASE(TIMEOUT);
        CASE(RESPONSE_EMPTY);
        CASE(DOWNLOAD_ERROR);
        CASE(FAILED_TO_START);
        CASE(FAILED_TO_OPEN_OUTPUTFILE);
        CASE(OUTPUTFILE_BEING_USED);
        CASE(ABORTED_BY_CALLBACK);
        CASE(UNKNOWN_ERROR);
        default: return STR_STATUS_UNKNOWN_ERROR;
    }
}


Response::Response(Status status/* = Status::BUILDING*/) : status_(status) {
    data_.reserve(4096);
}

void Response::ParseFromCurl(CURL* curl) {
    long http_response_code = 0L;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_response_code);
    http_status_code_ = http::from_curl_http_response_code(static_cast<int>(http_response_code));

    long http_version = 0L;
    curl_easy_getinfo(curl, CURLINFO_HTTP_VERSION, &http_version);
    http_version_ = static_cast<http::Version>(http_version);

    /* total time (unit: seconds) */
    curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &total_time_);

    long connect_count = 0L;
    curl_easy_getinfo(curl, CURLINFO_NUM_CONNECTS, &connect_count);
    num_connects_ = static_cast<int>(connect_count);

    long redirect_count = 0L;
    curl_easy_getinfo(curl, CURLINFO_REDIRECT_COUNT, &redirect_count);
    num_redirects_ = static_cast<int>(redirect_count);

    char* ip_str = nullptr;
    curl_easy_getinfo(curl, CURLINFO_PRIMARY_IP, &ip_str);
    if (ip_str) {
        server_ip_ = std::string(ip_str);
    }
}

const std::string& Response::GetHeader(const std::string& name) {
    std::string name_lower = name;
    util::to_lower(name_lower);
    auto iter = headers_.find(name_lower);
    if (iter != headers_.end()) {
        return iter->second;
    }
    return empty_string;
}

bool Response::HasHeader(const std::string& name) const {
    std::string name_lower = name;
    util::to_lower(name_lower);
    return headers_.find(name_lower) != headers_.end();
}

} // namespace client
} // namespace ic
