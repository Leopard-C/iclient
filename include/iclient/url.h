/**
 * @file url.h
 * @author Leopard-C (leopard.c@outlook.com)
 * @version 0.1
 * @date 2023-01-12
 * 
 * @copyright Copyright (c) 2021-present, Jinbao Chen.
 */
#ifndef IC_CLIENT_URL_H_
#define IC_CLIENT_URL_H_
#include <map>
#include <string>

namespace ic {
namespace client {

class Url {
public:
    Url() = default;
    Url(const std::string& base_url) { set_base_url(base_url); }

    void set_base_url(const std::string& base_url);

    /**
     * @brief Add key-value, the same keys exist will be kept.
     */
    void AddParam(const std::string& key, const std::string& value);
    void AddParam(const std::string& key, const char* value) {
        AddParam(key, std::string(value));
    }
    void AddParam(const std::string& key, char* const value) {
        AddParam(key, std::string(value));
    }
    template<typename T>
    void AddParam(const std::string& key, const T& value) {
        AddParam(key, std::to_string(value));
    }

    /**
     * @brief Set key-value, the same keys exist will be removed.
     */
    void SetParam(const std::string& key, const std::string& value);
    void SetParam(const std::string& key, const char* value) {
        SetParam(key, std::string(value));
    }
    void SetParam(const std::string& key, char* const value) {
        SetParam(key, std::string(value));
    }
    template<typename T>
    void SetParam(const std::string& key, const T& value) {
        SetParam(key, std::to_string(value));
    }

    void RemoveParam(const std::string& key);

    void ClearParams();

    const std::string& ToString();
    const std::string& GetParamsString();

private:
    bool baseurl_changed_ = false;
    bool params_changed_ = false;
    std::string base_url_;
    std::string params_str_;
    std::string url_;
    std::multimap<std::string, std::string> params_;
};

} // namespace client
} // namespace ic

#endif // IC_CLIENT_URL_H_
