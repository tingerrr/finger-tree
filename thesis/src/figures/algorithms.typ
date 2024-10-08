#import "util.typ": *

#let comment(body) = text(gray, {
  h(1fr)
  sym.triangle.r
  [ ]
  body
})

#let dd = $Delta d$

#let Int = math-type("Int")
#let None = math-type("None")

#let Node = math-type("Node")
#let Deep = math-type("Deep")
#let Key = math-type("Key")
#let Shallow = math-type("Shallow")
#let FingerTree = math-type("FingerTree")

#let popl = math-func("pop-left")
#let pushl = math-func("push-left")
#let popr = math-func("pop-right")
#let pushr = math-func("push-right")
#let concat = math-func("concat")
#let split = math-func("split")
#let last = math-func("last")
#let rev = math-func("rev")

#let btsearch = math-func("btree-search")
#let btsplit = math-func("btree-split")

#let dsearch = math-func("digit-search")
#let dsplit = math-func("digit-split")

#let pnodes = math-func("pack-nodes")

#let ftsearch = math-func("ftree-search")

#let ftpushl = math-func("ftree-push-left")
#let ftpushr = math-func("ftree-push-right")
#let ftpopl = math-func("ftree-pop-left")
#let ftpopr = math-func("ftree-pop-right")

#let ftappendl = math-func("ftree-append-left")
#let fttakel = math-func("ftree-take-left")
#let ftappendr = math-func("ftree-append-right")
#let fttaker = math-func("ftree-take-right")

#let ftsplit = math-func("ftree-split")
#let ftconcat = math-func("ftree-concat")

#let ftinsert = math-func("ftree-insert")
#let ftremove = math-func("ftree-remove")

#let finger-tree-alg-search = algorithm(
  numbered-title: $ftsearch(t, m): (FingerTree, Key) -> Node$,
)[
  + *if* $t$ *is* $Shallow$
    + *if* $m <= t."key"$
      + *return* $dsearch(t."digits", m)$
    + *else*
      #comment[key not in this subtree]
      + *return* $None$
  + *else*
    #comment[find suitable branch to descend]
    + *if* $m <= t."left"."key"$
      + *return* $dsearch(t."left", m)$
    + *else if* $m <= t."middle"."key"$
      + *return* $ftsearch(t."middle", m)$
    + *else if* $m <= t."right"."key"$
      + *return* $dsearch(t."right", m)$
    + *else*
      #comment[key not in this subtree]
      + *return* $None$
]

#let finger-tree-alg-push-left = algorithm(
  numbered-title: $ftpushl(t, e): (FingerTree, Node) -> FingerTree$,
)[
  + *if* $t$ *is* $Shallow$
      #comment[overflow by split into]
    + *let* $"digits" = pushl(t."digits", e)$
    + *if* $abs("digits") <= 2d_min$
      #comment[no split]
      + *return* $Shallow("digits")$
    + *else*
      + *let* $"left", "right" := split("digits"."children", d_min)$
    + *return* $Deep("left", None, "right")$
  + *else*
    + *let* $"left" := pushl(t."left"."children", e)$
    + *if* $abs("left") <= d_max$
      #comment[no overflow]
      + *return* $Deep("left", t."middle", t."right")$
    + *else*
      + *let* $"rest", "overflow" := split("left", abs("left") - dd)$
        #comment[overflow by descent]
      + *let* $"middle" := ftpushl(t."middle", Node("overflow"))$
      + *return* $Deep("rest", "middle", t."right")$
]

#let finger-tree-alg-pop-left = algorithm(
  numbered-title: $ftpopl(t): FingerTree -> (Node, FingerTree)$,
)[
  + *if* $t$ *is* $Shallow$
    + *if* $abs(t."digits") = 0$
      + *return* $(None, t)$
    + *else*
      + *let* $e, "rest" := popl(t."digits")$
      + *return* $(e, Shallow("rest"))$
  + *else*
    + *let* $e, "rest"_l := popl(t."left"."digits")$
    + *if* $abs("rest"_l) >= d_min$
      #comment[no underflow]
      + *return* $(e, Deep("rest"_l, t."middle", t."right"))$
    + *else if* $abs(t."middle") = 0$
      #comment[underflow by left+right merge]
      + *let* $"middle" = concat(t."left", t."right")$
      + *if* $abs("middle") >= 2d_min$
        + *let* $"left", "right" = split("middle", floor(abs("middle") \/ 2))$
        + *return* $(e, Deep("left", Shallow(None), "right"))$
      + *else*
        + *return* $(e, Shallow("middle"))$
    + *else*
      #comment[underflow by descent]
      + *let* $"node", "rest"_m := ftpopl(t."middle")$
      + *return* $(e, Deep(concat("rest"_l, "node"."children"), "rest"_m, t."right"))$
]

