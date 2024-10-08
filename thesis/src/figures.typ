#import "figures/util.typ": *

#let (
  list,
  t4gl,
  vector,
  tree,
  b-tree,
  finger-tree,
  srb-tree,
) = {
  import "figures/algorithms.typ": *
  import "figures/figures.typ": *
  import "figures/listings.typ": *
  import "figures/math.typ": *
  import "figures/tables.typ": *

  (
    (
      new: list-new,
      copy: list-copy,
      pop: list-pop,
      push: list-push,
    ),
    (
      ex: (
        array1: t4gl-ex-array1,
        array2: t4gl-ex-array2,
      ),
      layers: (
        new: t4gl-layers-new,
        shallow: t4gl-layers-shallow,
        deep-new: t4gl-layers-deep-new,
        deep-mut: t4gl-layers-deep-mut,
      ),
      analogies: t4gl-analogies,
    ),
    (
      repr: vector-repr,
      example: vector-ex,
    ),
    (
      new: tree-new,
      shared: tree-shared,
    ),
    (
      node: b-tree-node,
    ),
    (
      repr: finger-tree,
      ranges: finger-tree-ranges,
      def: (
        old: finger-tree-def-old,
        new: finger-tree-def-new,
        illegal: finger-tree-def-illegal,

        node: finger-tree-def-node,
        digits: finger-tree-def-digits,
        self: finger-tree-def-self,
      ),
      alg: (
        search: finger-tree-alg-search,

        pushl: finger-tree-alg-push-left,
        popl: finger-tree-alg-pop-left,

        appendl: finger-tree-alg-append-left,
        takel: finger-tree-alg-take-left,

        nodes: finger-tree-alg-nodes,
        concat: finger-tree-alg-concat,
        split: finger-tree-alg-split,

        insert: finger-tree-alg-insert,
        remove: finger-tree-alg-remove,
      ),
    ),
    srb-tree,
  )
}
