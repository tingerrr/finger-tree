#pragma once

#include <exception>
#include <string>

class UninitException : public std::exception {
  public:
    UninitException(char const* str) : _str(str) {};
    UninitException(std::string str) : _str(str) {};

  public:
    virtual auto what() const noexcept -> char const* override {
      return this->_str.data();
    }

  private:
    std::string _str;
};
