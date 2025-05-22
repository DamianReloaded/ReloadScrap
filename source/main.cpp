#include "scrapper.hpp"
#include <iostream>
#include <future>
#include <vector>

std::vector<std::string> ignore = {"GPT-J", "CogView", "STAGING", "training_data"};

bool has_extension(const std::string& filename, const std::string& extension)
{
    return filename.size() >= 4 && filename.compare(filename.size() - 4, 4, std::string(".") + extension) == 0;
}

bool must_be_ignored (const std::string& url)
{
  for (auto& item : ignore)
  {
    if (url.contains(item)) return true;
  }
  return false;
}

void visit(const std::string& url, const std::string& domain, const std::string& download_path)
{
    reload::scrap::scrapper s;

    std::cout << "Visiting: " << url << "\n";

    std::string html = s.request(url);
    reload::scrap::document doc = s.parse(html);

    auto v = reload::scrap::node::find(doc.root, "a");
    for (auto e : v)
    {
        std::string href = reload::scrap::node::get_attribute_value(e, "href");
        auto decoded_href = reload::scrap::scrapper::url_decode(href);
        if (href.empty())
            continue;

        if (href.ends_with("./") || href.ends_with("../"))
            continue;

        if (must_be_ignored(href)) continue;

        if (has_extension(href, "pdf"))
        {
            std::string full_url = (href.starts_with("http://") || href.starts_with("https://"))
                ? href
                : url + "/" + href;

            if (full_url.starts_with(domain))
            {
                bool success = s.download_to_file(full_url, download_path, domain);
                if (success)
                    std::cout << "File downloaded: " << decoded_href << "\n";
                else
                    std::cout << "Error downloading file: " << decoded_href << "\n";
            }
        }
        else
        {
            if (!href.ends_with("/"))
                continue;

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

            if (next_url.starts_with(domain)) {
                std::cout << "Found link: " << decoded_href << "\n";
                std::async(std::launch::async, visit, next_url, domain, download_path);
            }
        }
    }
}

int main(int argc, char** argv)
{
    // Launch the top-level crawl
    std::future<void> root_future = std::async(std::launch::async,
        visit,
        "https://the-eye.eu/public/",
        "https://the-eye.eu",
        "/home/shared/Documents/Scrapped");

    // Optionally wait for the root crawl to finish
    root_future.get();

    return 0;
}
