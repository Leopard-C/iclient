#include "iclient/request.h"
#include "executor.h"
#include "util.h"

namespace ic {
namespace client {

size_t end{ static_cast<size_t>(-1) };
std::string empty_string;

static const char* STR_SSL_CERT_TYPE_UNKNOWN = "UNKNOWN";
static const char* STR_SSL_CERT_TYPE_PEM = "PEM";
static const char* STR_SSL_CERT_TYPE_DER = "DER";

/**
 * @brief SSL Certificate Type.
 */
const char* to_string(SslCertificateType type) {
    switch (type) {
        case SslCertificateType::PME: return STR_SSL_CERT_TYPE_PEM;
        case SslCertificateType::DER: return STR_SSL_CERT_TYPE_DER;
        default:                      return STR_SSL_CERT_TYPE_UNKNOWN;
    }
}

/**
 * @brief Proxy Type.
 */
int to_curl(ProxyType type) {
    return type == ProxyType::HTTPS ? CURLPROXY_HTTPS : CURLPROXY_HTTP;
}

static const char* STR_PROXY_TYPE_HTTP  = "HTTP";
static const char* STR_PROXY_TYPE_HTTPS = "HTTPS";

const char* to_string(ProxyType type) {
    return type == ProxyType::HTTPS ? STR_PROXY_TYPE_HTTPS : STR_PROXY_TYPE_HTTP;
}

/**
 * @brief Http Auth Type
 */
unsigned long to_curl(HttpAuthType type) {
    switch (type) {
        case HttpAuthType::BASIC:    return CURLAUTH_BASIC;
        case HttpAuthType::DIGEST:   return CURLAUTH_DIGEST;
        case HttpAuthType::ANY_SAFE: return CURLAUTH_ANYSAFE;
        case HttpAuthType::ANY:
        default:                     return CURLAUTH_ANY;
    }
}

static const char* STR_BASIC = "BASIC";
static const char* STR_DIGEST = "DIGEST";
static const char* STR_ANY = "ANY";
static const char* STR_ANY_SAFE = "ANY_SAFE";

const char* to_string(HttpAuthType type) {
    switch (type) {
        case HttpAuthType::BASIC:    return STR_BASIC;
        case HttpAuthType::DIGEST:   return STR_DIGEST;
        case HttpAuthType::ANY_SAFE: return STR_ANY_SAFE;
        case HttpAuthType::ANY:
        default:                     return STR_ANY;
    }
}

/**
 * @brief Perform request
 */
Response Request::Perform() {
    if (header("Host").empty()) {
        // Calculate 'Host' field
        std::string host = util::get_host(url_);
        if (host.empty()) {
            return Status::INVALID_URL;
        }
    }
    cancel_ = false;
    Response response;
    Executor executor(*this, response);
    executor.Perform();
    return response;
}

void Request::Cancel() {
    cancel_ = true;
}

void Request::set_header(std::string name, const std::string& value) {
    util::to_lower(name);
    headers_[name] = value;
}

void Request::RemoveHeader(std::string name) {
    util::to_lower(name);
    auto iter = headers_.find(name);
    if (iter != headers_.end()) {
        headers_.erase(iter);
    }
}

const std::string& Request::header(std::string name) const {
    util::to_lower(name);
    auto iter = headers_.find(name);
    if (iter != headers_.end()) {
        return iter->second;
    }
    return empty_string;
}

void Request::RemoveMimeStringField(const std::string& name) {
    auto iter = mime_string_fields_.find(name);
    if (iter != mime_string_fields_.end()) {
        mime_string_fields_.erase(iter);
    }
}

void Request::RemoveMimeFileField(const std::string& name) {
    auto iter = mime_file_fields_.find(name);
    if (iter != mime_file_fields_.end()) {
        mime_file_fields_.erase(iter);
    }
}

const std::string& Request::mime_string_field(const std::string& name) const {
    auto iter = mime_string_fields_.find(name);
    if (iter != mime_string_fields_.end()) {
        return iter->second;
    }
    return empty_string;
}

const std::string& Request::nime_file_field(const std::string& name) const {
    auto iter = mime_file_fields_.find(name);
    if (iter != mime_file_fields_.end()) {
        return iter->second;
    }
    return empty_string;
}

void Request::set_download_range(size_t bytes_start, size_t bytes_end/* = end*/) {
    download_range_ = std::to_string(bytes_start) + '-';
    if (bytes_end != end) {
        download_range_ += std::to_string(bytes_end);
    }
}

void Request::set_proxy(const std::string& host, uint32_t port, ProxyType type/* = ProxyType::HTTP*/) {
    proxy_data_.host = host;
    proxy_data_.port = port;
    proxy_data_.type = type;
    proxy_data_.username.clear();
    proxy_data_.password.clear();
    proxy_data_.auth_type = HttpAuthType::ANY;
}

} // namespace client
} // namespace ic
