#include <lartc/constants/constant_cache.hh>
#include <lartc/api/config.hh>
  
std::ostream& ConstantCache::Print(std::ostream& out, ConstantCache& constant_cache) {
  out << "# Constant Cache" << std::endl << std::endl;
  for (auto item : constant_cache.constants) {
    Expression::Print(Declaration::PrintShort(out <<  " - ", item.first) << ": ", item.second) << std::endl;
  }
  return out;
}

void ConstantCache::Delete(ConstantCache& constant_cache) {
  for (auto item : constant_cache.constants) {
    Expression::Delete(item.second);
  }
  constant_cache.constants = {};
}
