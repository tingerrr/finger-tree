#import "/src/util.typ": fletcher, cetz, algorithm, i18n, math-type, math-func
#import fletcher: edge, node

#let complexity-comparison(
  cases: ([worst], [average], [best]),
  columns,
  ..args,
) = {
  let headers = columns.keys()
  let rows = (:)
  for (column, entry) in columns {
    for key in entry.keys() {
      if key not in rows {
        rows.insert(key, (:))
      }

      if column not in entry.at(key) {
        rows.at(key).insert(column, ())
      }

      for idx in range(cases.len()) {
        let comp = entry.at(key).at(idx)
        rows.at(key).at(column).push(if comp == none [-] else { comp })
      }
    }
  }

  let column-gutter = (
    ..(0.5em,) + ((0pt,) * (cases.len() - 1)),
  ) * (columns.len() + 1)

  table(
    columns: 1 + columns.len() * cases.len(),
    stroke: none,
    align: (x, y) => if y > 1 { left } else { center },
    column-gutter: column-gutter,
    ..args.named(),
    table.header(
      [Operation], ..headers.map(table.cell.with(colspan: cases.len(), stroke: (bottom: 0.5pt))),
      none, ..(cases * columns.len()),
      table.hline(stroke: 0.5pt),
    ),
    ..rows.pairs().map(((op, entries)) => {
      (op, ..headers.map(header => {
        entries.at(header, default: ([-], ) * 3)
      }))
    }).flatten()
  )
}

#let instance = node.with(extrude: (-2, 0))

#let fdiag = fletcher.diagram.with(
  node-stroke: 0.075em,
  spacing: 4em,
)

#let dstroke(color, ..args) = (dash: "dashed", paint: color, ..args.named())

#let group(centers, name, color, ..args) = {
  let args = arguments(
    enclose: centers,
    inset: 10pt,
    ..args,
  )

  node(
    hide(name),
    layer: -1,
    stroke: dstroke(color),
    fill: color.lighten(75%),
    ..args,
  )

  node(
    context {
      let size = measure(name)
      // NOTE: align + move ensures the node thinks the text is inside, ensuring the text is not
      // hyphenated unecessarily
      align(top + left, move(
        dy: -(size.height + 10pt + 0.5em),
        dx: -10pt,
        box(fill: white, outset: 1pt, name),
      ))
    },
    layer: 1,
    stroke: none,
    fill: none,
    ..args,
  )
}
