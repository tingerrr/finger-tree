#pragma once

#include <exception>
#include <string>

class VariantException : public std::exception {
  public:
    VariantException(char const* str) : _str(str) {};
    VariantException(std::string str) : _str(str) {};

  public:
    virtual auto what() const noexcept -> char const* override {
      return this->_str.data();
    }

  private:
    std::string _str;
};
