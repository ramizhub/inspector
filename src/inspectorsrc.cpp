#include "inspectorlib.h"

size_t write_callback(char* buf, size_t size, size_t nmemb, void* userdata) {
    return size * nmemb;
}

void print_response_info(const long& status_code, const double& conn_time, const double& resp_time) {
    std::cout << "Status Code: "     << status_code << std::endl;
    std::cout << "Connection Time: " << conn_time   << std::endl;
    std::cout << "Response Time: "   << resp_time   << std::endl;
}

void curl_options_set(CURL *curl, const std::string& url) { 
    curl_easy_setopt(curl, CURLOPT_URL,               url.c_str());
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, CONNECTTIMEOUT_MS); 
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS,        TIMEOUT_MS); 
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,     write_callback);
}

void curl_get_info(CURL* curl, long* status_code, double* conn_time, double* resp_time) {
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, status_code);
    curl_easy_getinfo(curl, CURLINFO_CONNECT_TIME,  conn_time);
    curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME,    resp_time);
}

void convert_to_time_msec(double& conn_time, double& resp_time) {
    conn_time *= 1000;
    resp_time *= 1000;
}

void send_request_fill_db(const std::string& url, influxdb_cpp::server_info &si) {
    CURL*       curl;
    CURLcode    res;
    std::string respstr;
    long        status_code = 0;
    double      conn_time   = 0;
    double      resp_time   = 0;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if(!curl) 
        std::cerr << "curl_easy_init() failed." << std::endl;
    
    curl_options_set(curl, url);
    while(SERVICE_WORKS) {
        res = curl_easy_perform(curl);
        if(res == CURLE_OK) {
            curl_get_info(curl, &status_code, &conn_time, &resp_time);
            convert_to_time_msec(conn_time, resp_time);
 	    
	    influxdb_cpp::builder()
                .meas("http_response")
                .field("response_time", static_cast<int>(resp_time))
                .field("status_code", status_code)
		.field("connection_time", conn_time)
                .post_http(si, &respstr);                   
            
	    print_response_info(status_code, conn_time, resp_time);
	    std::cout << respstr << std::endl;
        } else
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        
        std::this_thread::sleep_for(std::chrono::seconds(120));
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();
}


