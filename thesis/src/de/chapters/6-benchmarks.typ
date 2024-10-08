#import "/src/util.typ": *
#import "/src/figures.typ"

#let data = json("/src/benchmarks.json")
#let benchmarks = (:)

#for entry in data.benchmarks {
  let name = entry.remove("name")

  if "BigO" in name or "RMS" in name {
    continue
  }

  name = name.trim(at: start, "benchmarks::")

  let (type_, method, arg) = name.split(regex("::|/"));

  if type_ not in benchmarks {
    benchmarks.insert(type_, (:))
  }

  if method not in benchmarks.at(type_) {
    benchmarks.at(type_).insert(method, (:))
  }

  benchmarks.at(type_).at(method).insert(arg, entry)
}

Zusätzlich zur theoretischen Analyse wurden zu verschiedenen verwendeten Datenstrukturen Benchmarks durchgeführt, um zu testen, ob eine simple Implementierung die theoretischen Erwartungen bestätigen kann.
Zur Kontrolle der Ergebnisse der neuen Implementierung wurden verschiedene Benchmarks mit den bis dato in T4gl verwendeten Datenstrukturen, sowie einer naiven persistenten B-Baum-Implementierung durchgeführt.
Die Tests beschränken sich auf die für die Arbeit relevanten Szenarien.
Dazu zählen Lesezugriffe und verschiedene Szenarien, in welchen worst-case Verhalten getestet wurde.
Dadurch sollen vor allem die Vor- und Nachteile granular-persistenter Datentrukturen wie 2-3-Fingerbäumen gegenüber grob-persistenten wie QMap hervorgehoben werden.

= Umgebung & Auswertung
Die Benchmarks wurden mit #link("https://github.com/google/benchmark", `google/benchmark`) ` v1.9.0` und #link("https://github.com/qt/qtbase", `Qt`) `5.15.15` durchgeführt.
Kompilation erfolgte mit `g++` `(gcc)` `14.2.1 20240910` und C++ 20.
Für die Kompilation wurde Optimierungslevel 2 (`-O2`), sowie Link-Time-Optimization (`-lto`, `-fno-fat-lto-objects`) verwendet.
Sowohl Benchmarking und Kompilation erfolgten auf `Arch Linux 6.10.10-arch1-1` mit einem 11th Gen Intel® Core™ i7-11 auf einem Lenovo ThinkPad E15 G2 i7-1165G7 TS.

Bei der Darstellung der Zeiten wird die gemessene Systemzeit verwendet und auf 2 Kommastellen genau angezeigt.
Die Iterationen-Spalte gibt die Anzahl der getesteten Aufrufe an, jede Iteration wurde mit den gleichen Ursprungsbedingungen getestet.
Bei höheren Zeiten senkt `google/benchmark` die Anzahl der Iterationen automatisch ab, sodass pro Szenario etwa eine Sekunde lang Werte erfasst werden.
Die Werte der Zeit-Spalte ergeben sich aus dem Durchschnitt aller Iterationen.
Da es sich lediglich um Datenstrukturen im Arbeitsspeicher handelt, weicht die CPU-Zeit nicht weit von der gemessenen Systemzeit ab und wurde daher nicht mit in die Tabellen aufgenommen.
Ähnlich wie auch beim Source Code werden die Daten im archivierten Repo vollständigt mit aufgeführt und können unter `src/bechmarks.json` gefunden werden.

= Kontrollgruppen
== QMap
Zunächst werden zwei Szenarien getestet, welche das Kernproblem der QMap-Implementierung hervorheben sollen: wiederholte Schreibzugriffe bei nicht-einzigartigen Referenten.
Zusätzlich dazu wird auch ein durchschnittlicher Lesezugriff gemessen.

#let benchmarks-table(type) = table(
  columns: 4,
  align: (left,) + (right,) * 3,
  table.header[Szenario][Größe][Zeit][Iterationen],
  ..type.pairs().map(((method, entries)) => {
    (
      table.cell(rowspan: entries.len(), raw(block: false, method)),
      ..entries.pairs().map(((arg, entry)) => {
        (
          arg,
          oxifmt.strfmt("{:.2} {}", entry.real_time, entry.time_unit),
          [#entry.iterations],
        )
      }),
      table.hline(stroke: 0.5pt),
    )
  }).flatten(),
)

#figure(
  benchmarks-table(benchmarks.qmap),
  caption: [Die verschiedenen Iterationen der QMap Benchmarks.],
) <tbl:bench-qmap>

