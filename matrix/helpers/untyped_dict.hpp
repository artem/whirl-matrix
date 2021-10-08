#pragma once

#include <any>
#include <string>
#include <map>

namespace whirl {

class UntypedDict {
 public:
  void Set(const std::string& key, std::any value) {
    entries_.insert_or_assign(key, std::move(value));
  }

  std::any Get(const std::string& key) const {
    if (Has(key)) {
      return entries_.at(key);
    } else {
      return {};
    }
  }

  bool Has(const std::string& key) const {
    return entries_.find(key) != entries_.end();
  }

 private:
  std::map<std::string, std::any> entries_;
};

}  // namespace whirl
