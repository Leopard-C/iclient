/**
 * @file request.h
 * @brief Http Request.
 * @author Leopard-C (leopard.c@outlook.com)
 * @version 0.1
 * @date 2023-01-12
 * 
 * @copyright Copyright (c) 2021-present, Jinbao Chen.
 */
#ifndef IC_CLIENT_REQUEST_H_
#define IC_CLIENT_REQUEST_H_

#include <functional>
#include <map>
#include <string>
#include <vector>

#include "http.h"
#include "response.h"

namespace ic {
namespace client {

extern size_t end;  /* static_cast<size_t>(-1) */


enum class SslCertificateType {
    PME,
    DER
};

const char* to_string(SslCertificateType type);


enum class ProxyType {
    HTTP,
    HTTPS
};

int to_curl(ProxyType type);
const char* to_string(ProxyType type);


enum class HttpAuthType {
    BASIC,
    DIGEST,
    ANY,
    ANY_SAFE
};

unsigned long to_curl(HttpAuthType type);
const char* to_string(HttpAuthType type);


enum class IpResolve {
    Whatever,
    V4,
    V6
};


struct ProxyData {
    ProxyType type = ProxyType::HTTP;
    uint32_t port = 0;
    HttpAuthType auth_type = HttpAuthType::ANY;
    std::string host;
    std::string username;
    std::string password;
    bool empty() const { return host.empty() || port == 0; }
};


/**
 * @brief Http Request.
 */
class Request {
    friend class Executor;
public:
    /**
     * @brief Transfer Process Handler.
     * 
     * @retval true: Continue transfer
     * @retval false: Cancel.
     */
    using TransferProgressHandler = std::function<bool(const Request& request, int64_t download_total_bytes,
            int64_t download_now_bytes, int64_t upload_total_bytes, int64_t upload_now_bytes)>;

public:
    Request() = default;
    Request(const std::string& url) : url_(url) {}
    Request(http::Method method) : http_method_(method) {}
    Request(const std::string& url, http::Method method) : url_(url), http_method_(method) {}

    /* Perform request */
    Response Perform();

    /* Cancel request */
    void Cancel();

    void set_transfer_progress_handler(TransferProgressHandler handler)
        { on_transfer_progress_handler_ = handler; }

    /* Timeout */
    int timeout() const { return timeout_ms_; }
    void set_timeout(int milliseconds) { timeout_ms_ = milliseconds; }

    /* Speed */
    int64_t max_upload_speed() const { return max_upload_speed_; }
    int64_t max_download_speed() const { return max_download_speed_; }
    void set_max_upload_speed(int64_t bps) { max_upload_speed_ = bps; }
    void set_max_download_speed(int64_t bps) { max_download_speed_ = bps; }

    /* Buffer size */
    int64_t buffer_size() const { return buffer_size_; }
    void set_buffer_size(int64_t buffer_size) { buffer_size_ = buffer_size; }

    /* Url */
    const std::string& url() const { return url_; }
    void set_url(const std::string& url) { url_ = url; }

    /* Method */
    http::Method http_method() const { return http_method_; }
    void set_http_method(http::Method method) { http_method_ = method; }

    /* Version */
    http::Version http_version() const { return http_version_; }
    void set_http_version(http::Version ver) { http_version_ = ver; }

    /* Follow redirects */
    int max_redirects() const { return max_redirects_; }
    void set_max_redirects(int count = -1) { max_redirects_ = count < -1 ? -1 : count; }

    /* Ip resolve */
    IpResolve ip_resolve() const { return ip_resolve_; }
    void set_ip_resolve(IpResolve ip_resolve) { ip_resolve_ = ip_resolve; }

    /* Verify */
    bool verify_ssl_peer() const { return verify_ssl_peer_; }
    bool verify_ssl_host() const { return verify_ssl_host_; }
    bool verify_ssl_status() const { return verify_ssl_status_; }
    void set_verify_ssl_peer(bool verify) { verify_ssl_peer_ = verify; }
    void set_verify_ssl_host(bool verify) { verify_ssl_host_ = verify; }
    void set_verify_ssl_status(bool verify) { verify_ssl_status_ = verify; }
    void set_verify_ssl(bool verify) { verify_ssl_peer_ = verify_ssl_host_ = verify_ssl_status_ = verify; }

    /* SSL cert & key */
    SslCertificateType ssl_cert_type() const { return ssl_cert_type_; }
    const std::string& ssl_cert_file() const { return ssl_cert_file_; }
    const std::string& ssl_key() const { return ssl_key_file_; }
    const std::string& ssl_key_password() const { return ssl_key_file_password_; }
    void set_ssl_key(const std::string& key_file) { ssl_key_file_ = key_file; }
    void set_ssl_key_password(const std::string& passowrd) { ssl_key_file_password_ = passowrd; }
    void set_ssl_cert_type(SslCertificateType cert_type) { ssl_cert_type_ = cert_type; }
    void set_ssl_cert_file(const std::string& cert_file) { ssl_cert_file_ = cert_file; }
    void set_ssl_cert(SslCertificateType cert_type, const std::string& cert_file) {
        ssl_cert_type_ = cert_type;
        ssl_cert_file_ = cert_file;
    }

