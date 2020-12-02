/************************************************************
** namespace:   ic
**
** class name:  Response
**
** description: Returned object of request.perform()
**
** author: Leopard-C
**
** update: 2020/12/02
************************************************************/
#ifndef __IC_RESPONSE_H__
#define __IC_RESPONSE_H__

#include "ic_http.h"
#include "ic_curl_inc.h"
#include <string>
#include <map>

namespace ic {

/*
 *  iclient Status Code
**/
enum class Status {
    BUILDING,
    EXECUTING,
    EMPTY_URL,
    SUCCESS,
    CONNECT_ERROR,
    CONNECT_DNS_ERROR,
    CONNECT_SSL_ERROR,
    TIMEOUT,
    RESPONSE_EMPTY,
/* f**k winnt.h */
#ifdef ERROR
#undef ERROR
#endif
    ERROR,
    FAILD_OPEN_OUTPUTFILE,
    OUTPUTFILE_BEING_USED,
    FAILED_TO_START,
    NOT_SUPPORT_DOWNLOAD_RESUME_OR_RANGE,
    DOWNLOAD_ERROR
};

Status from_curl_status_code(int code);
const std::string& to_string(Status code);


/*
 *  class: Response
**/
class Response {
    friend class Executor;
public:
    Response();

    Status getStatus() const { return status_; }

    http::Version getVersion() const { return http_version_; }

    http::StatusCode getHttpStatus() const { return http_status_code_; }

    const std::map<std::string, std::string>& getHeaders() const { return headers_; }
    bool hasHeader(std::string name) const;
    const std::string& getHeader(std::string name);

    const std::string& getData() const { return data_; }

    int getTotalTime() const { return total_time_ms_; }

    int getNumConnects() const { return num_connects_; }

    int getNumRedirects() const { return num_redirects_; }

    void parseFromCurl(CURL* curl);

public:
    friend size_t curl_write_header(char* buffer, size_t size, size_t nitems, void* user_ptr);
    friend size_t curl_write_data(void* buffer, size_t size, size_t nitems, void* user_ptr);
    friend int curl_xfer_info(void* clientp, curl_off_t download_total_bytes, curl_off_t download_now_bytes,
        curl_off_t upload_total_bytes, curl_off_t upload_now_bytes);

private:
    Status status_ = Status::BUILDING;

    http::Version http_version_ = http::Version::V1_1;

    http::StatusCode http_status_code_ = http::StatusCode::HTTP_UNKNOWN;

    int total_time_ms_ = 0;

    int num_connects_ = 0;

    int num_redirects_ = 0;

    std::string data_;

    std::map<std::string, std::string> headers_;
};

} // namespace ic

#endif //__IC_RESPONSE_H__
