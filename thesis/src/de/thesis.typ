#import "@preview/chiral-thesis-fhe:0.1.0" as ctf
#import ctf.prelude: *

#import "/src/util.typ"

#show "C++": util.cpp

// convenient smallcaps for simple author names
#show regex("![A-Za-z]{2,}\b"): it => smallcaps(it.text.slice(1))

#show: doc(
  kind: masters-thesis(
    id: [AI-2024-MA-005],
    title: [Dynamische Datenstrukturen unter Echtzeitbedingungen],
    author: "B. Sc. Erik Bünnig",
    supervisors: (
      "Prof. Dr. Kay Gürtzig",
      "Dipl-Ing. Peter Brückner",
    ),
    date: datetime(year: 2024, month: 10, day: 09),
    field: [Angewandte Informatik],
  ),
  draft: false,
  abstracts: (
    (title: [Kurzfassung], body: [
      Die Verwendung dynamischer Datenstrukturen unter Echtzeitbedingungen muss genau geprüft werden um sicher zustellen, dass ein Echtzeitsystem dessen vorgegebene Aufgaben in der erwarteten Zeit erfüllen kann.
      Ein solches Echtzeitsystem ist das Laufzeitsystem der T4gl-Progrmmiersprache, eine Domänenspezifische Sprache für Industrieprüfmaschinen.
      In dieser Arbeit wird untersucht, auf welche Weise die in T4gl verwendeten Datenstrukturen optimiert oder ausgetauscht werden können, um das Zeitverhalten unter Worst Case Bedingungen zu verbessern.
      Dabei werden vorallem persistente Datenstrukturen implementiert, getestet und verglichen.
    ]),
    (title: [Abstract], body: [
      The usage of dynamic data structures under real time constraints must be analyzed precisely in order to ensure that a real time system can execute its tasks in the expected time.
      Such a real time system is the runtime of the T4gl programming language, a domain-specific language for industrial measurement machines.
      This thesis is concerned with the analysis, optimization and re-implementation of T4gl's data structures, in order to improve their worst-case time complexity.
      For this, various, but foremost persistent data structures are implemented, benchmarked and compared.
    ]),
  ),
  outlines: (
    (target: image, title: [Abbildungsverzeichnis]),
    (target: table, title: [Tabellenverzeichnis]),
    (target: raw,   title: [Listingverzeichnis]),
  ),
  outlines-position: start,
  bibliography: bibliography("/src/bib.yaml", title: "Literatur"),
  acknowledgement: [
    Ich bedanke mich bei Kay Gürtzig für die wissenschaftliche Genauigkeit seines Feedbacks und die viele Zeit die er trotz vieler andere Pflichten in zahllose Rücksprachetermine investiert hat.
    Durch sein genaues Hinsehen wurden viele Fehler gefunden, welche gerade bei Änderungen schnell vergessen werden.
    Gleichermaßen bedanke ich mich bei Peter Brückner und Ralf Müller für deren Betreuung von seiten der Firma Brückner und Jarosch Ingeneugesellschaft mbH (BJ-IG).
    Ihre Unterstüzung, Zeit und Vorschläge haben dann geholfen venn Ergebnisse unrealistisch oder Beweise unmöglich erschienen.
    Desweiteren bedanke ich mich bei allen Problelesern, welche mir die Fehler gezeigt haben, welche man als Autor nach dem 30. mal Lesen des eigenen Texts nicht mehr sieht.
    Ohne die Unterstütung meiner Kollegen bei BJ-IG, meine probelesenden Freunde und Betreuer wäre ich nicht so weit gekommen.
    Ich bedanke mich bei allen Freunden und Familienmitgliedern, welche einfach nur da waren, wenn ich an etwas anderes denken wollte als der herannahende Abgabe Termin.

    In der Hoffnung, dass ich schon morgen anderen so helfen kann wie sie mir geholfen haben, danke!
  ],
)

#set grid.cell(breakable: false)
#show figure.where(kind: "algorithm"): set grid.cell(breakable: true)
#show figure.where(kind: "algorithm"): set par(justify: false)

#set raw(syntaxes: "/assets/t4gl.sublime-syntax")

#chapter(label: <chap:intro>)[Einleitung]
#include "chapters/1-intro.typ"

#chapter(label: <chap:t4gl>)[T4gl]
#include "chapters/2-t4gl.typ"

#chapter(label: <chap:non-solutions>)[Lösungsansätze]
#include "chapters/3-non-solutions.typ"

#chapter(label: <chap:persistence>)[Persistente Datastrukturen]
#include "chapters/4-persistent-data-structures.typ"

#chapter(label: <chap:impl>)[Implementierung]
#include "chapters/5-implementation.typ"

#chapter(label: <chap:benchmarks>)[Analyse & Vergleich]
#include "chapters/6-benchmarks.typ"

#chapter(label: <chap:conclusion>)[Fazit]
#include "chapters/7-conclusion.typ"
