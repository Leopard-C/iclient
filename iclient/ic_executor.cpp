#include "ic/ic_executor.h"
#include "ic/ic_util.h"
#include <mutex>
#include <atomic>

namespace ic {

/* Global init/cleanup libcurl */
static std::mutex g_ic_mutex;
static std::atomic_uint64_t g_ic_init_count;

static void global_init() {
    if (g_ic_init_count.fetch_add(1) == 0) {
        std::lock_guard<std::mutex> lck(g_ic_mutex);
        curl_global_init(CURL_GLOBAL_ALL);
    }
}

static void global_cleanup() {
    if (g_ic_init_count.fetch_sub(1) == 1) {
        std::lock_guard<std::mutex> lck(g_ic_mutex);
        curl_global_cleanup();
    }
}


size_t curl_write_header(char* buffer, size_t size, size_t nitems, void* user_ptr);
size_t curl_write_data(void* buffer, size_t size, size_t nitems, void* user_ptr);
int curl_xfer_info(void* clientp, curl_off_t download_total_bytes, curl_off_t download_now_bytes,
    curl_off_t upload_total_bytes, curl_off_t upload_now_bytes);


Executor::Executor() {
    curl_ = curl_easy_init();
}

Executor::~Executor() {
    reset();
    curl_easy_cleanup(curl_);
}

void Executor::reset() {
    request_ = nullptr;
    if (curl_mime_) {
        curl_mime_free(curl_mime_);
        curl_mime_ = nullptr;
    }
    if (curl_request_headers_) {
        curl_slist_free_all(curl_request_headers_);
        curl_request_headers_ = nullptr;
    }
    if (curl_) {
        curl_easy_reset(curl_);
    }
}

Response Executor::perform(Request* request) {
    global_init();

    request_ = request;

    do {
        /* Download: Resume or Range */
        if (!request_->dl_file_.empty() && (request_->dl_resume_ || !request_->dl_range_.empty())) {
            /* Check wether the server supports "Resume Breakpoint" */
            if (!(prepare_headonly_())) {
                break;
            }
            CURLcode code = curl_easy_perform(curl_);
            response_.status_ = from_curl_status_code(code);
            response_.parseFromCurl(curl_);
            if (response_.status_ != Status::SUCCESS) {
                break;
            }
            if (!response_.hasHeader("content-length")) {
                // won't get here
                response_.status_ = Status::ERROR;
                break;
            }
            if (response_.getHeader("content-length") == "0") {
                // the file has already full downloaded
                break;
            }
            if (!response_.hasHeader("content-range")) {
                response_.status_ = Status::NOT_SUPPORT_DOWNLOAD_RESUME_OR_RANGE;
                break;
            }
            curl_easy_reset(curl_);
            response_ = Response();
        }

        /* Normally prepare for query */
        if (prepare_normal_()) {
            CURLcode code = curl_easy_perform(curl_);
            response_.status_ = from_curl_status_code(code);
            response_.parseFromCurl(curl_);
        }
    } while (false);

    if (file_stream_) {
        fclose(file_stream_);
        file_stream_ = nullptr;
    }

    global_cleanup();

    return std::move(response_);
}

bool Executor::prepare_normal_() {
    /* Http method */
    switch (request_->http_method_) {
    case http::Method::POST:
        //curl_easy_setopt(curl_, CURLOPT_POST, 1L);
        curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, "POST");
        break;
    case http::Method::HEAD:
        curl_easy_setopt(curl_, CURLOPT_NOBODY, 1L);
        break;
    case http::Method::PUT:
        curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, "PUT");
        break;
#ifdef DELETE
#undef DELETE
#endif
    case http::Method::DELETE:
        curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, "DELETE");
        break;
    case http::Method::CONNECT:
        curl_easy_setopt(curl_, CURLOPT_CONNECT_ONLY, 1L);
        break;
    case http::Method::OPTIONS:
        curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, "OPTIONS");
        break;
    case http::Method::PATCH:
        curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, "PATCH");
        break;
    case http::Method::UNKNOWN:
    case http::Method::GET:
    default:
        curl_easy_setopt(curl_, CURLOPT_HTTPGET, 1L);
        break;
    }

    /* Process writing data */
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, curl_write_data);
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(curl_, CURLOPT_NOSIGNAL, 1L);

    /* POST Data */
    if (!request_->getData().empty()) {
        curl_easy_setopt(curl_, CURLOPT_POSTFIELDSIZE, static_cast<long>(request_->data_.size()));
        curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, request_->data_.c_str());
    }
    else {
        if (curl_mime_ != nullptr) {
            curl_mime_free(curl_mime_);
            curl_mime_ = nullptr;
        }
        /* String */
        if (!request_->mime_string_fields_.empty()) {
            for (const auto& mime_field : request_->mime_string_fields_) {
                auto* field = curl_mime_addpart(curl_mime_);
                curl_mime_name(field, mime_field.first.c_str());
                curl_mime_data(field, mime_field.second.c_str(), static_cast<long>(mime_field.second.length()));
            }
            curl_easy_setopt(curl_, CURLOPT_MIMEPOST, curl_mime_);
        }
        /* File */
        else if (!request_->mime_file_fields_.empty()) {
            for (const auto& mime_field : request_->mime_file_fields_) {
                auto* field = curl_mime_addpart(curl_mime_);
                curl_mime_filename(field, mime_field.first.c_str());
                curl_mime_filedata(field, mime_field.second.c_str());
            }
            curl_easy_setopt(curl_, CURLOPT_MIMEPOST, curl_mime_);
        }
    }

    /* Transfer info */
    if (request_->on_transfer_progress_handler_ != nullptr) {
        curl_easy_setopt(curl_, CURLOPT_XFERINFODATA, request_);
        curl_easy_setopt(curl_, CURLOPT_XFERINFOFUNCTION, curl_xfer_info);
        curl_easy_setopt(curl_, CURLOPT_NOPROGRESS, 0L);
    }
    else {
        curl_easy_setopt(curl_, CURLOPT_NOPROGRESS, 1L);
    }

    /* Download to file */
    if (!request_->dl_file_.empty()) {
        if (file_stream_) {
            response_.status_ = Status::OUTPUTFILE_BEING_USED;
            return false;
        }
        if (request_->dl_resume_) {
            /* Get file's current size */
            long size = util::get_file_size(request_->dl_file_);
            if (size < 0) {
                response_.status_ = Status::FAILD_OPEN_OUTPUTFILE;
                return false;
            }
            curl_easy_setopt(curl_, CURLOPT_RESUME_FROM, size);
            file_stream_ = fopen(request_->dl_file_.c_str(), "ab+");  // append
        }
        else {
            if (!request_->dl_range_.empty()) {
                curl_easy_setopt(curl_, CURLOPT_RANGE, request_->dl_range_.c_str());
            }
            file_stream_ = fopen(request_->dl_file_.c_str(), "wb+");  // overwrite
        }
        if (!file_stream_) {
            response_.status_ = Status::FAILD_OPEN_OUTPUTFILE;
            return false;
        }
    }

    return prepare_others_();
}


