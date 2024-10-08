#import "/src/util.typ": *
#import "util.typ": *

#let t4gl-analogies = table(columns: 2, align: left,
  table.header(
    i18n(de: [Signatur], en: [signature]),
    i18n(de: [C++ Analogie], en: [C++ analogy])
  ),
  `T[N] name`, `std::array<T, N>`,
  `T[U] name`, `std::map<U, T> name`,
  `T[U, N] name`, `std::map<U, std::array<T, N>> name`,
  `T[N, U] name`, `std::array<std::map<U, T>, N> name`,
  align(center)[...], align(center)[...],
)
