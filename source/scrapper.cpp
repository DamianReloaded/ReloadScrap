#include "scrapper.hpp"
#include <curl/curl.h>
#include <algorithm>
#include <cstring>
#include <iostream>

namespace reload::scrap
{
    typedef size_t( * curl_write)(char * , size_t, size_t, std::string * );

    scrapper::scrapper ()
    {
        user_agent = "Mozilla/5.0";
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
                            static_cast < curl_write >  (
                                                            []( char * contents, size_t size, size_t nmemb, std::string * data) -> size_t 
                                                            {
                                                            size_t new_size = size * nmemb;
                                                            if (data == NULL) {
                                                                return 0;
                                                            }
                                                            data -> append(contents, new_size);
                                                            return new_size;
                                                            }
                                                        )
                            );
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, & result);
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_USERAGENT, user_agent);

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