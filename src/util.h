#ifndef IC_CLIENT_UTIL_H_
#define IC_CLIENT_UTIL_H_

#include <string>
#include <vector>

namespace ic {
namespace client {
namespace util {

/**
 * @brief URL encode
 */
std::string escape(const std::string& str);

/**
 * @brief URL decode
 */
std::string unescape(const std::string& str);

/**
 * @brief Merge string array
 * @param strs string array
 * @param sep separator string
 */
std::string join(const std::vector<std::string>& strs, const std::string& sep);

std::string& trim(std::string& str);

std::string& to_upper(std::string& str);
std::string& to_lower(std::string& str);

long get_file_size(const std::string& filename);

/**
 * @brief Calculate "Host" filed in request header
 */
std::string get_host(const std::string& url);

} // namespace util
} // namespace client
} // namespace ic

#endif // IC_CLIENT_UTIL_H_