@tbl:bench-qmap zeigt die zuvorgenannten Szenarien.
Das erste Szenario `get` zeigt einen durchschnittlichen Lesezugriff auf QMaps verschiedener Größen.
Die Szenarien `insert_unique` und `insert_shared` testen das Einfügen von Werten für eine QMap mit einem Referenten (`unique`) und mehreren Referenten (`shared`).
Dabei ist ein klarer Sprung der benötigten Zeit zwischen beiden Szenarien zu sehen.
Sobald eine QMap-Instanz nicht der einzige Referent ist, muss der gesamte Speicher der QMap kopiert werden, um ein einziges Element hinzuzufügen.

== Persistenter B-Tree
Da 2-3-Fingerbäume von 2-3-Bäumen abgeleitet wurden und keine Generalisiernug der Zweigfaktoren gelungen ist, ist als Kontrolle auch eine simple B-Baum Implementierung vorhanden.
Ähnlich wie die der 2-3-Fingerbäume, ist diese Implementierung granular-persistent, die Persistenz erfolgt auf jeder Knotenebene.

#figure(
  benchmarks-table(benchmarks.b_tree),
  caption: [Die verschiedenen Iterationen der B-Baum Benchmarks.],
) <tbl:bench-btree>

@tbl:bench-btree zeigt zwei Szenarien.
Ähnlich wie bei QMap wird der durchschnittliche Lesezugriff sowie das Einfügen von Werten gemessen.
Da die B-Baum Implementierung generell eine Pfadkopie bei einem Schreibzugriff erzeugt, selbst wenn diese Instanz der einzige Referent ist, ist für das Einfügen nur ein Szenario vorhanden.

= 2-3-Fingerbäume
Für 2-3-Fingerbäume wurden verschiedene Szenarien getestet, aber nicht direkt Szenarien für das Einfügen von Werten in den Baum.
Da die Implementierung von 2-3-Fingerbäumen als geordnete Sequenzen `insert` durch `split`, gefolgt von `push` und dann `concat` umgesetzt, kann dabei nicht ohne Weiteres ein worst-case für alle drei Operationen erzeugt werden, da diese voneinander abhängen.
Stattdessen wird für alle drei Operationen das worst-case Szenario gemessen, um daraus Schlüsse auf die worst-case Performance von `insert` zu ziehen.

@tbl:bench-finger-tree enthält fünf Szenarien:
- `get` wie zuvor als durchschnittlicher Lesezugriff,
- `split` für das Trennen von 2-3-Fingerbäumen,
- `concat` für das Zusammenführen von 2-3-Fingerbäumen,
- `push_worst` als worst-case `push`-Szenario und
- `push_avg` als durchschnittliches `push`-Szenario.

Die Szenarien `split` und `concat` testen dabei mit Bäumen, welche gezielt aufgebaut wurden um den Worst Case zu simulieren.
Bei `split` werden zufällige Werte zwischen `INT_MIN` und `INT_MAX` durch `std::rand()` generiert und eingefügt.
Gleichermaßen werden diese Werte behalten und deren Median verwendet, um bei einem möglichst tiefen Punkt im Baum die Trennung zu erzwingen.
Das geschieht unter der Annahme, dass die gleichmäßig verteilten Werte von `std::rand()` einen relativ balancierten Baum erzeugen, in welchem der Median im tiefesten Knoten liegt.
Die Schlüssel für das Szenario `get` werden ebenfalls auf diese Weise ausgesucht, um möglichst tief in den Baum gehen zu müssen.
Im Szenario `concat` wird ein 2-3-Fingerbaum mit sich selbst verknüpft.
Dabei wird zwar zwangsläufig die Ordnungsrelation der Schlüssel ignoriert, diese hat aber keinen Einfluss auf die Implementierung von `concat` oder anderweitige Implementierungsdetails des Benchmarkszenarios.
Die Verknüpfung mit sich selbst ermögicht dabei ein Szenario, in welchem kein seichterer Baum existiert, welcher die Rekursion frühzeitig stoppen würde.

Die Szenarios `push_worst` und `push_avg` zeigen jeweils die worst-case und average-case Szenarien von `push`.
Zur Erstellung des worst-case Szenarios wurden Bäume erstellt, bei welchen ein weiterer `push` bis in die tiefste Ebene überläuft.

Das Szenario `push_avg` zeigt, warum es nicht einfach ist, direkt `insert` zu testen.
Es müssten Bäume erstellt werden, welche nach ihrer Trennung genau so aufgebaut sind, dass auch die `push`-Operation danach den Schlimmstfall zeigt, genau so wie `concat` nach dieser.
Es ist nicht trivial, die genaue Struktur der Bäume zu kontrollieren, vor allem bei hohen Datenmengen.
Bei Versuchen, die `insert`-Operation direkt zu messen, zeigte sich das vor allem in stärkerem Rauschen der erfassten Zeiten.
Daher werden zum Einordnen von `insert` die individuellen worst-case Ergebnisse von `split`, `push_worst` und `concat` verwendet, auch wenn diese möglicherweise nie zusammen auftreten können.

