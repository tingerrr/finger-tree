#import "@local/chiral-thesis-fhe:0.1.0" as ctf
#import ctf.prelude: *

#import "@preview/cetz:0.2.2"
#import "@preview/fletcher:0.5.0"
#import "@preview/showybox:2.0.1"
#import "@preview/cheq:0.1.0"
#import "@preview/lovelace:0.3.0"
#import "@preview/oxifmt:0.2.1"

#let math-type(ty) = $text(#rgb("#407959"), ty)$
#let math-func(ty) = $op(text(#fuchsia.darken(35%), ty))$

#let cpp = box([C] + text(0.75em, baseline: -0.175em)[++])

#let cbox(color) = box(rect(stroke: black + 0.75pt, fill: color, height: 0.5em, width: 0.5em))
 
#let _block = block

#let todo(..annotation, body) = {
  if annotation.named().len() != 0 {
    panic(oxifmt.strfmt("Unknown named args: {}", annotation.named()))
  } else if annotation.pos().len() > 1 {
    panic("Only one annotation may be used")
  }

  let annotation = annotation.pos().at(0, default: none)

  [#metadata(body) <todo>]

  set par.line(numbering: none)

  showybox.showybox(
    title: [TODO] + if annotation != none [: #annotation], 
    title-style: (
      weight: 900,
      color: red.darken(40%),
      sep-thickness: 0pt,
    ),
    frame: (
      title-color: red.transparentize(80%),
      border-color: red.darken(40%),
      body-color: white.transparentize(100%),
      thickness: (left: 1pt),
      radius: 0pt,
    ),
    {
      set text(lang: "en")
      show: cheq.checklist
      body
    },
  )
}

#let no-cite = {
  [#metadata(none) <todo>]
  "["
  h(0pt, weak: true)
  text(red, i18n(de: [Quelle benötigt], en: [citation needed]))
  h(0pt, weak: true)
  "]"
}
