#ifndef INSPECTORLIB_H
#define INSPECTORLIB_H

#include <iostream>
#include <thread>
#include <string>
#include <curl/curl.h>
#include <cmath>
#include "./influxdb.hpp"

#define SERVICE_WORKS           1
#define TIMEOUT_MS              8000L
#define CONNECTTIMEOUT_MS       5000L

size_t write_callback(char* buf, size_t size, size_t nmemb, void* userdata);
void   print_response_info(const long& status_code, const double& conn_time, const double& resp_time);
void   curl_options_set(CURL *curl, const std::string& url);
void   curl_get_info(CURL* curl, long* status_code, double* conn_time, double* resp_time);
void   convert_to_time_msec(double& conn_time, double& resp_time);
void   send_request_fill_db(const std::string& url, influxdb_cpp::server_info &si);

#endif


