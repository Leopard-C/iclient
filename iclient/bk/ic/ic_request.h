#ifndef __IC_REQUEST_H__
#define __IC_REQUEST_H__

#include "ic_http.h"
#include "ic_response.h"

#include <string>
#include <vector>
#include <functional>
#include <map>

namespace ic {


/*
 *   SSL Certificate Type
**/
enum class SslCertificateType {
    PME,
    DER
};

const std::string& to_string(SslCertificateType type);


/*
 *  Proxy Type
**/
enum class ProxyType {
    HTTP,
    HTTPS
};

int to_curl(ProxyType type);
const std::string& to_string(ProxyType type);


/*
 *  Http Auth Type
**/
enum class HttpAuthType {
    BASIC,
    DIGEST,
    ANY,
    ANY_SAFE
};

int to_curl(HttpAuthType type);
const std::string& to_string(HttpAuthType type);


struct ProxyData {
    ProxyType type = ProxyType::HTTP;
    uint32_t port;
    HttpAuthType auth_type = HttpAuthType::ANY;
    std::string host;
    std::string username;
    std::string password;
};


class Request {
    friend class Executor;
public:
    using TransferProgressHandler = std::function<bool(const Request& request, int64_t download_total_bytes,
            int64_t download_now_bytes, int64_t upload_total_bytes, int64_t upload_now_bytes)>;

public:
    Request(const std::string& url, int timeout_ms = -1) :
        url_(url), timeout_ms_(timeout_ms) {}

    /* Perform Request */
    Response perform();

    void setTransferProgressHandler(TransferProgressHandler handler)
        { on_transfer_progress_handler_ = handler; }

    /* Timeout */
    void setTimeout(int millisconds) { timeout_ms_ = millisconds; }
    int getTimeout() const { return timeout_ms_; }

    /* Url */
    void setUrl(const std::string& url) { url_ = url; }
    const std::string& getUrl() const { return url_; }

    /* Method */
    void setMethod(http::Method method) { http_method_ = method; }
    http::Method getMethod() const { return http_method_; }

    /* Version */
    void setVersion(http::Version ver) { http_version_ = ver; }
    http::Version getVersion() const { return http_version_; }

    /* Follow redirects */
    void setFollowRedirects(int maxCount = -1) { max_redirects_ = maxCount < -1 ? -1 : maxCount; }
    bool isFollowRedirects() const { return max_redirects_ != 0; }
    int getFollowRedirects() const { return max_redirects_; }

    /* Verify */
    void setVerifySslPeer(bool verify) { verify_ssl_peer_ = verify; }
    void setVerifySslHost(bool verify) { verify_ssl_host_ = verify; }
    void setVerifySslStatus(bool verify) { verify_ssl_status_ = verify; }
    void setVerifySsl(bool verify) { verify_ssl_peer_ = verify_ssl_host_ = verify_ssl_status_ = verify; }
    bool isVerifySslPeer() const { return verify_ssl_peer_; }
    bool isVerifySslHost() const { return verify_ssl_host_; }
    bool isVerifySslStatus() const { return verify_ssl_status_; }

    /* SSL cert & key */
    void setSslCert(SslCertificateType certType, const std::string& certFile) {
        ssl_cert_type_ = certType;
        ssl_cert_file_ = certFile;
    }
    void setSslKey(const std::string& keyFile) { ssl_key_file_ = keyFile; }
    void setSslKeyPassword(const std::string& pwd) { ssl_key_file_password_ = pwd; }
    const std::string& getSslCert() const { return ssl_cert_file_; }
    SslCertificateType getSslCertType() const { return ssl_cert_type_; }
    const std::string& getSslKey() const { return ssl_key_file_; }
    const std::string& getSslKeyPassword() const { return ssl_key_file_password_; }

    /* Proxy */
    void setProxy(const ProxyData& proxyData) { proxy_data_ = proxyData; }
    void removeProxy() { proxy_data_.host = std::string(); }
    bool hasProxy() const { return !proxy_data_.host.empty(); }
    void setProxy(ProxyType type, const std::string& host, uint32_t port,
        const std::string& user = "", const std::string& pwd = "",
        HttpAuthType authType = HttpAuthType::ANY)
    {
        proxy_data_.type = type;
        proxy_data_.host = host;
        proxy_data_.port = port;
        proxy_data_.username = user;
        proxy_data_.password = pwd;
        proxy_data_.auth_type = authType;
    }
    const ProxyData& getProxy() const { return proxy_data_; }