#let finger-tree-alg-append-left = algorithm(
  numbered-title: $ftappendl(t, "nodes"): (FingerTree, [Node]) -> FingerTree$,
)[
  + *for* $e$ *in* $rev("nodes")$
    #comment[simply push all values one by one]
    + $t = ftpushl(t, e)$

  + *return* $t$
]

#let finger-tree-alg-take-left = algorithm(
  numbered-title: $fttakel(t, "count"): (FingerTree, Int) -> ([Node], FingerTree)$,
)[
  + *let* $n' = nothing$
  + *for* $"__"$ *in* $1.."count"$
    + *if* $abs(t) = 0$
      #comment[no more values left]
      + *break*
    + *else*
      + *let* $e, t' = ftpopl(t)$
      + $t = t'$
      + $n' = pushr(n', e)$

  + *return* $t$
]

#let finger-tree-alg-nodes = algorithm(
  numbered-title: $pnodes(n) : [Node] -> [Node]$,
)[
  + *if* $abs(n) < k_min$
    #comment[can't form a single node]
    + *return* $None$
  + *else*
    + *let* $n' := nothing$
    + *while* $abs(n) - k_max >= k_min$
      #comment[push max size nodes as long as it goes]
      + *let* $"children", "rest" = split(n, k_max)$
      + $n = "rest"$
      + $n' = pushr(n', Node("children"))$
    + *if* $abs(n) - k_min >= k_min$
      #comment[push the remaining 1 or 2 nodes]
      + *let* $a, b = split(n, k_min)$
      + $n' = pushr(n', Node(a))$
      + $n' = pushr(n', Node(b))$
    + *else*
      + $n' = pushr(n', Node(n))$
    + *return* $n'$
]

#let finger-tree-alg-concat = algorithm(
  numbered-title: $ftconcat(l, m, r): (FingerTree, [Node], FingerTree) -> FingerTree$,
)[
  + *if* $l$ *is* $Shallow$
    + *return* $ftappendl(r, pushl(m, l."digits"))$
  + *else if* $r$ *is* $Shallow$
    + *return* $ftappendr(l, pushr(m, r."digits"))$
  + *else*
    + *let* $m' := pnodes(concat(l."right", m, r."left"))$
      #comment[pack nodes for the next layer]
    + *return* $Deep(l."left", ftconcat(l."middle", m', r."middle"), r."right")$
]

#let finger-tree-alg-split = algorithm(
  numbered-title: $ftsplit(t, k): (FingerTree, Key) -> (FingerTree, Node, FingerTree)$,
)[
  + *if* $t$ *is* $Shallow$
    + *let* $l, v, r := dsplit(t."digits", k)$
    + *return* $(Shallow(l), v, Shallow(r))$
  + *else*
    + *if* $k <= t."left"."key"$
      + *let* $l, v, r := dsplit(t."left"."children", k)$
      + *return* $(Shallow(l), v, Deep(r, t."middle", t."right"))$
    + *else if* $k <= t."middle"."key"$
      + *let* $l, v, r := ftsplit(t."middle", k)$
        #comment[descend and unpack]
      + *let* $l', v', r' := dsplit(v."children", k)$
      + *return* $(Deep(t."left", l, l'), v', Deep(r', r, t."right"))$
    + *else if* $k <= t."right"."key"$
      + *let* $l, v, r := dsplit(t."right"."children", k)$
      + *return* $(Deep(t."left", t."middle", l), v, Shallow(r))$
    + *else*
      + *return* $(t, Shallow(nothing))$
]

#let finger-tree-alg-insert = algorithm(
  numbered-title: $ftinsert(t, e): (FingerTree, Node) -> FingerTree$,
)[
  + *let* $l, v, r := ftsplit(t, e."key")$
  + *if* $v$ *is* $None$
    #comment[$e."key"$ not in $t$]
    + *let* $l' := ftpushr(l, e)$
    + *return* $ftconcat(l', r)$
  + *else*
    #comment[$k$ in $t$]
    + *let* $l' := ftpushr(l, v)$
    + *let* $l'' := ftpushr(l', e)$
    + *return* $ftconcat(l'', r)$
]

#let finger-tree-alg-remove = algorithm(
  numbered-title: $ftremove(t, k): (FingerTree, Key) -> (FingerTree, Node)$,
)[
  + *let* $l, v, r := ftsplit(t, k)$
  + *if* $v$ *is* $None$
    #comment[$k$ not in $t$]
    + *return* $(t, None)$
  + *else*
    #comment[$k$ in $t$]
    + *let* $t' := ftconcat(l, r)$
    + *return* $(t', v)$
]
