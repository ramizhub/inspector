#include <iostream>
#include <chrono>
#include <curl/curl.h>
#include <thread>
#include "./influxdb.hpp"

using namespace std;

// Callback function to discard the received data
size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    return size * nmemb;
}

void print_response_info(long response_code, chrono::milliseconds response_time) {
    cout << "Response status code: " << response_code << endl;
    cout << "Response time: " << response_time.count() << " ms" << endl;
}

void print_request_err(CURLcode res)
{
    cerr << "Request failed: " << curl_easy_strerror(res) << endl;
}

// Perform the HTTP request and store response info in InfluxDB
void send_request_fill_db(CURL *curl, influxdb_cpp::server_info &si)
{
    int ret;
    string resp;
    long response_code;

    while (true) {
        auto start = chrono::high_resolution_clock::now();
        CURLcode res = curl_easy_perform(curl);
        auto end = chrono::high_resolution_clock::now();
        auto response_time = chrono::duration_cast<chrono::milliseconds>(end - start);

        if (res == CURLE_OK) {
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            print_response_info(response_code, response_time);
            ret = influxdb_cpp::builder()
                .meas("http_response")
                .field("response_time", response_time.count())
                .field("status_code", response_code)
                .post_http(si, &resp);
        } else
            print_request_err(res);
        
        std::this_thread::sleep_for(std::chrono::seconds(120));
    }
}

int main()
{
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();
    if (!curl) {
        cerr << "Failed to create curl handle." << endl;
        return 1;
    }

    curl_easy_setopt(curl, CURLOPT_URL, "https://www.sumnotes.net/");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

    influxdb_cpp::server_info si("127.0.0.1", 8086, "inspector_data", "", "");

    send_request_fill_db(curl, si);

    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return 0;
}
