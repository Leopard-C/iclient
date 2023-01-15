#include "iclient/url.h"
#include "util.h"

namespace ic {
namespace client {

void Url::set_base_url(const std::string& base_url) {
    baseurl_changed_ = true;
    base_url_ = base_url;
    util::trim(base_url_);
    auto pos = base_url_.find('?');
    if (pos != std::string::npos) {
        base_url_ = base_url_.substr(0, pos);
    }
}

void Url::AddParam(const std::string& key, const std::string& value) {
    params_.emplace(key, value);
    params_changed_ = true;
}

void Url::SetParam(const std::string& key, const std::string& value) {
    RemoveParam(key);
    AddParam(key, value);
}

void Url::RemoveParam(const std::string& key) {
    auto pair = params_.equal_range(key);
    if (pair.first != pair.second) {
        params_.erase(pair.first, pair.second);
        params_changed_ = true;
    }
}

void Url::ClearParams() {
    std::multimap<std::string, std::string>().swap(params_);
    params_changed_ = true;
}

const std::string& Url::ToString() {
    if (!baseurl_changed_ && !params_changed_) {
        return url_;
    }
    url_ = base_url_;
    if (!params_.empty()) {
        url_ += '?' + GetParamsString();
    }
    baseurl_changed_ = false;
    params_changed_ = false;
    return url_;
}

const std::string& Url::GetParamsString() {
    if (!params_changed_) {
        return params_str_;
    }
    params_changed_ = false;
    params_str_.clear();
    if (params_.empty()) {
        return params_str_;
    }
    for (const auto& p : params_) {
        params_str_ += p.first;
        params_str_ += '=';
        params_str_ += util::escape(p.second);
        params_str_ += '&';
    }
    params_str_.pop_back();
    return params_str_;
}

} // namespace client
} // namespace ic
