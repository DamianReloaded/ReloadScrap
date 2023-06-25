# ReloadScrap
 A wrapper for curl and gumbo for scraping web pages

Usage:
```
  reload::scrap::scrapper s;
  std::string html = s.request("https://www.merriam-webster.com/dictionary/esoteric");
  reload::scrap::document doc = s.parse(html);
  auto v = reload::scrap::node::find(doc.root,"p");
  for (auto e : v)
  {
    std::cout << reload::scrap::node::get_attribute_value(e, "href") <<"\n";
    std::cout << reload::scrap::node::get_inner_text(e) <<"\n";
  }

```
