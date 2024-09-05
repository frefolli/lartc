#include <lartc/typecheck/type_cache.hh>

std::ostream& TypeCache::Print(std::ostream& out, TypeCache& type_cache) {
  out << "# Type Cache" << std::endl << std::endl;
  out << "## Expression" << std::endl << std::endl; 
  for (auto item : type_cache.expression_types) {
    Type::Print(Expression::Print(out << " - ", item.first) << " => ", item.second) << std::endl;
  }
  return out;
}
