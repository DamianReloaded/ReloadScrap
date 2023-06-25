#include "scrapper.hpp"
#include <iostream>

int main(int argc, char** argv)
{
  reload::scrap::scrapper s;
  std::string res = s.request("https://www.merriam-webster.com/dictionary/esoteric");
  
  reload::scrap::document doc = s.parse(res);

  auto v = reload::scrap::node::find(doc.root,"p");

  for (auto e : v)
  {
    //std::cout << reload::scrap::node::get_attribute_value(e, "href") <<"\n";
    std::cout << reload::scrap::node::get_inner_text(e) <<"\n";
  }

	return 0;
}
