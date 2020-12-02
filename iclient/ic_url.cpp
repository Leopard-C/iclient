#include "ic/ic_url.h"
#include "ic/ic_util.h"

namespace ic {

void Url::setBaseUrl(const std::string& base_url) {
    size_t len = base_url.length();
    if (len > 0) {
        if (base_url[len - 1] == '?') {
            base_url_ = base_url.substr(0, len - 1);
        }
        else {
            base_url_ = base_url;
        }
    }
    else {
        base_url_ = std::string();
    }
    baseurl_changed_ = true;
}

/* Add key-value, the same keys exist will be keep !!! */
void Url::addParam(const std::string& key, const std::string& value) {
    params_.emplace(key, value);
    params_changed_ = true;
}

/* Add key-value, the same keys exist will be remove !!! */
void Url::setParam(const std::string& key, const std::string& value) {
    removeParam(key);
    addParam(key, value);
}

void Url::removeParam(const std::string& key) {
    auto pair = params_.equal_range(key);
    if (pair.first != pair.second) {
        params_.erase(pair.first, pair.second);
        params_changed_ = true;
    }
}

void Url::clearParams() {
    std::multimap<std::string, std::string>().swap(params_);
    params_changed_ = true;
}

const std::string& Url::toString() {
    if (!baseurl_changed_ && !params_changed_) {
        return url_;
    }
    url_ = base_url_;
    if (!params_.empty()) {
        url_ += "?" + toParamString();
    }
    baseurl_changed_ = false;
    params_changed_ = false;
    return url_;
}

const std::string& Url::toParamString() {
    if (!params_changed_) {
        return params_str_;
    }
    params_changed_ = false;
    params_str_ = std::string();
    if (params_.empty()) {
        return params_str_;
    }
    for (auto& p : params_) {
        params_str_ += p.first;
        params_str_ += "=";
        params_str_ += util::escape(p.second);
        params_str_ += "&";
    }
    params_str_.pop_back();
    return params_str_;
}

} // namespace ic
