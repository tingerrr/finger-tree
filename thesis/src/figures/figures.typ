#import "util.typ": *

//
// linked lists
//

#let list-new = fdiag({
  instance((0, 0), `l`)
  edge("-|>")
  node((0.5, 0), `A`)
  edge("-|>")
  node((1, 0), `B`)
  edge("-|>")
  node((1.5, 0), `C`)
})

#let list-copy = fdiag({
  instance((0, 0), `l`)
  edge("-|>")
  node((0.5, 0), `A`, stroke: green)
  edge("-|>")
  node((1, 0), `B`, stroke: green)
  edge("-|>")
  node((1.5, 0), `C`, stroke: green)
  instance((0, 0.5), `m`)
  edge((0.5, 0), "-|>")
})

#let list-pop = fdiag({
  instance((0, 0), `l`)
  edge("-|>")
  node((0.5, 0), `A`)
  edge("-|>")
  node((1, 0), `B`, stroke: green)
  edge("-|>")
  node((1.5, 0), `C`, stroke: green)
  instance((0, 0.5), `n`)
  edge((1, 0), "-|>")
})

#let list-push = fdiag({
  instance((0, 0), `l`)
  edge("-|>")
  node((0.5, 0), `A`)
  edge("-|>")
  node((1, 0), `B`, stroke: green)
  edge("-|>")
  node((1.5, 0), `C`, stroke: green)
  instance((0, 0.5), `o`)
  edge("-|>")
  node((0.5, 0.5), `D`)
  edge((1, 0), "-|>")
})

//
// t4gl arrays
//

#let t4gl-layers-new = fdiag({
  instance((0, 0), `a`, name: <i>)
  edge("-|>")
  instance((0, 0.5), `storage`, name: <s>)
  edge("-|>")
  node((0, 1), `buffer`, name: <b>)
})

#let t4gl-layers-shallow = fdiag({
  instance((-0.5, 0), `a`, name: <i_a>)
  edge("-|>")
  instance((0, 0.5), `shared storage`, name: <s>, stroke: green)
  edge("-|>")
  node((0, 1), `buffer`, name: <b>, stroke: green)

  instance((0.5, 0), `b`, name: <i_b>)
  edge(<i_b>, <s>, "-|>")
})

#let t4gl-layers-deep-new = fdiag({
  instance((-0.5, 0), `a`, name: <i_a>)
  instance((0.5, 0), `c`, name: <i_c>)

  instance((-0.5, 0.5), `storage a`, name: <s_a>)
  instance((0.5, 0.5), `storage c`, name: <s_c>, stroke: red)

  node((0, 1), `shared buffer`, name: <b>, stroke: green)

  edge(<i_a>, <s_a>, "-|>")
  edge(<i_c>, <s_c>, "-|>")
  edge(<s_c>, <b>, "-|>")
  edge(<s_a>, <b>, "-|>")
})

#let t4gl-layers-deep-mut = fdiag({
  instance((-0.5, 0), `a`, name: <i_a>)
  instance((0.5, 0), `c`, name: <i_c>)

  instance((-0.5, 0.5), `storage a`, name: <s_a>)
  instance((0.5, 0.5), `storage c`, name: <s_c>)

  node((-0.5, 1), `buffer a`, name: <b_a>)
  node((0.5, 1), `buffer c`, name: <b_c>, stroke: red)

  edge(<i_a>, <s_a>, "-|>")
  edge(<i_c>, <s_c>, "-|>")
  edge(<s_a>, <b_a>, "-|>")
  edge(<s_c>, <b_c>, "-|>")
})

//
// vector
//

#let vector-repr = cetz.canvas({
  import cetz.draw: *

  rotate(x: 180deg)

  rect((-0.1, -0.1), (1.1, 3.1))
  grid((0, 0), (1, 3))
  content((0.5, 0.5), `ptr`)
  content((0.5, 1.5), `len`)
  content((0.5, 2.5), `cap`)

  grid((3, 0), (4, 4), stroke: gray)
  content((3.5, 0.5), `e1`)
  content((3.5, 1.5), `e2`)
  content((3.5, 2.5), `e3`)
  content((3.5, 3.5), `...`)

  line((1, 0.5), (3, 0.5), mark: (end: (symbol: ">", fill: black)))
})

