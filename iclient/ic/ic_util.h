/*******************************************************
** namespace:   ic::util
**
** description: helper functions
**
** author: Leopard-C
**
** update: 2020/12/02
*******************************************************/
#ifndef __IC_UTIL_H__
#define __IC_UTIL_H__

#include <string>
#include <vector>

namespace ic {
namespace util {

/*
 *  URL encode
**/
std::string escape(const std::string& str);

/*
 *  URL decode
**/
std::string unescape(const std::string& str);

/*
 *  @sep: separator string
**/
std::string join(const std::vector<std::string>& strs, const std::string& sep);

/*
 *  trim spaces on the left and right sides.
**/
std::string& trim(std::string& str);

/*
 *  upper and lower case
**/
std::string& toupper(std::string& str);
std::string& tolower(std::string& str);

/*
 *  Get file size
**/
long get_file_size(const std::string& filename);

} // namespace util
} // namespace ic

#endif // __IC_UTIL_H__
