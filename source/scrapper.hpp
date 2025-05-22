#pragma once
#include <string>
#include <gumbo.h>
#include <memory>
#include "output.hpp"
#include <sstream>
#include <iomanip>

namespace reload::scrap
{
    class scrapper
    {
        public:
                                    scrapper            ();
            std::string             request             (const std::string& url);
            document                parse               (const std::string& markup);
            bool                    download_to_file    (const std::string& url, const std::string& output_base, const std::string& domain);

            static inline std::string url_decode(const std::string& input) {
                std::string result;
                result.reserve(input.size());

                for (size_t i = 0; i < input.length(); ++i) {
                    if (input[i] == '%' && i + 2 < input.length()) {
                        std::string code = input.substr(i, 3);
                        if (code == "%20") result += ' ';
                        else if (code == "%28") result += '(';
                        else if (code == "%29") result += ')';
                        else if (code == "%2C") result += ',';
                        else if (code == "%21") result += '!';
                        else if (code == "%27") result += '\'';
                        else if (code == "%3A") result += ':';
                        else result += code;  // leave it as-is if unknown
                        i += 2;
                    } else {
                        result += input[i];
                    }
                }

                return result;
            }

            std::string user_agent;
    };
}