//
// trees
//

#let tree-new = fdiag({
  instance((0, 0), `t`, name: <root>)
  node((0, 0.5), `A`, name: <A>)
  node((-0.5, 1), `B`, name: <B>)
  node((0.5, 1), `C`, name: <C>)
  node((-1, 1.5), `D`, name: <D>)
  node((0, 1.5), `E`, name: <E>)
  node((1, 1.5), `X`, name: <X>, stroke: dstroke(gray))

  edge(<root>, <A>, "-|>")
  edge(<A>, <B>, "-|>")
  edge(<A>, <C>, "-|>")
  edge(<B>, <D>, "-|>")
  edge(<B>, <E>, "-|>")
  edge(<C>, <X>, "-|>", stroke: dstroke(gray))
})

#let tree-shared = fdiag({
  instance((0, 0), `t`, name: <t-root>)
  node((0, 0.5), `A`, name: <t-A>)
  node((-0.5, 1), `B`, name: <B>, stroke: green)
  node((0.5, 1), `C`, name: <t-C>)
  node((-1, 1.5), `D`, name: <D>, stroke: green)
  node((0, 1.5), `E`, name: <E>, stroke: green)

  edge(<t-root>, <t-A>, "-|>")
  edge(<t-A>, <B>, "-|>")
  edge(<t-A>, <t-C>, "-|>")

  edge(<B>, <D>, "-|>")
  edge(<B>, <E>, "-|>")

  instance((1, 0), `u`, name: <u-root>)
  node((1, 0.5), `A`, name: <u-A>, stroke: red)
  node((1.5, 1), `C`, name: <u-C>, stroke: red)
  node((2, 1.5), `X`, name: <X>)

  edge(<u-A>, <B>, "-|>")

  edge(<u-root>, <u-A>, "-|>")
  edge(<u-A>, <u-C>, "-|>")
  edge(<u-C>, <X>, "-|>")
})

//
// b-tree
//

#let b-tree-node = cetz.canvas({
  import cetz.draw: *

  rotate(x: 180deg)

  grid((0, 0), (3, 1))
  content((0.5, 0.5), $s_1$)
  content((1.5, 0.5), $s_2$)
  content((2.5, 0.5), $s_3$)

  line((-1, 2), (0, 1), name: "l1")
  line((0.5, 2), (1, 1), name: "l2")
  line((2.5, 2), (2, 1), name: "l3")
  line((4, 2), (3, 1), name: "l4")

  for l in range(1, 5) {
    content(
      ("l" + str(l) + ".start", 50%, "l" + str(l) + ".end"),
      box(
        fill: white,
        outset: (bottom: 0.4em, x: 0.2em),
        $k_#l$
      ),
    )
  }
})

//
// finger-tree
//

