#include <lartc/typecheck/size_cache.hh>
  
std::ostream& SizeCache::Print(std::ostream& out, SizeCache& size_cache) {
  out << "# Size Cache" << std::endl << std::endl;
  for (auto item : size_cache.sizes) {
    Declaration::PrintShort(out <<  " - ", item.first) << ": " << item.second << std::endl;
  }
  return out;
}
