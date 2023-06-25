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
                                    scrapper    ();
            std::string             request     (const std::string& url);
            document                parse       (const std::string& markup);

            std::string user_agent;
    };
}