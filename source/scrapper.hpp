#pragma once
#include <string>
#include <gumbo.h>
#include <memory>
#include "output.hpp"

namespace reload::scrap
{
    class scrapper
    {
        public:
                                    scrapper            ();
            std::string             request             (const std::string& url);
            document                parse               (const std::string& markup);
            bool                    download_to_file    (const std::string& url, const std::string& output_base, const std::string& domain);
            std::string user_agent;
    };
}
