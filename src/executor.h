#ifndef IC_CLIENT_EXECUTOR_H_
#define IC_CLIENT_EXECUTOR_H_

#include "iclient/request.h"
#include "iclient/response.h"
#include "iclient/curl_inc.h"

namespace ic {
namespace client {

class Executor {
    friend class Request;
    friend class Response;
public:
    Executor(Request& request, Response& response);
    ~Executor();

    void Perform();

    void Reset();

public:
    friend size_t curl_write_header(char* buffer, size_t size, size_t nitems, void* user_ptr);
    friend size_t curl_write_data(void* buffer, size_t size, size_t nitems, void* user_ptr);
    friend int curl_xfer_info(void* clientp, curl_off_t download_total_bytes, curl_off_t download_now_bytes,
        curl_off_t upload_total_bytes, curl_off_t upload_now_bytes);

private:
    bool Prepare();

private:
    CURL* curl_{nullptr};
    FILE* file_stream_{nullptr};
    curl_mime* curl_mime_{nullptr};
    curl_slist* curl_request_headers_{nullptr};
    Request& request_;
    Response& response_;
};

} // namespace client
} // namespace ic

#endif // IC_CLIENT_EXECUTOR_H_
