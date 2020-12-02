/*******************************************************
** class name:  Executor
**
** description: Perform request
**
** author: Leopard-C
**
** update: 2020/12/02
*******************************************************/
#ifndef __IC_EXECUTOR_H__
#define __IC_EXECUTOR_H__

#include "ic_request.h"
#include "ic_response.h"
#include "ic_curl_inc.h"

namespace ic {

class Executor {
    friend class Request;
    friend class Response;
public:
    Executor();
    ~Executor();

    Response perform(Request* request);

    void reset();

public:
    friend size_t curl_write_header(char* buffer, size_t size, size_t nitems, void* user_ptr);
    friend size_t curl_write_data(void* buffer, size_t size, size_t nitems, void* user_ptr);
    friend int curl_xfer_info(void* clientp, curl_off_t download_total_bytes, curl_off_t download_now_bytes,
        curl_off_t upload_total_bytes, curl_off_t upload_now_bytes);

private:
    bool prepare_normal_();
    bool prepare_headonly_();

    bool prepare_others_();

private:
    CURL* curl_ = nullptr;

    FILE* file_stream_ = nullptr;

    curl_mime* curl_mime_ = nullptr;

    curl_slist* curl_request_headers_ = nullptr;

    Request* request_ = nullptr;

    Response response_;
};

} // namespace ic

#endif // __IC_EXECUTOR_H__
