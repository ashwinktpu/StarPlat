#include "debugutils.h"

std::ostream &operator<<(std::ostream &os, Type &t) {
  os << "TYPE:\n";
  os << "\tType ID: " << t.gettypeId() << std::endl;
  os << "\tRoot Type: " << t.getRootType() << std::endl;
  os << "\tIs Prop Type: " << t.isPropType() << std::endl;
  return os;
}