#figure(
  benchmarks-table(benchmarks.finger_tree),
  caption: [Die verschiedenen Iterationen der 2-3-Fingerbaum Benchmarks.],
) <tbl:bench-finger-tree>

Die zuvorgenannten Szenarien sind in @tbl:bench-finger-tree zu sehen.
Aufällig sind dabei direkt die Sprünge in `get` bei 65536 und 524288, es ist unklar, ob es sich um ein Implementierungsproblem handelt oder ein inherentes Problem mit 2-3-Fingerbäumen.
Mehrere Durchläufe der Benchmarks zeigten ähnliche Sprünge bei den gleichen Werten auf.


= Vergleich
Bei Lesezugriffen steigt $t$ für alle Datenstrukturen etwa logarithmisch zu $n$, jedoch mit verschiedenen Faktoren.
2-3-Fingerbäume zeigen dabei einen besonders hohen Faktor und schneiden schlechter ab.
Bei QMap gibt es allerdings bei 200.000 bis 500.000 eine Verschlechterung, dort gibt es einen linearen Sprung (zu sehen in @tbl:bench-qmap, Szenario `get`).
Das stimmt mit den theoretischen Erwartungen überein, während die zuvor erwähnten Sprünge der 2-3-Fingerbäume noch unerklärt bleiben.

#let plot-operation(type, op) = cetz.plot.add(
  op.pairs().map(((arg, entry)) => (int(arg), entry.real_time)),
  label: box(inset: 0.25em, raw(block: false, type)),
)

#figure(
  cetz.canvas({
    import cetz.draw: *
    import cetz.plot

    cetz.draw.set-style(axes: (bottom: (tick: (label: (angle: 45deg, anchor: "north-east")))))

    plot.plot(
      size: (9, 7),
      x-label: $n$,
      y-label: $t$,
      y-unit: "ns",
      {
        for (type, methods) in benchmarks {
          plot-operation(type, methods.get)
        }
      }
    )
  }),
  caption: [Vergleich der Lesezugriffe in Abhängigkeit der Anzahl der Elemente $n$.],
) <fig:bench-get>

Für den Vergleich der `insert`-Operation werden für 2-3-Fingerbäume die Datenpunkte der `split`-, `push_worst`- und `concat`-Operationen aufsummiert.
Es ist möglich, dass selbst im realen worst-case solche Werte nie gemeinsam vorkommen.
So ist zum Beispiel nach dem worst-case `push` der Baum dort minimal auf jeder Ebene besetzt, wo dieser für den `concat` worst-case maximal besetzt sein müsste.
Die Summierung aller worst-case Werte gibt daher eine besonders pessimistische, aber sichere Aussage über die worst-case Performance von `insert`.

#figure(
  cetz.canvas({
    import cetz.draw: *
    import cetz.plot

    cetz.draw.set-style(axes: (bottom: (tick: (label: (angle: 45deg, anchor: "north-east")))))

    plot.plot(
      size: (9, 7),
      x-label: $n$,
      y-label: $t$,
      y-unit: "ns",
      {
        for (type, methods) in benchmarks {
          if type == "qmap" {
            plot-operation(type, methods.insert_unique)
          } else if type == "b_tree" {
            plot-operation(type, methods.insert)
          } else {
            plot-operation(
              type,
              methods
                .split
                .pairs()
                .map(((arg, entry)) => (
                  (arg): (
                    real_time: entry.real_time
                      + methods.push_worst.at(arg).real_time
                      + methods.concat.at(arg).real_time
                    )
                  )
                )
                .fold((:), (acc, it) => acc + it)
            )
          }
        }
      }
    )
  }),
  caption: [Vergleich der Schreibzugriffe in Abhängigkeit der Anzahl der Elemente $n$.],
) <fig:bench-insert>

@fig:bench-insert zeigt für B-Baum das Senario `insert`, für QMap das Szenario `insert_unique` und für 2-3-Fingerbaum die Summe der Szenarien `spit`, `push_worst` und `concat`.
Dabei ist zu beachten, dass das worst-case QMap Szenario so hohe Werte erziehlt, dass die Werte von B-Baum und 2-3-Fingerbaum gleich aussehen (siehe @tbl:bench-qmap, Szenario `insert_shared`).
Die Abbildung zeigt, dass 2-3-Fingerbäume in ihrer jetzigen Implementierung deutlich schlechter abschließen als QMap im best-case Szenario und die B-Baum-Implementierung generell.
Allerdings schließen beide Baum-Implementierungen weitaus besser als QMap ab sobald mehr als ein Referent existiert, wie es in T4gl oft der Fall ist.
