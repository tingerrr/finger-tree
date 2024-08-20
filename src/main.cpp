#include "btree/node.hpp"

#include <iostream>
#include <memory>
#include <type_traits>
#include <variant>

int main() {
  auto visitor = [](auto&& arg){
    using T = std::decay_t<decltype(arg)>;
    if constexpr (std::is_same_v<T, btree::Inserted<int, int, 3, 2>>) {
      arg->show();
      return arg;
    } else if constexpr (std::is_same_v<T, btree::Split<int, int, 3, 2>>) {
      std::cout << "left" << std::endl;
      std::get<0>(arg)->show();
      std::cout << "k: " << std::get<1>(arg) << std::endl;
      std::cout << "right" << std::endl;
      std::get<2>(arg)->show();
      return std::get<0>(arg);
    } else {
      static_assert(false, "non-exhaustive visitor");
    }
  };

  auto n = std::static_pointer_cast<btree::Node<int, int, 3>>(
    std::make_shared<btree::Leaf<int, int, 3>>()
  );
  n->show();

  std::cout << "1 -> 1" << std::endl;
  n = std::visit(visitor, n->insert(1, 1));

  std::cout << "2 -> 2" << std::endl;
  n = std::visit(visitor, n->insert(2, 2));

  std::cout << "3 -> 3" << std::endl;
  n = std::visit(visitor, n->insert(3, 3));

  std::cout << "2 -> 42" << std::endl;
  n = std::visit(visitor, n->insert(2, 42));

  std::cout << "4 -> 1" << std::endl;
  n = std::visit(visitor, n->insert(4, 1));

  return 0;
}
