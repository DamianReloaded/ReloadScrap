#include "output.hpp"
#include "string.hpp"

namespace reload::scrap
{
    std::string node::get_name (GumboElement* element)
    {
        return gumbo_normalized_tagname(element->tag);
    }

    bool node::is_element (GumboNode* node)
    {
        return (node->type == GUMBO_NODE_ELEMENT);
    }        

    std::string node::get_attribute_value (GumboElement* element, const std::string& name)
    {
        return std::string(get_attribute(element,name)->value);
    }

    std::string node::get_inner_text(GumboElement* element) 
    {
        std::string innerText;
        GumboVector *children = &element->children;
        for (size_t i = 0; i < children->length; ++i)
        {
            GumboNode* node = static_cast<GumboNode *>(children->data[i]);
            if (node->type == GUMBO_NODE_TEXT) 
            {
                innerText += node->v.text.text;
            } 
            else if (node->type == GUMBO_NODE_ELEMENT) 
            {
                innerText += get_inner_text(&node->v.element);
            }
        }
        return innerText;
    }

    std::deque<GumboElement*> node::find(GumboNode* node, const std::string& names)
    {
        std::deque<GumboElement*> v;
        node::find(node,string::split(names),v);
        return v;
    }

    void node::find (GumboNode* node, const std::deque<std::string>& names, std::deque<GumboElement*>& v)
    {
        if (!is_element(node)) return;
        GumboElement* element = &node->v.element;
        GumboVector *children = &element->children;
        
        std::string node_name = get_name(element);
        if (string::in_list(node_name, names)) v.push_back(element);

        for (size_t i = 0; i < children->length; ++i)
        {
            GumboNode* n = static_cast<GumboNode *>(children->data[i]);
            find(n,names,v);
        }
    }

     GumboAttribute* node::get_attribute(GumboElement* element, const std::string& name)
     {
        return gumbo_get_attribute(&element->attributes, name.c_str());
     }

    document::document(const std::string& markup)
    {
        output = gumbo_parse_with_options(&kGumboDefaultOptions, markup.data(), markup.length());
        root = output->root;
    }

    document::~document()
    {
        gumbo_destroy_output(&kGumboDefaultOptions, output);
    } 
}