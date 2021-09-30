#pragma once

#include <cstdlib>
#include <string>
#include <ostream>
#include <stack>

namespace whirl {

class JsonWriter {
  enum class Container {
    Object = 0,
    Map = 1,
    List = 2
  };

  struct State {
    Container container;
    size_t index;
  };

 public:
  explicit JsonWriter(std::ostream& out) : out_(out) {
  }

  void OpenList() {
    BeforeValue();
    out_ << "[";

    PushState();
    container_ = Container::List;
    index_ = 0;
  }

  void CloseList() {
    Complete();
    PopState();
    Ident();
    out_ << "]";
  }

  void OpenMap() {
    BeforeValue();
    out_ << "{";

    PushState();
    container_ = Container::Map;
    index_ = 0;
  }

  void CloseMap() {
    Complete();
    PopState();
    Ident();
    out_ << "}";
  }

  void WriteField(std::string_view name) {
    BeforeField();
    out_ << "\"" << name << "\": ";
  }

  void WriteString(const std::string& str) {
    BeforeValue();
    out_ << "\"" << str << "\"";
  }

  void WriteInteger(int64_t value) {
    BeforeValue();
    out_ << value;
  }

 private:
  void Ident() {
    if (Depth() > 0) {
      out_ << std::string(Depth(), ' ');
    }
  }

  void BeforeValue() {
    if (container_ == Container::List) {
      NewEntry();
    }
  }

  void BeforeField() {
    NewEntry();
  }

  void NewEntry() {
    ++index_;
    if (index_ > 1) {
      out_ << ",";
    }
    out_ << std::endl;
    Ident();
  }

  void Complete() {
    if (index_ > 1) {
      // At least one entry
      out_ << std::endl;
    }
  }

 private:
  void PushState() {
    stack_.push({container_, index_});
  }

  void PopState() {
    State s = stack_.top();
    stack_.pop();
    container_ = s.container;
    index_ = s.index;
  }

  size_t Depth() const {
    return stack_.size();
  }

 private:
  std::ostream& out_;

  // Current state
  Container container_ = Container::Object;
  size_t index_ = 0;

  std::stack<State> stack_;
};

}  // namespace whirl
