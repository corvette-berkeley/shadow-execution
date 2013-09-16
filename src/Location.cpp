#include "Location.h"


void Location::setType(int t) {
  type = t;
}

void Location::setValue(long double v) {
  value = v;
}

void Location::setLocal(bool l) {
  local = l;
}

int Location::getType() {
  return type;
}

long double Location::getValue() {
  return value;
}

bool Location::getLocal() {
  return local;
}

