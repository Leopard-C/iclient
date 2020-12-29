#ifndef __IC_URL_H__
#define __IC_URL_H__
#include <map>
#include <string>

namespace ic {

class Url {
public:
    Url() = default;
    Url(const std::string& base_url) { setBaseUrl(base_url); }

    void setBaseUrl(const std::string& base_url);

    /* Add key-value, the same keys exist will be keep !!! */
    void addParam(const std::string& key, const std::string& value);
    void addParam(const std::string& key, const char* value) {
        addParam(key, std::string(value));
    }
    template<typename T>
    void addParam(const std::string& key, const T& value) {
        addParam(key, std::to_string(value));
    }

    /* Add key-value, the same keys exist will be remove !!! */
    void setParam(const std::string& key, const std::string& value);
    void setParam(const std::string& key, const char* value) {
        setParam(key, std::string(value));
    }
    template<typename T>
    void setParam(const std::string& key, const T& value) {
        setParam(key, std::to_string(value));
    }

    void removeParam(const std::string& key);

    void clearParams();

    const std::string& toString();
    const std::string& toParamString();

private:
    bool baseurl_changed_ = false;
    bool params_changed_ = false;
    std::string base_url_;
    std::string params_str_;
    std::string url_;
    std::multimap<std::string, std::string> params_;
};

} // namespace ic

#endif // __IC_URL_H__
