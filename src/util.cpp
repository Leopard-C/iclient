#include "util.h"
#include "iclient/curl_inc.h"

namespace ic {
namespace client {
namespace util {

/**
 * @brief URL encode
 */
std::string escape(const std::string& str) {
    char* escaped = curl_escape(str.c_str(), static_cast<int>(str.length()));
    if (escaped) {
        std::string ret(escaped);
        curl_free(escaped);
        return ret;
    }
    return {};
}

/**
 * @brief URL decode
 */
std::string unescape(const std::string& str) {
    char* unescaped = curl_unescape(str.c_str(), static_cast<int>(str.length()));
    if (unescaped) {
        std::string ret(unescaped);
        curl_free(unescaped);
        return ret;
    }
    return {};
}

/**
 * @brief Merge string array
 * @param strs string array
 * @param sep separator string
 */
std::string join(const std::vector<std::string>& strs, const std::string& sep) {
    if (strs.empty()) {
        return {};
    }

    size_t length = 0;
    const size_t strs_size = strs.size();
    const size_t sep_length = sep.length();
    for (size_t i = 0; i < strs_size; ++i) {
        length += strs[i].length() + sep_length;
    }

    std::string joined;
    joined.reserve(length);
    joined += strs[0];
    for (size_t i = 1; i < strs_size; ++i) {
        joined += sep;
        joined += strs[i];
    }

    return joined;
}

std::string& trim(std::string& str) {
    str.erase(str.find_last_not_of(' ') + 1);
    str.erase(0, str.find_first_not_of(' '));
    return str;
}

std::string& to_upper(std::string& str) {
    for (char& c : str) {
        if (c >= 'a' && c <= 'z') {
            c -= 32;
        }
    }
    return str;
}

std::string& to_lower(std::string& str) {
    for (char& c : str) {
        if (c >= 'A' && c <= 'Z') {
            c += 32;
        }
    }
    return str;
}

long get_file_size(const std::string& filename) {
    FILE* fp = fopen(filename.c_str(), "rb");
    if (!fp) {
        return -1;
    }
    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);
    //fseek(fp, 0L, SEEK_SET);
    fclose(fp);
    return size;
}

/**
 * @brief Calculate "Host" filed in request header
 */
std::string get_host(const std::string& url) {
    auto pos_start = url.find("://");
    if (pos_start == std::string::npos) {
        pos_start = 0;   /* eg.  www.example.com/example */
    }
    else {
        pos_start += 3;  /* eg.  https://www.example.com/example */
    }
    auto pos_end = url.find('/', pos_start);
    return url.substr(pos_start, pos_end - pos_start);
}

} // namespace util
} // namespace client
} // namespace ic
