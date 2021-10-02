#pragma once

#include <ostream>
#include <cassert>

namespace whirl {

// For rapidjson

class OStreamAdapter {
 public:
  using Ch = char;

  OStreamAdapter(std::ostream& out) : out_(out) {}

  Ch Peek() const {
    assert(false);
    return '\0';
  }

  Ch Take() {
    assert(false);
    return '\0';
  }

  size_t Tell() const {
    return 0;
  }

  Ch* PutBegin() {
    assert(false);
    return nullptr;
  }

  void Put(Ch c) {
    out_.put(c);
  }

  void Flush() {
    out_.flush();
  }

  size_t PutEnd(Ch*) {
    assert(false);
    return 0;
  }

 private:
  std::ostream& out_;
};

}  // namespace whirl