    /* Proxy */
    const ProxyData& proxy() const { return proxy_data_; }
    void set_proxy(const ProxyData& proxy_data) { proxy_data_ = proxy_data; }
    void set_proxy(const std::string& host, uint32_t port, ProxyType type = ProxyType::HTTP);
    bool HasProxy() const { return !proxy_data_.empty(); }

    /* Accept Encodings */
    const std::vector<std::string>& accept_encodings() const { return accept_encodings_;  }
    void set_accpet_encodings(const std::vector<std::string>& encodings) { accept_encodings_ = encodings; }
    void set_accept_all_encodings() { std::vector<std::string>().swap(accept_encodings_); }

    /* Header */
    const std::map<std::string, std::string>& headers() const { return headers_; }
    const std::string& header(std::string name) const;
    void set_headers(const std::map<std::string, std::string>& headers) { headers_ = headers; }
    void set_header(std::string name, const std::string& value);
    void RemoveHeader(std::string name);
    void ClearHeaders() { std::map<std::string, std::string>().swap(headers_); }

    /* Cookie */
    const std::string& cookie() const { return cookie_; }
    const std::string& cookie_file() const { return cookie_file_; }
    void set_cookie(const std::string& cookie) { cookie_ = cookie; }
    void set_cookie_file(const std::string& cookie_file) { cookie_file_ = cookie_file; }
    void ClearCookie() { cookie_.clear(); cookie_file_.clear(); }

    /* Data */
    const std::string& data() const { return data_; }
    void set_data(const std::string& data) { data_ = data; }
    void ClearData() { data_.clear(); }

    /* MimeField, string */
    const std::map<std::string, std::string>& mime_string_fields() const { return mime_string_fields_; }
    const std::map<std::string, std::string>& mime_file_fields() const { return mime_file_fields_; }
    const std::string& mime_string_field(const std::string& name) const;
    const std::string& nime_file_field(const std::string& name) const;
    void set_mime_string_field(const std::string& name, const std::string value) { mime_string_fields_[name] = value; }
    void set_mime_file_field(const std::string& name, const std::string value) { mime_file_fields_[name] = value; }
    void RemoveMimeStringField(const std::string& name);
    void RemoveMimeFileField(const std::string& name);
    void ClearMimeStringFields() { mime_string_fields_.clear(); }
    void ClearMimeFileField() { mime_file_fields_.clear(); }

    /* Download to file */
    const std::string& download_file() const { return download_file_; }
    const std::string& download_range() const { return download_range_; }
    bool download_resume() const { return download_resume_; }
    void set_download_file(const std::string& download_file, bool resume = false)
        { download_file_ = download_file; download_resume_ = resume; }
    void set_download_range(size_t bytes_start, size_t bytes_end = end);
    void set_download_resume(bool resume) { download_resume_ = resume; }

    /* User Data */
    void* user_data() const { return user_data_; }
    void set_user_data(void* data) { user_data_ = data; }
    void ClearUserData() { user_data_ = nullptr; }

    friend size_t curl_write_header(char* buffer, size_t size, size_t nitems, void* user_ptr);
    friend size_t curl_write_data(void* buffer, size_t size, size_t nitems, void* user_ptr);
    friend int curl_xfer_info(void* clientp, curl_off_t download_total_bytes, curl_off_t download_now_bytes,
        curl_off_t upload_total_bytes, curl_off_t upload_now_bytes);

private:
    bool cancel_{false};

    std::string url_;
    http::Method http_method_{http::Method::HTTP_GET};
    http::Version http_version_{http::Version::V1_1};

    IpResolve ip_resolve_{IpResolve::Whatever};

    /* Redirect */
    int max_redirects_{-1};

    /* Referer */
    bool auto_referer_{true};
    std::string referer_;

    /* Timeout: millseconds */
    int timeout_ms_{-1};

    /* Upload/Download Speed, bytes per second */
    int64_t max_upload_speed_{0};
    int64_t max_download_speed_{0};

    /* Buffer size (bytes) */
    int64_t buffer_size_{16 * 1024};  /* Default: 16kB */

    /* User Data */
    void* user_data_{nullptr};

    /* Transfer progress */
    TransferProgressHandler on_transfer_progress_handler_{nullptr};

    /* SSL */
    bool verify_ssl_peer_{false};
    bool verify_ssl_host_{false};
    bool verify_ssl_status_{false};
    SslCertificateType ssl_cert_type_{SslCertificateType::PME};
    std::string ssl_cert_file_;
    std::string ssl_key_file_;
    std::string ssl_key_file_password_;

    /* Proxy Data */
    ProxyData proxy_data_;

    /* Accept encodings */
    std::vector<std::string> accept_encodings_;

    /* Request headers */
    std::map<std::string, std::string> headers_;

    /* User-Agent */
    std::string user_agent_;

    /* Post Data */
    std::string data_;

    /* Mime Fileds */
    std::map<std::string, std::string> mime_string_fields_;
    std::map<std::string, std::string> mime_file_fields_;

    /* Cookie */
    std::string cookie_;
    std::string cookie_file_;

    /* Download to file */
    bool download_resume_{false}; // true => dl_range_ will be ignored.
    std::string download_file_;
    std::string download_range_;
};

} // namespace client
} // namespace ic

#endif // IC_CLIENT_REQUEST_H_