/* Send HEAD Request */
bool Executor::prepare_headonly_() {
    /* HEAD method */
    curl_easy_setopt(curl_, CURLOPT_NOBODY, 1L);

    /* Download to file */
    if (!request_->dl_file_.empty()) {
        if (request_->dl_resume_) {
            long size = util::get_file_size(request_->dl_file_);
            if (size < 0) {
                response_.status_ = Status::FAILD_OPEN_OUTPUTFILE;
                return false;
            }
            curl_easy_setopt(curl_, CURLOPT_RESUME_FROM, size);
        }
        else {
            if (!request_->dl_range_.empty()) {
                curl_easy_setopt(curl_, CURLOPT_RANGE, request_->dl_range_.c_str());
            }
        }
    }

    return prepare_others_();
}


bool Executor::prepare_others_() {
    /* Process writing header */
    curl_easy_setopt(curl_, CURLOPT_PRIVATE, this);
    curl_easy_setopt(curl_, CURLOPT_HEADERFUNCTION, curl_write_header);
    curl_easy_setopt(curl_, CURLOPT_HEADERDATA, this);

    /* URL */
    if (request_->url_.empty()) {
        response_.status_ = Status::EMPTY_URL;
        return false;
    }
    curl_easy_setopt(curl_, CURLOPT_URL, request_->url_.c_str());

    /* Http Version */
    curl_easy_setopt(curl_, CURLOPT_HTTP_VERSION, to_curl(request_->http_version_));

    /* Follow requests */
    if (request_->max_redirects_ != 0) {
        curl_easy_setopt(curl_, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl_, CURLOPT_MAXREDIRS, static_cast<long>(request_->max_redirects_));
    }
    else {
        curl_easy_setopt(curl_, CURLOPT_FOLLOWLOCATION, 0L);
    }

    /* timeout */
    if (request_->timeout_ms_ > 0) {
        curl_easy_setopt(curl_, CURLOPT_TIMEOUT_MS, static_cast<long>(request_->timeout_ms_));
    }

    /* Verify SSL */
    curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYPEER, (request_->isVerifySslPeer()) ? 1L : 0L);
    curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYHOST, (request_->isVerifySslHost()) ? 2L : 0L);
    curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYSTATUS, (request_->isVerifySslStatus()) ? 1L : 0L);

    /* SSL cert & key */
    if (!request_->ssl_cert_file_.empty()) {
        curl_easy_setopt(curl_, CURLOPT_SSLCERT, request_->ssl_cert_file_.c_str());
        curl_easy_setopt(curl_, CURLOPT_SSLCERTTYPE, to_string(request_->ssl_cert_type_).c_str());
        if (!request_->ssl_key_file_.empty()) {
            curl_easy_setopt(curl_, CURLOPT_SSLKEY, request_->ssl_key_file_.c_str());
        }
        if (!request_->ssl_key_file_password_.empty()) {
            curl_easy_setopt(curl_, CURLOPT_KEYPASSWD, request_->ssl_key_file_password_.c_str());
        }
    }

    /* Header */
    if (curl_request_headers_ != nullptr) {
        curl_slist_free_all(curl_request_headers_);
        curl_request_headers_ = nullptr;
    }
    for (const auto& header : request_->headers_) {
        std::string header_merged;
        header_merged.reserve(header.first.size() + header.second.size() + 1);
        header_merged += header.first;
        header_merged += ":";
        header_merged += header.second;
        curl_request_headers_ = curl_slist_append(curl_request_headers_, header_merged.c_str());
    }
    if (curl_request_headers_) {
        curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, curl_request_headers_);
    }

    /* Cookie */
    if (!request_->cookie_.empty()) {
        curl_easy_setopt(curl_, CURLOPT_COOKIE, request_->cookie_.c_str());
    }
    else if (!request_->cookie_file_.empty()) {
        curl_easy_setopt(curl_, CURLOPT_COOKIEFILE, request_->cookie_file_.c_str());
    }

    /* Proxy */
    if (request_->hasProxy()) {
        const auto& proxyData = request_->getProxy();
        curl_easy_setopt(curl_, CURLOPT_PROXY, proxyData.host.c_str());
        curl_easy_setopt(curl_, CURLOPT_PROXYTYPE, to_curl(proxyData.type));
        curl_easy_setopt(curl_, CURLOPT_PROXYPORT, proxyData.port);
        if (!proxyData.username.empty()) {
            curl_easy_setopt(curl_, CURLOPT_PROXYAUTH, to_curl(proxyData.auth_type));
            curl_easy_setopt(curl_, CURLOPT_PROXYUSERNAME, proxyData.username.c_str());
            if (!proxyData.password.empty()) {
                curl_easy_setopt(curl_, CURLOPT_PROXYPASSWORD, proxyData.password.c_str());
            }
        }
    }

    /* Encodings */
    std::string joined = util::join(request_->accept_encodings_, ", ");
    curl_easy_setopt(curl_, CURLOPT_ACCEPT_ENCODING, joined.c_str());

    return true;
}


