#include "iclient/iclient.h"
#include <iostream>

ic::client::Request request("");

void test() {


    request.set_max_redirects(3);   // 最多重定向3次
    request.set_max_redirects(0);   // 设为0，表示禁止重定向
    request.set_max_redirects(-1);  // 设为-1，表示不限制重定向次数（默认）
request.set_verify_ssl_host(false);
request.set_verify_ssl_peer(false);
request.set_verify_ssl_status(false);
request.set_verify_ssl(false); // 等价于上面三条语句
// @downloadFile: 下载的本地文件名
// @resume(bool): 是否启用断点续传，默认不启用
request.set_download_file("output.jpg"/*, false*/);

// @range: 单位bytes
// 包括0和4095
request.set_download_range(0, 4095);


request.set_transfer_progress_handler(nullptr);

}

