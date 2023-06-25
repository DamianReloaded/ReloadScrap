#pragma once
#include <string>
#include <gumbo.h>
#include <memory>
#include <deque>

namespace reload::scrap
{
    class node
    {
        public:

            static std::deque<GumboElement*>    find                (GumboNode* node, const std::string& names);
            static void                         find                (GumboNode* node, const std::deque<std::string>& names, std::deque<GumboElement*>& v);
            static GumboAttribute*              get_attribute       (GumboElement* element, const std::string& name);
            static bool                         is_element          (GumboNode* node);
            static std::string                  get_name            (GumboElement* element);
            static std::string                  get_attribute_value (GumboElement* element, const std::string& name);
            static std::string                  get_inner_text      (GumboElement* element);
    };

    class document
    {
        public:
                            document            (const std::string& markup);
            virtual         ~document           ();
            
            GumboNode*      root;
            GumboOutput*    output;
        private:
            
            document () {}            
    };
}