/*
 *  process accepted headers
**/
size_t curl_write_header(char* buffer, size_t size, size_t nitems, void* user_ptr) {
    Executor* executor_ptr = static_cast<Executor*>(user_ptr);
    auto& headers = executor_ptr->response_.headers_;
    const size_t data_length = size * nitems;
    if (data_length == 0) {
        return 0;
    }

    std::string data(buffer, data_length);

    if (data_length == 2 && data == "\r\n") {
        return data_length;
    }
    if (data_length >= 4 && data.substr(0, 5) == "HTTP/") {
        return data_length;
    }
    if (data_length >= 2 && data[data_length - 2] == '\r' && data[data_length - 1] == '\n') {
        data.pop_back();
        data.pop_back();
    }

    auto pos = data.find(":");
    if (pos == std::string::npos) {
        return data_length;
    }

    std::string name = data.substr(0, pos);
    std::string value = data.substr(pos + 1);
    //printf("%s: %s\n", name.c_str(), value.c_str());

    /* Transform to lower case !!! */
    util::tolower(util::trim(name));
    util::tolower(util::trim(value));
    headers[name] = value;

    return data_length;
}


/*
 *  process accepted data
**/
size_t curl_write_data(void* buffer, size_t size, size_t nitems, void* user_ptr) {
    Executor* executor_ptr = static_cast<Executor*>(user_ptr);
    const size_t data_length = size * nitems;
    if (data_length > 0) {
        /* Write to file */
        if (executor_ptr->file_stream_) {
            fwrite(buffer, size, nitems, executor_ptr->file_stream_);
        }
        /* Write to response data */
        else {
            Response& response = executor_ptr->response_;
            response.data_.append(static_cast<const char*>(buffer), data_length);
        }
    }
    return data_length;
}

int curl_xfer_info(void* clientp, curl_off_t download_total_bytes, curl_off_t download_now_bytes,
    curl_off_t upload_total_bytes, curl_off_t upload_now_bytes)
{
    const Request* request_ptr = static_cast<const Request*>(clientp);
    if (request_ptr != nullptr && request_ptr->on_transfer_progress_handler_ != nullptr) {
        bool ret = request_ptr->on_transfer_progress_handler_(*request_ptr,
            download_total_bytes, download_now_bytes, upload_total_bytes, upload_now_bytes);
        if (!ret) {
            return 1;
        }
    }
    return 0;
}


} // namespace ic

