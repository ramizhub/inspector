#include <iostream>
#include <chrono>
#include <curl/curl.h>
using namespace std;

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    // Discard the received data
    return size * nmemb;
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

    auto start = chrono::high_resolution_clock::now();
    CURLcode res = curl_easy_perform(curl);
    auto end = chrono::high_resolution_clock::now();
    auto response_time = chrono::duration_cast<chrono::milliseconds>(end - start);

    if (res == CURLE_OK) {
        long response_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        cout << "Response status code: " << response_code << endl;
        cout << "Response time: " << response_time.count() << " ms" << endl;
    } else {
        cerr << "Request failed: " << curl_easy_strerror(res) << endl;
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return 0;
}

