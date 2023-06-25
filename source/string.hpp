#pragma once
#include <iostream>
#include <string>
#include <deque>
#include <sstream>

namespace reload::scrap 
{
    class string
    {
        public:
            static bool in_list(const std::string& str, const std::deque<std::string>& strings) 
            {
                if (strings.size()==0) return true;
                for (const std::string& s : strings) 
                {
                    if (str == s) {
                        return true;
                    }
                }
                return false;
            }

            static std::deque<std::string> split(const std::string& input, const std::string& separators = " ") 
            {
                std::deque<std::string> substrings;
                std::string separatorsCopy = separators; // Create a copy to avoid modifying the original string
                std::istringstream iss(input);

                char separator;
                while (iss.get(separator)) {
                    if (separatorsCopy.find(separator) != std::string::npos) {
                        continue;
                    }

                    std::string token(1, separator);
                    while (iss.get(separator) && separatorsCopy.find(separator) == std::string::npos) {
                        token += separator;
                    }

                    if (!token.empty()) {
                        substrings.push_back(token);
                    }
                }

                return substrings;
            }

    };
}