#let finger-tree-ranges  = [
  #let (kmin, kmax) = (2, 3)
  #let (dmin, dmax) = (1, 4)

  #let fmins(t) = calc.pow(kmin, t - 1)
  #let fmin(t) = 2 * dmin * calc.pow(kmin, t - 1)
  #let fmax(t) = 2 * dmax * calc.pow(kmax, t - 1)

  #let fcummin(t) = range(1, t + 1).map(fmin).fold(0, (acc, it) => acc + it)
  #let fcummins(t) = fmins(t) + fcummin(t - 1)
  #let fcummax(t) = range(1, t + 1).map(fmax).fold(0, (acc, it) => acc + it)

  #let ranges(t) = {
    let individual = range(2, t + 1).map(t => (
      t: t,
      mins: fmins(t),
      min: fmin(t),
      max: fmax(t),
    ))

    individual
  }

  #let cum-ranges(t) = {
    let cummulative = range(1, t + 1).map(t => (
      t: t,
      mins: fcummins(t),
      min: fcummin(t),
      max: fcummax(t),
    ))

    cummulative
  }

  #let count = 8
  #let cum-ranges = cum-ranges(count)
  #let mmax = cum-ranges.last().max

  #let base = 2
  #let sqr = calc.pow.with(base)
  #let lg = calc.log.with(base: base)
  #let tick-max = int(calc.round(lg(mmax)))
  #let tick-args = (
    x-tick-step: none,
    x-ticks: range(tick-max + 1).map(x => (x, sqr(x))),
  )

  // comment out to make linear scale plot
  // #let sqr = x => x
  // #let lg = x => x
  // #let tick-max = int(lg(mmax))
  // #let tick-args = ()

  #import "@preview/cetz:0.2.2"

  #cetz.canvas({
    cetz.draw.set-style(axes: (bottom: (tick: (label: (angle: 45deg, anchor: "north-east")))))

    cetz.plot.plot(
      size: (9, 6),
      x-label: $n$,
      y-label: $t$,
      y-tick-step: none,
      y-ticks: range(1, count + 1),
      plot-style: cetz.palette.pink,
      ..tick-args,
      {
        let intersections(n) = {
          cetz.plot.add-vline(
            style: (stroke: (paint: gray.lighten(70%), dash: "dashed")),
            lg(n),
          )
          cetz.plot.add(
            label: box(inset: 0.2em)[$n' = #n$],
            style: (stroke: none),
            mark-style: cetz.palette.new(
              colors: color.map.crest.chunks(32).map(array.first)
            ).with(stroke: true),
            mark: "x",
            cum-ranges.filter(r => r.mins <= n and n <= r.max).map(r => (lg(n), r.t))
          )
        }
        // force the plot domain
        cetz.plot.add(
          style: (stroke: none),
          ((-1, 0), (tick-max + 1, count + 1)),
        )
        for t in cum-ranges {
          cetz.plot.add(
            label: if t.t == 1 { box(inset: 0.2em)[$n'(t)$] },
            domain: (0, lg(mmax)),
            style: cetz.palette.blue.with(stroke: true),
            mark-style: cetz.palette.blue.with(stroke: true),
            mark: "|",
            (
              (lg(t.mins), t.t),
              (lg(t.max), t.t),
            )
          )
        }
        intersections(9)
        intersections(27)
        intersections(250)
      },
    )
  })

  // #table(
  //   columns: 6,
  //   align: right,
  //   table.header[$t$],
  //   ..cum-ranges.map(t => (t.t, t.mins, $<=$, $n$, $<=$, t.max)).flatten().map(x => $#x$)
  // )
]

