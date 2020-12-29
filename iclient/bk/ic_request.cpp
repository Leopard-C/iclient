#include "ic/ic_request.h"
#include "ic/ic_executor.h"
#include "ic/ic_util.h"


namespace ic {

std::string ic_empty_string{};

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
        return ic_empty_string;
    }
    return findIt->second;
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
        return ic_empty_string;
    }
    return findIt->second;
}

const std::string& Request::getMimeFileField(const std::string& name) const {
    auto findIt = mime_file_fields_.find(name);
    if (findIt == mime_file_fields_.end()) {
        return ic_empty_string;
    }
    return findIt->second;
}

} // namespace ic

