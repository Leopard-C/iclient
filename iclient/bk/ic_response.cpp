#include "ic/ic_response.h"
#include "ic/ic_http.h"
#include "ic/ic_util.h"


namespace ic {

extern std::string ic_empty_string;

/*
 *  iclient Status Code
**/
Status from_curl_status_code(int code) {
    switch (code) {
    case CURLcode::CURLE_OK:
        return Status::SUCCESS;
    case CURLcode::CURLE_GOT_NOTHING:
        return Status::RESPONSE_EMPTY;
    case CURLcode::CURLE_OPERATION_TIMEDOUT:
        return Status::TIMEOUT;
    case CURLcode::CURLE_COULDNT_CONNECT:
        return Status::CONNECT_ERROR;
    case CURLcode::CURLE_COULDNT_RESOLVE_HOST:
        return Status::CONNECT_DNS_ERROR;
    case CURLcode::CURLE_SSL_CONNECT_ERROR:
        return Status::CONNECT_SSL_ERROR;
    case CURLcode::CURLE_WRITE_ERROR:
        return Status::DOWNLOAD_ERROR;
    case CURLcode::CURLE_SEND_ERROR:
        return Status::FAILED_TO_START;
    default:
#ifdef ERROR
#undef ERROR
#endif
        return Status::ERROR;
    }
}

static const std::string STR_STATUS_BUILDING = "BUILDING";
static const std::string STR_STATUS_EXECUTING = "EXECUTING";
static const std::string STR_EMPTY_URL = "EMPTY_URL";
static const std::string STR_STATUS_SUCCESS = "SUCCESS";
static const std::string STR_STATUS_CONNECT_ERROR = "CONNECT_ERROR";
static const std::string STR_STATUS_CONNECT_DNS_ERROR = "CONNECT_DNS_ERROR";
static const std::string STR_STATUS_CONNECT_SSL_ERROR = "CONNECT_SSL_ERROR";
static const std::string STR_STATUS_TIMEOUT = "TIMEOUT";
static const std::string STR_STATUS_RESPONSE_EMPTY = "RESPONSE_EMPTY";
static const std::string STR_STATUS_ERROR_FAILED_TO_START = "ERROR_FAILED_TO_START";
static const std::string STR_STATUS_ERROR = "ERROR";
static const std::string STR_STATUS_DOWNLOAD_ERROR = "DOWNLOAD_ERROR";
static const std::string STR_STATUS_ERROR_OPEN_OUTPUTFILE = "ERROR_OPEN_OUTPUTFILE";
static const std::string STR_STATUS_OUTPUTFILE_BEING_USED = "OUTPUTFILE_BEING_USED";
static const std::string STR_NOT_SUPPORT_DOWNLOAD_RESUME_OR_RANGE = "NOT_SUPPORT_DOWNLOAD_RESUME_OR_RANGE";

const std::string& to_string(Status code) {
    switch (code) {
    case Status::BUILDING:
        return STR_STATUS_BUILDING;
    case Status::EXECUTING:
        return STR_STATUS_EXECUTING;
    case Status::EMPTY_URL:
        return STR_EMPTY_URL;
    case Status::SUCCESS:
        return STR_STATUS_SUCCESS;
    case Status::CONNECT_ERROR:
        return STR_STATUS_CONNECT_ERROR;
    case Status::CONNECT_DNS_ERROR:
        return STR_STATUS_CONNECT_DNS_ERROR;
    case Status::CONNECT_SSL_ERROR:
        return STR_STATUS_CONNECT_SSL_ERROR;
    case Status::TIMEOUT:
        return STR_STATUS_TIMEOUT;
    case Status::RESPONSE_EMPTY:
        return STR_STATUS_RESPONSE_EMPTY;
    case Status::DOWNLOAD_ERROR:
        return STR_STATUS_DOWNLOAD_ERROR;
    case Status::FAILED_TO_START:
        return STR_STATUS_ERROR_FAILED_TO_START;
    case Status::FAILD_OPEN_OUTPUTFILE:
        return STR_STATUS_ERROR_OPEN_OUTPUTFILE;
    case Status::OUTPUTFILE_BEING_USED:
        return STR_STATUS_OUTPUTFILE_BEING_USED;
    case Status::NOT_SUPPORT_DOWNLOAD_RESUME_OR_RANGE:
        return STR_NOT_SUPPORT_DOWNLOAD_RESUME_OR_RANGE;
#ifdef ERROR
#undef ERROR
#endif
    case Status::ERROR:
    default:
        return STR_STATUS_ERROR;
    }
}


/*
 *  class: Response
**/
Response::Response() {
    data_.reserve(4096);
}

void Response::parseFromCurl(CURL* curl) {
    long http_response_code = 0L;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_response_code);
    http_status_code_ = http::to_curl(static_cast<int>(http_response_code));

    long http_version = 0L;
    curl_easy_getinfo(curl, CURLINFO_HTTP_VERSION, &http_version);
    http_version_ = static_cast<http::Version>(http_version);

    double total_time = 0L;
    curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &total_time);
    total_time_ms_ = static_cast<int>(total_time);

    long connect_count = 0L;
    curl_easy_getinfo(curl, CURLINFO_NUM_CONNECTS, &connect_count);
    num_connects_ = static_cast<int>(connect_count);

    long redirect_count = 0L;
    curl_easy_getinfo(curl, CURLINFO_REDIRECT_COUNT, &redirect_count);
    num_redirects_ = static_cast<int>(redirect_count);
}

const std::string& Response::getHeader(std::string name) {
    util::tolower(name);
    auto findIt = headers_.find(name);
    if (findIt == headers_.end()) {
        return ic_empty_string;
    }
    return findIt->second;
}

bool Response::hasHeader(std::string name) const {
    util::tolower(name);
    return headers_.find(name) != headers_.end();
}

} // namespace ic