    /* Accept Encodings */
    void acceptAllEncodings() { std::vector<std::string>().swap(accept_encodings_); }
    void setAccpetEncodings(const std::vector<std::string>& encodings) { accept_encodings_ = encodings; }
    void addAcceptEncoding(const std::string& encoding) { accept_encodings_.emplace_back(encoding); }
    const std::vector<std::string>& getAcceptEncodings() const { return accept_encodings_;  }

    /* Header */
    void setHeader(std::string name, const std::string& value);
    void removeHeader(std::string name);
    void clearHeaders() { std::map<std::string, std::string>().swap(headers_); }
    const std::map<std::string, std::string>& getHeaders() const { return headers_; }
    const std::string& getHeader(std::string name) const;
    bool hasHeader(std::string name) const;

    /* Cookie */
    void setCookie(const std::string& cookie) { cookie_ = cookie; }
    void setCookieFile(const std::string& cookieFile) { cookie_file_ = cookieFile; }
    void removeCookie() { cookie_ = std::string(); cookie_file_ = std::string(); }
    const std::string& getCookie() const { return cookie_; }
    const std::string& getCookieFile() const { return cookie_file_; }

    /* Data */
    void setData(const std::string& data) { data_ = data; }
    void clearData() { data_ = std::string(); }
    const std::string& getData() const { return data_; }

    /* MimeField, string */
    void setMimeStringField(const std::string& name, const std::string value) { mime_string_fields_[name] = value; }
    void setMimeFileField(const std::string& name, const std::string value) { mime_file_fields_[name] = value; }
    void removeMimeStringField(const std::string& name);
    void removeMimeFileField(const std::string& name);
    void clearMimeStringFields() { std::map<std::string, std::string>().swap(mime_string_fields_); }
    void clearMimeFileField() { std::map<std::string, std::string>().swap(mime_file_fields_); }
    const std::string& getMimeStringField(const std::string& name) const;
    const std::string& getMimeFileField(const std::string& name) const;
    const std::map<std::string, std::string>& getMimeStringFields() const { return mime_string_fields_; }
    const std::map<std::string, std::string>& getMimeFileFields() const { return mime_file_fields_; }

    /* Download to file */
    void setDownloadFile(const std::string& downloadFile, size_t resume = false)
        { dl_file_ = downloadFile; dl_resume_ = resume; }
    void setDownloadFile(const std::string& downloadFile, const std::string& range)
        { dl_file_ = downloadFile; dl_resume_ = false; dl_range_ = range; }
    void setDownloadRange(const std::string& range) { dl_range_ = range; }
    const std::string& getDownloadFile() const { return dl_file_; }
    const std::string& getDownloadRange() const { return dl_range_; }
    bool isDownloadResume() const { return dl_resume_; }

    friend size_t curl_write_header(char* buffer, size_t size, size_t nitems, void* user_ptr);
    friend size_t curl_write_data(void* buffer, size_t size, size_t nitems, void* user_ptr);
    friend int curl_xfer_info(void* clientp, curl_off_t download_total_bytes, curl_off_t download_now_bytes,
        curl_off_t upload_total_bytes, curl_off_t upload_now_bytes);

private:
    std::string url_;
    http::Method http_method_ = http::Method::GET;
    http::Version http_version_ = http::Version::V1_1;

    /* Redirect */
    int max_redirects_ = -1;

    /* Timeout: millseconds */
    int timeout_ms_ = -1;

    /* Transfer progress */
    TransferProgressHandler on_transfer_progress_handler_ = nullptr;

    /* SSL */
    bool verify_ssl_peer_ = true;
    bool verify_ssl_host_ = true;
    bool verify_ssl_status_ = false;
    SslCertificateType ssl_cert_type_ = SslCertificateType::PME;
    std::string ssl_cert_file_;
    std::string ssl_key_file_;
    std::string ssl_key_file_password_;

    /* Proxy Data */
    ProxyData proxy_data_;

    /* Accept encodings */
    std::vector<std::string> accept_encodings_;

    /* Request headers */
    std::map<std::string, std::string> headers_;

    /* Post Data */
    std::string data_;

    /* Mime Fileds */
    std::map<std::string, std::string> mime_string_fields_;
    std::map<std::string, std::string> mime_file_fields_;

    /* Cookie */
    std::string cookie_;
    std::string cookie_file_;

    /* Download to file */
    std::string dl_file_;
    std::string dl_range_;
    bool dl_resume_ = false; // true -> dl_range_ will be ignored
};

}
#endif //__IC_REQUEST_H__