#let finger-tree = fdiag({
  let elem = node.with(radius: 7.5pt)
  let spine = node.with(radius: 7.5pt, fill: blue.lighten(75%))
  let node = node.with(radius: 5pt, fill: gray.lighten(50%))

  let elems(coord, data, parent: none, ..args) = {
    let parent = if parent != none { parent } else {
      label(str(data.first()) + "-" + str(data.last()))
    }
    let (x, y) = coord
    let deltas = range(0, data.len()).map(dx => dx * 0.3)
    let deltas = deltas.map(dx => dx - deltas.last() / 2)

    for (e, dx) in data.zip(deltas) {
      let l = label(str(e))

      elem((x + dx, y), raw(str(e)), name: l, ..args)
      edge(parent, l, "-|>")
    }
  }

  let fill-digit-elem = gradient.linear(angle: 45deg, teal.lighten(50%), white).sharp(2)
  let fill-digit-node = gradient.linear(angle: 45deg, teal.lighten(50%), gray).sharp(2)

  instance((0, -0.5), `t`)
  edge("-|>")
  spine((0, 0), name: <l1>)
  edge("-|>")
  spine((0, 0.5), name: <l2>)
  edge("-|>")
  spine((0, 1), name: <l3>)

  elems((-3.15, 1.25), (1, 2, 3), parent: <l1>, fill: fill-digit-elem)
  elems((2.15, 1.25), (20, 21), parent: <l1>, fill: fill-digit-elem)

  node((-2.45, 1.25), name: <4-5>, fill: fill-digit-node)
  node((-1.7, 1.25), name: <6-8>, fill: fill-digit-node)
  node((1.7, 1.25), name: <18-19>, fill: fill-digit-node)

  elems((-2.45, 1.75), (4, 5))
  elems((-1.7, 1.75), (6, 7, 8))
  elems((1.7, 1.75), (18, 19))

  edge(<l2>, <4-5>, "-|>")
  edge(<l2>, <6-8>, "-|>")
  edge(<l2>, <18-19>, "-|>")

  node((-0.725, 1.25), name: <9-12>, fill: fill-digit-node)
  node((0.725, 1.25), name: <13-17>, fill: fill-digit-node)

  node((-1.1, 1.75), name: <9-10>)
  node((-0.35, 1.75), name: <11-12>)
  node((0.35, 1.75), name: <13-14>)
  node((1.1, 1.75), name: <15-17>)

  elems((-1.1, 2.25), (9, 10))
  elems((-0.35, 2.25), (11, 12))
  elems((0.35, 2.25), (13, 14))
  elems((1.1, 2.25), (15, 16, 17))

  edge(<l3>, <9-12>, "-|>")
  edge(<9-12>, <9-10>, "-|>")
  edge(<9-12>, <11-12>, "-|>")

  edge(<l3>, <13-17>, "-|>")
  edge(<13-17>, <13-14>, "-|>")
  edge(<13-17>, <15-17>, "-|>")
})

//
// srb tree
//

#let srb-tree = fdiag({
  let arraybox(..args, leaf: false, hl: (0,)) = {
    let items = args.pos()
    grid(
      inset: 5pt,
      stroke: 1pt,
      fill: (x, y) => if x in hl { teal.lighten(50%) },
      columns: if leaf { int(items.len() / 2) } else { items.len() },
      rows: 1.5em,
      ..items.map(i => [#i])
    )
  }

  let node = node.with(inset: 0pt, stroke: none)

  instance((0, -0.5), `t`, name: <t>)
  edge("-|>")
  node((0, 0), arraybox(0, [...], 15, hl: (0, 2)), name: <l1>)

  node((-0.5, 0.5), arraybox(0, [...], 15), name: <l2-1>)
  node((0.5, 0.5), arraybox(0, [...], 15, hl: (2,)), name: <l2-2>)

  edge(<l1>, <l2-1>, "-|>")
  edge(<l1>, <l2-2>, "-|>")

  node((-1, 1), arraybox(0, 1, [...], 15, hl: (0, 1)), name: <l3-1>)
  node((1, 1), arraybox(0, 1, [...], 14, 15, hl: (1, 3)), name: <l3-2>)

  edge(<l2-1>, <l3-1>, "-|>", shift: -2pt)
  edge(<l2-2>, <l3-2>, "-|>")

  node((-1.5, 1.75), arraybox(leaf: true, 0, [...], 15, 0, [...], 15), name: <l4-1>)
  node((-0.5, 1.75), arraybox(leaf: true, 0, [...], 15, 16, [...], 31), name: <l4-2>)
  node((0.5, 1.75), arraybox(leaf: true, 0, [...], 15, 65296, [...], 65311), name: <l4-3>)
  node((1.5, 1.75), arraybox(leaf: true, 0, [...], 15, 65504, [...], 65519, hl: (2,)), name: <l4-4>)

  edge(<l3-1>, <l4-1>, "-|>", shift: -10pt)
  edge((rel: (-30pt, 0pt), to: <l3-1>), <l4-2>, "-|>")
  edge(<l3-2>, <l4-3>, "-|>", shift: -4pt)
  edge(<l3-2>, <l4-4>, "-|>", shift: -2pt)
})
