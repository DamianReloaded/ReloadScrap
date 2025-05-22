#include "scrapper.hpp"
#include <curl/curl.h>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace reload::scrap
{
    typedef size_t( * curl_write)(char * , size_t, size_t, std::string * );

    scrapper::scrapper ()
    {
        user_agent = "Mozilla/5.0";
    }

bool scrapper::download_to_file(const std::string& url, const std::string& output_base, const std::string& domain)
{
    if (url.find(domain) != 0) {
        std::cerr << "URL does not start with the provided domain.\n";
        return false;
    }

    // Extract relative path from URL
    std::string relative_path = url.substr(domain.length());
    if (!relative_path.empty() && relative_path[0] == '/')
        relative_path.erase(0, 1);

    std::filesystem::path full_output_path = std::filesystem::path(output_base) / relative_path;

    // Create necessary parent directories
    std::filesystem::create_directories(full_output_path.parent_path());

    // Open the output file in binary mode
    std::ofstream output_file(full_output_path, std::ios::binary);
    if (!output_file.is_open()) {
        std::cerr << "Failed to open output file: " << full_output_path << "\n";
        return false;
    }

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_ALL);
    CURL* curl = curl_easy_init();

    if (!curl) {
        std::cerr << "Failed to initialize curl.\n";
        output_file.close();
        curl_global_cleanup();
        return false;
    }

    // Configure curl to write directly to the file stream
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 10L);

    // Write callback to write raw binary data directly into the file
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
        +[](char* ptr, size_t size, size_t nmemb, void* userdata) -> size_t {
            std::ofstream* stream = static_cast<std::ofstream*>(userdata);
            size_t total_size = size * nmemb;
            if (!stream || !stream->is_open()) return 0;
            stream->write(ptr, total_size);
            return stream->good() ? total_size : 0;
        });

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &output_file);

    // Perform the request
    CURLcode res = curl_easy_perform(curl);

    // Clean up
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    output_file.close();

    if (res != CURLE_OK) {
        std::cerr << "Download failed: " << curl_easy_strerror(res) << "\n";
        return false;
    }

    return true;
}

    document scrapper::parse(const std::string& markup)
    {
        return document(markup);
    }

    std::string scrapper::request(const std::string& url)
    {
        CURLcode res_code = CURLE_FAILED_INIT;
        CURL * curl = curl_easy_init();
        std::string result;
        curl_global_init(CURL_GLOBAL_ALL);

        if (curl) 
        {
            curl_easy_setopt(
                curl,
                CURLOPT_WRITEFUNCTION,
                static_cast<curl_write>(
                    [](char * contents, size_t size, size_t nmemb, std::string * data) -> size_t 
                    {
                        size_t new_size = size * nmemb;
                        if (data == nullptr) {
                            return 0;
                        }
                        data->append(contents, new_size);
                        return new_size;
                    }
                )
            );

            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_USERAGENT, user_agent);

            // ✅ Follow redirects
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

            // (Optional) Set a reasonable redirect limit
            curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 10L);

            res_code = curl_easy_perform(curl);

            if (res_code != CURLE_OK) 
            {
                return curl_easy_strerror(res_code);
            }

            curl_easy_cleanup(curl);
        }

        curl_global_cleanup();

        return result;
    }
}
