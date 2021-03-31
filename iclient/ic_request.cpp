#include "ic/ic_request.h"
#include "ic/ic_executor.h"
#include "ic/ic_util.h"
#ifdef ICLIENT_LOG
#  include "../log/logger.h"
#endif


namespace ic {

size_t end{ static_cast<size_t>(-1) };

std::string empty_string{};


/*
 *   SSL Certificate Type
**/
static const std::string STR_SSL_CERT_TYPE_UNKNOWN = "UNKNOWN";
static const std::string STR_SSL_CERT_TYPE_PEM = "PEM";
static const std::string STR_SSL_CERT_TYPE_DER = "DER";

const std::string& to_string(SslCertificateType type) {
    switch (type) {
    case SslCertificateType::PME:
        return STR_SSL_CERT_TYPE_PEM;
    case SslCertificateType::DER:
        return STR_SSL_CERT_TYPE_DER;
    default:
        return STR_SSL_CERT_TYPE_UNKNOWN;
    }
}


/*
 *  Proxy Type
**/
int to_curl(ProxyType type) {
    return type == ProxyType::HTTPS ? CURLPROXY_HTTPS : CURLPROXY_HTTP;
}

static const std::string STR_PROXY_TYPE_HTTP  = "HTTP";
static const std::string STR_PROXY_TYPE_HTTPS = "HTTPS";

const std::string& to_string(ProxyType type) {
    return type == ProxyType::HTTPS ? STR_PROXY_TYPE_HTTPS : STR_PROXY_TYPE_HTTP;
}


/*
 *  Http Auth Type
**/
int to_curl(HttpAuthType type) {
    switch (type) {
    case HttpAuthType::BASIC:
        return CURLAUTH_BASIC;
    case HttpAuthType::DIGEST:
        return CURLAUTH_DIGEST;
    case HttpAuthType::ANY_SAFE:
        return CURLAUTH_ANYSAFE;
    case HttpAuthType::ANY:
    default:
        return CURLAUTH_ANY;
    }
}

static const std::string STR_BASIC = "BASIC";
static const std::string STR_DIGEST = "DIGEST";
static const std::string STR_ANY = "ANY";
static const std::string STR_ANY_SAFE = "ANY_SAFE";

const std::string& to_string(HttpAuthType type) {
    switch (type) {
    case HttpAuthType::BASIC:
        return STR_BASIC;
    case HttpAuthType::DIGEST:
        return STR_DIGEST;
    case HttpAuthType::ANY_SAFE:
        return STR_ANY_SAFE;
    case HttpAuthType::ANY:
    default:
        return STR_ANY;
    }
}


/* Perform Request */
Response Request::perform() {
#ifdef ICLIENT_LOG
    LInfo("iClient: {}, Proxy: {}", url_, proxy_data_.host);
#endif

    /* Calculate 'Host' field */
    if (!hasHeader("Host")) {
        std::string host = util::get_host(url_);
        if (host.empty()) {
            return Status::INVALID_URL;
        }
    }

    Executor executor;
    return executor.perform(this);
}


/*
 *  Header
**/
void Request::setHeader(std::string name, const std::string& value) {
    util::tolower(name);
    auto findIt = headers_.find(name);
    if (findIt == headers_.end()) {
        headers_.emplace(name, value);
    }
    else {
        findIt->second = value;
    }
}

void Request::removeHeader(std::string name) {
    util::tolower(name);
    auto findIt = headers_.find(name);
    if (findIt != headers_.end()) {
        headers_.erase(findIt);
    }
}

const std::string& Request::getHeader(std::string name) const {
    util::tolower(name);
    auto findIt = headers_.find(name);
    if (findIt == headers_.end()) {
        return empty_string;
    }
    return findIt->second;
}

bool Request::hasHeader(std::string name) const {
    util::tolower(name);
    return headers_.find(name) != headers_.end();
}


/*
 *  Mime
**/
void Request::removeMimeStringField(const std::string& name) {
    auto findIt = mime_string_fields_.find(name);
    if (findIt != mime_string_fields_.end()) {
        mime_string_fields_.erase(findIt);
    }
}

void Request::removeMimeFileField(const std::string& name) {
    auto findIt = mime_file_fields_.find(name);
    if (findIt != mime_file_fields_.end()) {
        mime_file_fields_.erase(findIt);
    }
}

const std::string& Request::getMimeStringField(const std::string& name) const {
    auto findIt = mime_string_fields_.find(name);
    if (findIt == mime_string_fields_.end()) {
        return empty_string;
    }
    return findIt->second;
}

const std::string& Request::getMimeFileField(const std::string& name) const {
    auto findIt = mime_file_fields_.find(name);
    if (findIt == mime_file_fields_.end()) {
        return empty_string;
    }
    return findIt->second;
}


/*
 *  Download to file
 */
void Request::setDownloadRange(size_t bytes_start, size_t bytes_end/* = end*/) {
    dl_range_ = std::to_string(bytes_start) + "-";
    if (bytes_end != end) {
        dl_range_ += std::to_string(bytes_end);
    }
}


} // namespace ic

