#include "iclient/http.h"
#include "iclient/curl_inc.h"

namespace ic {
namespace client {
namespace http {

/***********************************************************************
 * 
 *                     1. Http Method
 * 
************************************************************************/

static const char* STR_METHOD_HTTP_UNKNOWN = "UNKNOWN";
static const char* STR_METHOD_HTTP_GET = "GET";
static const char* STR_METHOD_HTTP_HEAD = "HEAD";
static const char* STR_METHOD_HTTP_POST = "POST";
static const char* STR_METHOD_HTTP_PUT = "PUT";
static const char* STR_METHOD_HTTP_DELETE = "DELETE";
static const char* STR_METHOD_HTTP_CONNECT = "CONNECT";
static const char* STR_METHOD_HTTP_OPTIONS = "OPTIONS";
static const char* STR_METHOD_HTTP_PATCH = "PATCH";

#ifdef CASE
#undef CASE
#endif

#define CASE(type) case Method::type: return STR_METHOD_##type

const char* to_string(Method method) {
    switch (method) {
        CASE(HTTP_GET);
        CASE(HTTP_HEAD);
        CASE(HTTP_POST);
        CASE(HTTP_OPTIONS);
        CASE(HTTP_PUT);
        CASE(HTTP_DELETE);
        CASE(HTTP_CONNECT);
        CASE(HTTP_PATCH);
        CASE(HTTP_UNKNOWN);
        default: return STR_METHOD_HTTP_UNKNOWN;
    }
}

/***********************************************************************
 * 
 *                     2. Http Version
 * 
************************************************************************/

#undef  CASE
#define CASE(ver) case Version::V##ver: return CURL_HTTP_VERSION_##ver

int to_curl_http_version(Version ver) {
    switch (ver) {
        CASE(1_0);
        CASE(1_1);
        CASE(2_0);
        CASE(2TLS);
        default: return CURL_HTTP_VERSION_NONE;
    }
}

#undef  CASE
#define CASE(ver) case CURL_HTTP_VERSION_##ver: return Version::V##ver

Version from_curl_http_version(int ver) {
    switch (ver) {
        CASE(1_0);
        CASE(1_1);
        CASE(2_0);
        CASE(2TLS);
        default: return Version::NONE;
    }
}

static const char* STR_VERSION_NONE = "HTTP/None";
static const char* STR_VERSION_V1_0 = "HTTP/1.0";
static const char* STR_VERSION_V1_1 = "HTTP/1.1";
static const char* STR_VERSION_V2_0 = "HTTP/2.0";
static const char* STR_VERSION_V2TLS = "HTTP/2.0-TLS";

#undef  CASE
#define CASE(type) case Version::type: return STR_VERSION_##type

const char* to_string(Version ver) {
    switch (ver) {
        CASE(V1_0);
        CASE(V1_1);
        CASE(V2_0);
        CASE(V2TLS);
        CASE(NONE);
        default: return STR_VERSION_NONE;
    }
}

/***********************************************************************
 * 
 *                     3. Http Status Code 
 * 
************************************************************************/

StatusCode from_curl_http_response_code(int code) {
    switch (code) {
        case 100: return StatusCode::HTTP_100_CONTINUE;
        case 101: return StatusCode::HTTP_101_SWITCHING_PROTOCOLS;
        case 102: return StatusCode::HTTP_102_PROCESSING;
        case 103: return StatusCode::HTTP_103_EARLY_HINTS;

        case 200: return StatusCode::HTTP_200_OK;
        case 201: return StatusCode::HTTP_201_CREATED;
        case 202: return StatusCode::HTTP_202_ACCEPTED;
        case 203: return StatusCode::HTTP_203_NON_AUTHORITATIVE_INFORMATION;
        case 204: return StatusCode::HTTP_204_NO_CONTENT;
        case 205: return StatusCode::HTTP_205_RESET_CONTENT;
        case 206: return StatusCode::HTTP_206_PARTIAL_CONTENT;
        case 207: return StatusCode::HTTP_207_MULTI_STATUS;
        case 208: return StatusCode::HTTP_208_ALREADY_REPORTED;
        case 226: return StatusCode::HTTP_226_IM_USED;

        case 300: return StatusCode::HTTP_300_MULTIPLE_CHOICES;
        case 301: return StatusCode::HTTP_301_MOVED_PERMANENTLY;
        case 302: return StatusCode::HTTP_302_FOUND;
        case 303: return StatusCode::HTTP_303_SEE_OTHER;
        case 304: return StatusCode::HTTP_304_NOT_MODIFIED;
        case 305: return StatusCode::HTTP_305_USE_PROXY;
        case 306: return StatusCode::HTTP_306_SWITCH_PROXY;
        case 307: return StatusCode::HTTP_307_TEMPORARY_REDIRECT;
        case 308: return StatusCode::HTTP_308_PERMANENT_REDIRECT;

        case 400: return StatusCode::HTTP_400_BAD_REQUEST;
        case 401: return StatusCode::HTTP_401_UNAUTHORIZED;
        case 402: return StatusCode::HTTP_402_PAYMENT_REQUIRED;
        case 403: return StatusCode::HTTP_403_FORBIDDEN;
        case 404: return StatusCode::HTTP_404_NOT_FOUND;
        case 405: return StatusCode::HTTP_405_METHOD_NOT_ALLOWED;
        case 406: return StatusCode::HTTP_406_NOT_ACCEPTABLE;
        case 407: return StatusCode::HTTP_407_PROXY_AUTHENTICATION_REQUIRED;
        case 408: return StatusCode::HTTP_408_REQUEST_TIMEOUT;
        case 409: return StatusCode::HTTP_409_CONFLICT;
        case 410: return StatusCode::HTTP_410_GONE;
        case 411: return StatusCode::HTTP_411_LENGTH_REQUIRED;
        case 412: return StatusCode::HTTP_412_PRECONDITION_FAILED;
        case 413: return StatusCode::HTTP_413_PAYLOAD_TOO_LARGE;
        case 414: return StatusCode::HTTP_414_URI_TOO_LONG;
        case 415: return StatusCode::HTTP_415_UNSUPPORTED_MEDIA_TYPE;
        case 416: return StatusCode::HTTP_416_RANGE_NOT_SATISFIABLE;
        case 417: return StatusCode::HTTP_417_EXPECTATION_FAILED;
        case 418: return StatusCode::HTTP_418_IM_A_TEAPOT;
        case 421: return StatusCode::HTTP_421_MISDIRECTED_REQUEST;
        case 422: return StatusCode::HTTP_422_UNPROCESSABLE_ENTITY;
        case 423: return StatusCode::HTTP_423_LOCKED;
        case 424: return StatusCode::HTTP_424_FAILED_DEPENDENCY;
        case 425: return StatusCode::HTTP_425_TOO_EARLY;
        case 426: return StatusCode::HTTP_426_UPGRADE_REQUIRED;
        case 428: return StatusCode::HTTP_428_PRECONDITION_REQUIRED;
        case 429: return StatusCode::HTTP_429_TOO_MANY_REQUESTS;
        case 431: return StatusCode::HTTP_431_REQUEST_HEADER_FIELDS_TOO_LARGE;
        case 451: return StatusCode::HTTP_451_UNAVAILABLE_FOR_LEGAL_REASONS;

        case 500: return StatusCode::HTTP_500_INTERNAL_SERVER_ERROR;
        case 501: return StatusCode::HTTP_501_NOT_IMPLEMENTED;
        case 502: return StatusCode::HTTP_502_BAD_GATEWAY;
        case 503: return StatusCode::HTTP_503_SERVICE_UNAVAILABLE;
        case 504: return StatusCode::HTTP_504_GATEWAY_TIMEOUT;
        case 505: return StatusCode::HTTP_505_HTTP_VERSION_NOT_SUPPORTED;
        case 506: return StatusCode::HTTP_506_VARIANT_ALSO_NEGOTIATES;
        case 507: return StatusCode::HTTP_507_INSUFFICIENT_STORAGE;
        case 508: return StatusCode::HTTP_508_LOOP_DETECTED;
        case 510: return StatusCode::HTTP_510_NOT_EXTENDED;
        case 511: return StatusCode::HTTP_511_NETWORK_AUTHENTICATION_REQUIRED;

        case 0:
        default:  return StatusCode::HTTP_UNKNOWN;
    }
}

#if 1 // Http Status Code Message
static const char* STR_STATUS_CODE_HTTP_UNKNOWN = "UNKNOWN";
static const char* STR_STATUS_CODE_HTTP_100_CONTINUE = "100 Continue";
static const char* STR_STATUS_CODE_HTTP_101_SWITCHING_PROTOCOLS = "101 Switching Protocols";
static const char* STR_STATUS_CODE_HTTP_102_PROCESSING = "102 Processing";
static const char* STR_STATUS_CODE_HTTP_103_EARLY_HINTS = "103 Early Hints";
static const char* STR_STATUS_CODE_HTTP_200_OK = "200 OK";
static const char* STR_STATUS_CODE_HTTP_201_CREATED = "201 Created";
static const char* STR_STATUS_CODE_HTTP_202_ACCEPTED = "202 Accepted";
static const char* STR_STATUS_CODE_HTTP_203_NON_AUTHORITATIVE_INFORMATION = "203 Non-Authoritative Information";
static const char* STR_STATUS_CODE_HTTP_204_NO_CONTENT = "204 No Content";
static const char* STR_STATUS_CODE_HTTP_205_RESET_CONTENT = "205 Reset Content";
static const char* STR_STATUS_CODE_HTTP_206_PARTIAL_CONTENT = "206 Partial Content";
static const char* STR_STATUS_CODE_HTTP_207_MULTI_STATUS = "207 Multi-Status";
static const char* STR_STATUS_CODE_HTTP_208_ALREADY_REPORTED = "208 Already Reported";
static const char* STR_STATUS_CODE_HTTP_226_IM_USED = "226 IM Used";
static const char* STR_STATUS_CODE_HTTP_300_MULTIPLE_CHOICES = "300 Multiple Choices";
static const char* STR_STATUS_CODE_HTTP_301_MOVED_PERMANENTLY = "301 Moved Permanently";
static const char* STR_STATUS_CODE_HTTP_302_FOUND = "302 Found";
static const char* STR_STATUS_CODE_HTTP_303_SEE_OTHER = "303 See Other";
static const char* STR_STATUS_CODE_HTTP_304_NOT_MODIFIED = "304 Not Modified";
static const char* STR_STATUS_CODE_HTTP_305_USE_PROXY = "305 Use Proxy";
static const char* STR_STATUS_CODE_HTTP_306_SWITCH_PROXY = "306 Switch Proxy";
static const char* STR_STATUS_CODE_HTTP_307_TEMPORARY_REDIRECT = "307 Temporary Redirect";
static const char* STR_STATUS_CODE_HTTP_308_PERMANENT_REDIRECT = "308 Permanent Redirect";
static const char* STR_STATUS_CODE_HTTP_400_BAD_REQUEST = "400 Bad Request";
static const char* STR_STATUS_CODE_HTTP_401_UNAUTHORIZED = "401 Unauthorized";
static const char* STR_STATUS_CODE_HTTP_402_PAYMENT_REQUIRED = "402 Payment Required";
static const char* STR_STATUS_CODE_HTTP_403_FORBIDDEN = "403 Forbidden";
static const char* STR_STATUS_CODE_HTTP_404_NOT_FOUND = "404 Not Found";
static const char* STR_STATUS_CODE_HTTP_405_METHOD_NOT_ALLOWED = "405 Method Not Allowed";
static const char* STR_STATUS_CODE_HTTP_406_NOT_ACCEPTABLE = "406 Not Acceptable";
static const char* STR_STATUS_CODE_HTTP_407_PROXY_AUTHENTICATION_REQUIRED = "407 Proxy Authentication Required";
static const char* STR_STATUS_CODE_HTTP_408_REQUEST_TIMEOUT = "408 Request Timeout";
static const char* STR_STATUS_CODE_HTTP_409_CONFLICT = "409 Conflict";
static const char* STR_STATUS_CODE_HTTP_410_GONE = "410 Gone";
static const char* STR_STATUS_CODE_HTTP_411_LENGTH_REQUIRED = "411 Length Required";
static const char* STR_STATUS_CODE_HTTP_412_PRECONDITION_FAILED = "412 Precondition Failed";
static const char* STR_STATUS_CODE_HTTP_413_PAYLOAD_TOO_LARGE = "413 Payload Too Large";
static const char* STR_STATUS_CODE_HTTP_414_URI_TOO_LONG = "414 URI Too Long";
static const char* STR_STATUS_CODE_HTTP_415_UNSUPPORTED_MEDIA_TYPE = "415 Unsupported Media Type";
static const char* STR_STATUS_CODE_HTTP_416_RANGE_NOT_SATISFIABLE = "416 Range Not Satisfiable";
static const char* STR_STATUS_CODE_HTTP_417_EXPECTATION_FAILED = "417 Expectation Failed";
static const char* STR_STATUS_CODE_HTTP_418_IM_A_TEAPOT = "418 I'm a teapot";
static const char* STR_STATUS_CODE_HTTP_421_MISDIRECTED_REQUEST = "421 Misdirected Request";
static const char* STR_STATUS_CODE_HTTP_422_UNPROCESSABLE_ENTITY = "422 Unprocessable Entity";
static const char* STR_STATUS_CODE_HTTP_423_LOCKED = "423 Locked";
static const char* STR_STATUS_CODE_HTTP_424_FAILED_DEPENDENCY = "424 Failed Dependency";
static const char* STR_STATUS_CODE_HTTP_425_TOO_EARLY = "425 Too Early";
static const char* STR_STATUS_CODE_HTTP_426_UPGRADE_REQUIRED = "426 Upgrade Required";
static const char* STR_STATUS_CODE_HTTP_428_PRECONDITION_REQUIRED = "428 Precondition Required";
static const char* STR_STATUS_CODE_HTTP_429_TOO_MANY_REQUESTS = "429 Too Many Requests";
static const char* STR_STATUS_CODE_HTTP_431_REQUEST_HEADER_FIELDS_TOO_LARGE = "431 Request Header Fields Too Large";
static const char* STR_STATUS_CODE_HTTP_451_UNAVAILABLE_FOR_LEGAL_REASONS = "451 Unavailable For Legal Reasons";
static const char* STR_STATUS_CODE_HTTP_500_INTERNAL_SERVER_ERROR = "500 Internal Server Error";
static const char* STR_STATUS_CODE_HTTP_501_NOT_IMPLEMENTED = "501 Not Implemented";
static const char* STR_STATUS_CODE_HTTP_502_BAD_GATEWAY = "502 Bad Gateway";
static const char* STR_STATUS_CODE_HTTP_503_SERVICE_UNAVAILABLE = "503 Service Unavailable";
static const char* STR_STATUS_CODE_HTTP_504_GATEWAY_TIMEOUT = "504 Gateway Timeout";
static const char* STR_STATUS_CODE_HTTP_505_HTTP_VERSION_NOT_SUPPORTED = "505 HTTP Version Not Supported";
static const char* STR_STATUS_CODE_HTTP_506_VARIANT_ALSO_NEGOTIATES = "506 Variant Also Negotiates";
static const char* STR_STATUS_CODE_HTTP_507_INSUFFICIENT_STORAGE = "507 Insufficient Storage";
static const char* STR_STATUS_CODE_HTTP_508_LOOP_DETECTED = "508 Loop Detected";
static const char* STR_STATUS_CODE_HTTP_510_NOT_EXTENDED = "510 Not Extended";
static const char* STR_STATUS_CODE_HTTP_511_NETWORK_AUTHENTICATION_REQUIRED = "511 Network Authentication Required";
#endif

#undef  CASE
#define CASE(code) case StatusCode::HTTP_##code: return STR_STATUS_CODE_HTTP_##code

const char* to_string(StatusCode code) {
    switch (code) {
        CASE(UNKNOWN);
        CASE(100_CONTINUE);
        CASE(101_SWITCHING_PROTOCOLS);
        CASE(102_PROCESSING);
        CASE(103_EARLY_HINTS);

        CASE(200_OK);
        CASE(201_CREATED);
        CASE(202_ACCEPTED);
        CASE(203_NON_AUTHORITATIVE_INFORMATION);
        CASE(204_NO_CONTENT);
        CASE(205_RESET_CONTENT);
        CASE(206_PARTIAL_CONTENT);
        CASE(207_MULTI_STATUS);
        CASE(208_ALREADY_REPORTED);
        CASE(226_IM_USED);

        CASE(300_MULTIPLE_CHOICES);
        CASE(301_MOVED_PERMANENTLY);
        CASE(302_FOUND);
        CASE(303_SEE_OTHER);
        CASE(304_NOT_MODIFIED);
        CASE(305_USE_PROXY);
        CASE(306_SWITCH_PROXY);
        CASE(307_TEMPORARY_REDIRECT);
        CASE(308_PERMANENT_REDIRECT);

        CASE(400_BAD_REQUEST);
        CASE(401_UNAUTHORIZED);
        CASE(402_PAYMENT_REQUIRED);
        CASE(403_FORBIDDEN);
        CASE(404_NOT_FOUND);
        CASE(405_METHOD_NOT_ALLOWED);
        CASE(406_NOT_ACCEPTABLE);
        CASE(407_PROXY_AUTHENTICATION_REQUIRED);
        CASE(408_REQUEST_TIMEOUT);
        CASE(409_CONFLICT);
        CASE(410_GONE);
        CASE(411_LENGTH_REQUIRED);
        CASE(412_PRECONDITION_FAILED);
        CASE(413_PAYLOAD_TOO_LARGE);
        CASE(414_URI_TOO_LONG);
        CASE(415_UNSUPPORTED_MEDIA_TYPE);
        CASE(416_RANGE_NOT_SATISFIABLE);
        CASE(417_EXPECTATION_FAILED);
        CASE(418_IM_A_TEAPOT);
        CASE(421_MISDIRECTED_REQUEST);
        CASE(422_UNPROCESSABLE_ENTITY);
        CASE(423_LOCKED);
        CASE(424_FAILED_DEPENDENCY);
        CASE(425_TOO_EARLY);
        CASE(426_UPGRADE_REQUIRED);
        CASE(428_PRECONDITION_REQUIRED);
        CASE(429_TOO_MANY_REQUESTS);
        CASE(431_REQUEST_HEADER_FIELDS_TOO_LARGE);
        CASE(451_UNAVAILABLE_FOR_LEGAL_REASONS);

        CASE(500_INTERNAL_SERVER_ERROR);
        CASE(501_NOT_IMPLEMENTED);
        CASE(502_BAD_GATEWAY);
        CASE(503_SERVICE_UNAVAILABLE);
        CASE(504_GATEWAY_TIMEOUT);
        CASE(505_HTTP_VERSION_NOT_SUPPORTED);
        CASE(506_VARIANT_ALSO_NEGOTIATES);
        CASE(507_INSUFFICIENT_STORAGE);
        CASE(508_LOOP_DETECTED);
        CASE(510_NOT_EXTENDED);
        CASE(511_NETWORK_AUTHENTICATION_REQUIRED);

        default: return STR_STATUS_CODE_HTTP_UNKNOWN;
    }
}


/***********************************************************************
 * 
 *                     4. Http Connection Type
 * 
************************************************************************/

static const char* STR_CONNECTION_TYPE_UNKNOWN = "UNKNOWN";
static const char* STR_CONNECTION_TYPE_CLOSE = "close";
static const char* STR_CONNECTION_TYPE_KEEP_ALIVE = "keep-alive";
static const char* STR_CONNECTION_TYPE_UPGRADE = "upgrade";

#undef  CASE
#define CASE(type) case ConnectionType::type: return STR_CONNECTION_TYPE_##type

const char* to_string(ConnectionType type) {
    switch (type) {
        CASE(CLOSE);
        CASE(KEEP_ALIVE);
        CASE(UPGRADE);
        default: return STR_CONNECTION_TYPE_UNKNOWN;
    }
}

/***********************************************************************
 * 
 *                     5. Http Content Type
 *
************************************************************************/

#if 1 // Http Content Type String
static const char* STR_CONTENT_TYPE_UNKNOWN = "UNKNOWN";
static const char* STR_CONTENT_TYPE_NO_CONTENT = "";
static const char* STR_CONTENT_TYPE_TEXT_CSS = "text/css";
static const char* STR_CONTENT_TYPE_TEXT_CSV = "text/csv";
static const char* STR_CONTENT_TYPE_TEXT_HTML = "text/html";
static const char* STR_CONTENT_TYPE_TEXT_PLAIN = "text/plain";
static const char* STR_CONTENT_TYPE_TEXT_XML = "text/xml";
static const char* STR_CONTENT_TYPE_IMAGE_GIF = "image/gif";
static const char* STR_CONTENT_TYPE_IMAGE_JPEG = "image/jpeg";
static const char* STR_CONTENT_TYPE_IMAGE_PNG = "image/png";
static const char* STR_CONTENT_TYPE_IMAGE_TIFF = "image/tiff";
static const char* STR_CONTENT_TYPE_IMAGE_X_ICON = "image/x-icon";
static const char* STR_CONTENT_TYPE_IMAGE_SVG_XML = "image/svg+xml";
static const char* STR_CONTENT_TYPE_VIDEO_MPEG = "video/mpeg";
static const char* STR_CONTENT_TYPE_VIDEO_MP4 = "video/mp4";
static const char* STR_CONTENT_TYPE_VIDEO_X_FLV = "video/x-flv";
static const char* STR_CONTENT_TYPE_VIDEO_WEBM = "video/webm";
static const char* STR_CONTENT_TYPE_MULTIPART_MIXED = "multipart/mixed";
static const char* STR_CONTENT_TYPE_MULTIPART_ALTERNATIVE = "multipart/alternative";
static const char* STR_CONTENT_TYPE_MULTIPART_RELATED = "multipart/related";
static const char* STR_CONTENT_TYPE_MULTIPART_FORM_DATA = "multipart/form-data";
static const char* STR_CONTENT_TYPE_AUDIO_MPEG = "audio/mpeg";
static const char* STR_CONTENT_TYPE_AUDIO_X_MS_WMA = "audio/x-ms-wma";
static const char* STR_CONTENT_TYPE_AUDIO_X_WAV = "audio/x-wav";
static const char* STR_CONTENT_TYPE_APPLICATION_JAVASCRIPT = "application/javascript";
static const char* STR_CONTENT_TYPE_APPLICATION_OCTET_STREAM = "application/octet-stream";
static const char* STR_CONTENT_TYPE_APPLICATION_OGG = "application/ogg";
static const char* STR_CONTENT_TYPE_APPLICATION_PDF = "application/pdf";
static const char* STR_CONTENT_TYPE_APPLICATION_XHTML_XML = "application/xhtml+xml";
static const char* STR_CONTENT_TYPE_APPLICATION_X_SHOCKWAVE_FLASH = "application/x-shockwave-flash";
static const char* STR_CONTENT_TYPE_APPLICATION_JSON = "application/json";
static const char* STR_CONTENT_TYPE_APPLICATION_LD_JSON = "application/ld+json";
static const char* STR_CONTENT_TYPE_APPLICATION_XML = "application/xml";
static const char* STR_CONTENT_TYPE_APPLICATION_ZIP = "application/zip";
static const char* STR_CONTENT_TYPE_APPLICATION_X_WWW_FORM_URLENCODED = "application/x-www-form-urlencoded";
#endif

#undef  CASE
#define CASE(type) case ContentType::type: return STR_CONTENT_TYPE_##type

const char* to_string(ContentType type) {
    switch (type) {
        CASE(NO_CONTENT);
        CASE(TEXT_CSS);
        CASE(TEXT_CSV);
        CASE(TEXT_HTML);
        CASE(TEXT_PLAIN);
        CASE(TEXT_XML);
        CASE(IMAGE_GIF);
        CASE(IMAGE_JPEG);
        CASE(IMAGE_PNG);
        CASE(IMAGE_TIFF);
        CASE(IMAGE_X_ICON);
        CASE(IMAGE_SVG_XML);
        CASE(VIDEO_MPEG);
        CASE(VIDEO_MP4);
        CASE(VIDEO_X_FLV);
        CASE(VIDEO_WEBM);
        CASE(MULTIPART_MIXED);
        CASE(MULTIPART_ALTERNATIVE);
        CASE(MULTIPART_RELATED);
        CASE(MULTIPART_FORM_DATA);
        CASE(AUDIO_MPEG);
        CASE(AUDIO_X_MS_WMA);
        CASE(AUDIO_X_WAV);
        CASE(APPLICATION_JAVASCRIPT);
        CASE(APPLICATION_OCTET_STREAM);
        CASE(APPLICATION_OGG);
        CASE(APPLICATION_PDF);
        CASE(APPLICATION_XHTML_XML);
        CASE(APPLICATION_X_SHOCKWAVE_FLASH);
        CASE(APPLICATION_JSON);
        CASE(APPLICATION_LD_JSON);
        CASE(APPLICATION_XML);
        CASE(APPLICATION_ZIP);
        CASE(APPLICATION_X_WWW_FORM_URLENCODED);
        default: return STR_CONTENT_TYPE_UNKNOWN;
    }
}

/***********************************************************************
 * 
 *                         END
 *
************************************************************************/

#undef CASE

} // namespace http
} // namespace client
} // namespace ic
