#include "scrapper.hpp"
#include <iostream>

bool has_extension(const std::string& filename, const std::string& extension) 
{
    return filename.size() >= 4 && filename.compare(filename.size() - 4, 4, std::string(".") + extension ) == 0;
}

void visit(const std::string& url, const std::string& domain, const std::string& download_path)
{
    reload::scrap::scrapper s;
  
    std::cout << "Visiting: " << url << "\n";

    std::string html = s.request(url);
    reload::scrap::document doc = s.parse(html);

    //std::cout << "HTML: " << html << "\n";

    auto v = reload::scrap::node::find(doc.root, "a");
    for (auto e : v)
    {
        std::string href = reload::scrap::node::get_attribute_value(e, "href");
        if (href.empty())
            continue;

          // Ignore "./" and "../" links
          if (href.ends_with("./") || href.ends_with("../"))
              continue;

        std::cout << "Found link: " << href << "\n";

        // Handle PDF download
        if (has_extension(href, "pdf"))
        {
            std::string full_url = (href.starts_with("http://") || href.starts_with("https://")) 
                                    ? href 
                                    : url + "/" + href;

            // Only download files within domain
            if (full_url.starts_with(domain))
            {
                bool success = s.download_to_file(full_url, download_path, domain);
                if (success)
                    std::cout << "File downloaded: " << href << "\n";
                else
                    std::cout << "Error downloading file: " << href << "\n";
            }
        }
        else
        {
            // Only visit directory links
            if (!href.ends_with("/"))
                continue;
                          
            // Resolve full URL
            std::string next_url = href;
            if (!(href.starts_with("http://") || href.starts_with("https://")))
            {
                std::string base = url;
                if (!base.empty() && base.back() == '/')
                    base.pop_back();
                if (!href.empty() && href.front() == '/')
                    href.erase(0, 1);

                next_url = base + "/" + href;
            }

            // Only visit links within the domain
            if (next_url.starts_with(domain))
            {
                visit(next_url, domain, download_path);
            }
        }
    }
}



int main(int argc, char** argv)
{
  visit("https://the-eye.eu/public/",
        "https://the-eye.eu",
        "/home/shared/Documents/Scrapped"
    );

	return 0;
}
