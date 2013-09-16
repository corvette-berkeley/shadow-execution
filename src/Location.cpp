#include "Location.h"


void Location::setType(KIND t) {
  type = t;
}

void Location::setValue(VALUE v) {
  value = v;
}

void Location::setLocal(bool l) {
  local = l;
}

KIND Location::getType() {
  return type;
}

VALUE Location::getValue() {
  return value;
}

bool Location::getLocal() {
  return local;
}

