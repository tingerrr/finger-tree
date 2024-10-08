#import "/src/util.typ": *
#import "/src/figures.typ"

Die in @chap:non-solutions beschriebenen Lösungsansätze haben alle etwas gemeinsam, sie bekämpfen Symptome statt Ursachen.
Die Häufigkeit der Kopien kann reduziert werden, aber nie komplett eliminiert, da es eine fundamentale Operation ist.
Es gilt also, ungeachtet der Häufigkeit einer solchen Operation, auf Seiten der Implementierung von T4gl-Arrays, Kopien von Daten nur dann anzufertigen, wenn das vonnöten ist.
In @sec:t4gl:arrays wurde beschrieben, wie T4gl-Arrays bereits Daten teilen und nur bei Schreibzugriffen auf geteilte Daten Kopien erstellen.
Das fundamentale Problem ist, dass bei Schreibzugriffen auf geteilte Daten, ungeachtet der Ähnlichkeit der Daten nach dem Schreibzugriff, alle Daten kopiert werden.

Im Folgenden wird beschrieben, wie effiziente Datenteilung durch Persistenz @bib:fk-03 auf Speicherebene der T4gl-Arrays die Komplexität von Typ-3-Kopien auf logarithmische Komplexität senken kann.
Es werden die Begriffe der Persistenz und Kurzlebigkeit im Kontext von Datenstrukturen eingeführt, sowie der aktuelle Stand persistenter Datenstrukturen beschrieben.
Im Anschluss werden persistente Datenstrukturen untersucht, welche als Alternative der jetzigen Qt-Instanzen verwendet werden können.

= Kurzlebige Datenstrukturen <sec:eph-data>
Um den Begriff der Persistenz zu verstehen, müssen zunächst kurzlebige oder gewöhnliche Datenstrukuren entgegengestellt werden.
Dafür betrachten wir den Vektor, ein dynamisches Array.

#figure(
  figures.vector.repr,
  caption: [Der Aufbau eines Vektors in C++.],
) <fig:vec-repr>

Ein Vektor besteht in den meisten Fällen aus 3 Komponenten, einem Pointer `ptr` auf die Daten des Vektors (in grau #cbox(gray) umranded), der Länge `len` des Vektors und die Kapazität `cap`.
@fig:vec-repr zeigt den Aufbau eines Vektors in C++ #footnote[
  Der gezeigte Aufbau ist nicht vom Standard @bib:iso-cpp-20 vorgegeben, manche Implementierungen speichern statt der Kapazität einen `end` Pointer, aus welchem die Kapaziät errechnet werden kann.
  Funktionalität und Verhalten sind allerdings gleich.
].
Operationen am Vektor wie `push_back`, `pop_back` oder Indizierung arbeiten direkt im Speicher, auf welchen `ptr` verweist.
Reicht der Speicher nicht aus, wird er erweitert und die Daten werden aus dem alten Speicher in den neuen Speicher verschoben.
Die in @lst:vec-ex gezeigten Aufrufe zu `push_back` schreiben die Werte direkt in den Speicher des Vektors.
Zu keinem Zeitpunkt gibt es mehrere Instanzen, welche auf die gleichen Daten zeigen.
Wird der Vektor `vec` auf `other` zugewiesen, werden alle Daten kopiert, die Zeitkomplexität der Kopie selbst ist proportional zur Anzahl der Elemente $Theta(n)$.

#figure(
  figures.vector.example,
  caption: [
    Ein C++ Program, welches einen `std::vector` anlegt und mit Werten befüllt.
  ],
) <lst:vec-ex>

= Persistenz und Kurzlebigkeit <sec:per-eph>
Wenn eine Datenstruktur bei Schreibzugriffen die bis dahin bestehenden Daten nicht verändert, gilt diese als persistent/langlebig @bib:kt-96[S. 202].
Daraus folgt, dass viele Instanzen sich gefahrlos die gleichen Daten teilen können.
In den Standardbibliotheken verschiedener Programmiersprachen hat sich für dieses Konzept der Begriff _immutable_ (_engl._ unveränderbar) durchgesetzt.
Im Gegensatz dazu stehen Datenstrukturen, welche bei Schreibzugriffen ihre Daten direkt beschreiben.
Diese gelten als kurzlebig.
Dazu gehört zum Beispiel der Vektor, beschrieben in @sec:eph-data.
Persistente Datenstrukturen erstellen meist neue Instanzen für jeden Schreibzugriff, welche die Daten der vorherigen Instanz wenn möglich teilen.
Die bis dato verwendeten QMaps sind persistent in dem Sinne, dass keine Instanz die Schreibzugriffe anderer Instanzen sehen kann, da diese immer vorher ihre geteilten Daten kopieren.
Allerdings können clever aufgebaute Datenstrukturen bei einem Schreibzugriff dennoch Teile ihrer Daten mit anderen Instanzen teilen.
Ein gutes Beispiel dafür bietet die einfach verkettete Liste (@fig:linked-sharing).

#subpar.grid(
  figure(figures.list.new, caption: [
    Eine Liste `l` wird über die Sequenz `[A, B, C]` angelegt.
  ]),
  figure(figures.list.copy, caption: [
    Eine Kopie `m` von `l` teilt sich den Kopf der Liste mit `l`.
  ]),
  figure(figures.list.pop, caption: [
    // NOTE: the double linebreaks are a bandaid fix for the otherwise unaligned captions
    Soll der Kopf von `m` gelöscht werden, zeigt eine neue Liste `n` stattdessen auf den Rest. \
  ]), <fig:linked-sharing:pop>,
  figure(figures.list.push, caption: [
    Soll ein neuer Kopf an `n` angefügt werden, kann der Rest durch die neue Instanz `o` weiterhin geteilt werden.
  ]),
  columns: 2,
  caption: [
    Eine Abfolge von Operationen auf persistenten verketten Listen.
  ],
  label: <fig:linked-sharing>,
)

Trotz der Veränderung von `m` in @fig:linked-sharing:pop teilen sich `l` und `n` weiterhin Daten, während QMaps in diesem Fall gar keine Daten mehr teilen würden.

Im Folgenden werden durch die Datenteilung _Instanzen_ und _Daten_ semantisch getrennt:
/ Instanzen:
  Teile der Datenstruktur, welche auf die geteilten Daten verweisen und anderweitig nur Verwaltungsinformationen enthalten.
/ Daten:
  Die Teile einer Datenstruktur, welche die eigentlichen Elemente enthalten, in @fig:linked-sharing beschreibt das die Knoten mit einfacher Umrandung, während doppelt umrandete Knoten die Instanzen sind.

Persistenz zeigt vor allem bei Baumstrukturen ihre Vorteile.
Bei der Kopie der Daten eines persistenten Baums können je nach Tiefe und Balance des Baums große Teile des Baums geteilt werden.
Ähnlich wie bei persistenten einfach verketteten Listen, werden bei Schreibzugriffen auf persistente Bäume nur die Knoten des Baums kopiert, welche zwischen Wurzel (Instanz) und dem veränderten Knoten in den Daten liegen.
Das nennt sich eine Pfadkopie (_engl._ path copying).
Betrachten wir partielle Persistenz von Bäumen am Beispiel eines Binärbaums, sprich eines Baums mit Zweigfaktoren (ausgehendem Knotengrad) zwischen $0$ und $2$.
@fig:tree-sharing illustriert, wie am Binärbaum `t` ein Knoten `X` angefügt werden kann, ohne dessen partielle Persistenz aufzugeben.
Es wird eine neue Instanz angelegt und eine Kopie der Knoten `A` und `C` angelegt, der neue Knoten `X` wird in `C` eingehängt und der Knoten `B` wird von beiden `A` Knoten geteilt.
Durch die Teilung on `B` werden auch alle Kindknoten unter `B` geteilt.

#subpar.grid(
  figure(figures.tree.new, caption: [
    Eine Baumstruktur `t`, an welche ein neuer Knoten `X` unter `C` angefügt werden soll.
  ]), <fig:tree-sharing:new>,
  figure(figures.tree.shared, caption: [
    Bei Hinzufügen des Knotens `X` als Kind des Knotens `C` wird ein neuer Baum `u` angelegt.
  ]), <fig:tree-sharing:shared>,
  columns: 2,
  caption: [
    Partielle Persistenz teilt zwischen mehreren Instanzen die Teile der Daten, welche sich nicht verändert haben, ähnlich der Persistenz in @fig:linked-sharing.
  ],
  label: <fig:tree-sharing>,
)

Für unbalancierte Bäume lässt sich dabei aber noch keine besonders gute Zeitkomplexität garantieren.
Bei einem Binärbaum mit $n$ Kindern, welcher maximal unbalanciert ist (equivalent einer verketten Liste), degeneriert die Zeitkomplexität zu $Theta(n)$ für Veränderungen am Blatt des Baums.
Ein perfekt balancierter Binärbaum hat eine Tiefe $d = log_2 n$, sodass jeder Schreibzugriff auf einem persistenten Binärbaum maximal $d$ Knoten (Pfad zwischen Wurzel und Blattknoten) kopieren muss.
Je besser ein persistenter Baum balanciert ist, desto geringer ist die Anzahl der Knoten, welche bei Pfadkopien unnötig kopiert werden müssen.
Die Implementierung von T4gl-Arrays durch persistente Bäume könnte demnach die Komplexität von Kopien drastisch verbessern.

= Lösungsansatz
Zunächst definieren wir Invarianzen von T4gl-Arrays, welche durch eine Änderung der Implementierung nicht verletzt werden dürfen:
+ T4gl-Arrays sind assoziative Datenstrukturen.
  - Es werden Werte mit Schlüsseln addressiert, nicht nur Ganzzahl-Indizes.
+ T4gl-Arrays sind nach ihren Schlüsseln geordnet.
  - Die Schlüsseltypen von T4gl-Arrays haben eine voll definierte Ordnungsrelation.
  - Iteration über T4gl-Arrays ist deterministisch in aufsteigender Reihenfolge der Schlüssel.
+ T4gl-Arrays verhalten sich wie Referenztypen.
  - Schreibzugriffe auf ein Array, welches eine flache Kopie eines anderen T4gl-Arrays ist, sind in beiden Instanzen sichtbar.
  - Tiefe Kopien von T4gl-Arrays teilen sichtbar keine Daten, ungeachtet, ob die darin enthaltenen Typen selbst Referenztypen sind.

Die Ordnung der Schlüssel schließt ungeordnete assoziative Datenstrukturen wie Hashtabellen aus.
Das Referenztypenverhalten ist dadurch umzusetzen, dass wie bis dato drei Ebenen verwendet werden (siehe @sec:t4gl:arrays), es werden lediglich die Qt-Instanzen durch neue Datenstrukturen ersetzt.
Essentiell für die Verbesserung des worst-case Zeitverhaltens bei Kopien und Schreibzugriffen ist die Reduzierung der Daten, welche bei Schreibzugriffen kopiert werden müssen.
Hauptproblem bei flachen Kopien, gefolgt von Schreibzugriff auf CoW-Datenstrukturen, ist die tiefe Kopie _aller_ Daten, selbst wenn nur ein einziges Element beschrieben oder eingefügt/entfernt wird.
Ein Großteil der Elemente in den originalen und neuen kopierten Daten ist nach dem Schreibzugriff gleich.
Durch höhere Granularität der Datenteilung müssen bei Schreibzugriffen weniger unveränderte Daten kopiert werden.

Ein Beispiel für persistente Datenstrukturen mit granularer Datenteilung sind RRB-Vektoren @bib:br-11 @bib:brsu-15 @bib:stu-15, eine Sequenzdatenstruktur auf Basis von Radix-Balancierten Bäumen.
Trotz der zumeist logarithmischen worst-case Komplexitäten können RRB-Vektoren nicht als Basis für T4gl-Arrays verwendet werden.
Die Effizienz der RRB-Vektoren baut auf der Relaxed-Radix-Balancierung auf, dabei wird von einer Sequenzdatenstrukur ausgegangen.
Da die Schlüsselverteilung in T4gl-Arrays nicht dicht ist, können die Schlüssel nicht ohne Weiteres auf dicht verteilte Indizes abgebildet werden.
Etwas fundamentaler sind B-Bäume @bib:bm-70 @bib:bay-71. 
Bei persistenten B-Bäumen haben die meisten Operationen eine worst- und average-case Zeitkomplexität von $Theta(log n)$.
Eine Verbesserung der average-case Zeitkomplexität für bestimmte Sequenzoperationen (`push`, `pop`) bieten 2-3-Fingerbäume @bib:hp-06.
Diese bieten sowohl theoretisch exzellentes Zeitverhalten, als auch keine Enschränkung auf die Schlüsselverteilung.
Im Folgenden werden unter anderem B-Bäume, sowie 2-3-Fingerbäume als alternative Storage-Datenstrukturen für T4gl-Arrays untersucht.

= B-Bäume <sec:b-tree>
Eine für die Persistenz besonders gut geeignete Datenstruktur sind B-Bäume @bib:bm-70 @bib:bay-71, da diese durch ihren Aufbau und Operationen generell balanciert sind.
Schreiboperationen auf persistenten B-Bäumen müssen lediglich $Theta(log n)$ Knoten kopieren.
B-Bäume können vollständig durch ihre Zweigfaktoren beschrieben werden, sprich, die Anzahl der Kindknoten, welche ein interner Knoten haben kann bzw. muss.
Ein B-Baum ist wie folgt definiert @bib:knu-98[S. 483]:
+ Jeder Knoten hat maximal $k_max$ Kindknoten.
+ Jeder interne Knoten, außer dem Wurzelknoten, hat mindestens $k_min = ceil(k_max \/ 2)$ Knoten.
+ Der Wurzelknoten hat mindestens 2 Kindknoten, es sei denn, er ist selbst ein Blattknoten.
+ Alle Blattknoten haben die gleiche Entfernung zum Wurzelknoten.
+ Ein interner Knoten mit $k$ Kindknoten enthält $k - 1$ Schlüssel.

Die Schlüssel innerhalb eines internen Knoten dienen als Suchhilfe.
Sie sind Typen mit fester Ordnungsrelation und treten aufsteigend geordnet auf.
@fig:b-tree zeigt einen internen Knoten eines B-Baums mit $k_min = 2$ und $k_max = 4$, ein sogannter 2-4-Baum.
Jeder Schlüssel $s_i$ mit $1 <= i <= k - 1$ dient als Trennwert zwischen den Unterbäumen in den Kindknoten $k_j$ mit $1 <= j <= k$.
Sei $x$ ein Schlüssel im Unterbaum mit der Wurzel $k_i$, so gilt

$
  &x <= s_i             &&bold("wenn") i = 1 \
  &x > s_(i - 1)        &&bold("wenn") i = k_max \
  &s_(i - 1) < x <= s_i &&bold("sonst") \
$

Die Schlüssel erlauben bei der Suche auf jeder Ebene den Suchbereich drastisch zu reduzieren.
Die simpelste Form von B-Bäumen sind sogenannte 2-3-Bäume (B-Bäume mit $k_min = 2$ und $k_max = 3$), dabei ist die Angabe der Zweigfaktoren als Prefix des Baums eine übliche Konvention.

#figure(
  figures.b-tree.node,
  caption: [Ein interner B-Baum-Knoten eines 2-4-Baums.],
) <fig:b-tree>

Da die Schlüssel in B-Bäumen anhand ihrer Ordnungsrelation sortiert sind und demnach auch nicht auf Ganzzahlwerte beschränkt sind, können diese in persistenter Form durchaus eine gute Alternative zu QMaps darstellen.

= 2-3-Fingerbäume
2-3-Fingerbäume wurden von !Hinze und !Paterson @bib:hp-06 eingeführt und sind eine Erweiterung von 2-3-Bäumen, welche für verschiedene Sequenzoperationen optimiert wurden.
Die Autoren führen dabei folgende Begriffe im Verlauf des Texts ein:
/ Spine:
  Die Wirbelsäule eines Fingerbaums, sie beschreibt die Kette der zentralen Knoten, welche die sogenannten Digits enthalten.
/ Digit:
  Erweiterung von 2-3-Knoten auf 1-4-Knoten, von welchen jeweils zwei in einem Wirbelknoten vorzufinden sind.
  Obwohl ein Wirbelknoten zwei Digits enthält, sind, statt dieser selbst, direkt deren 1 bis 4 Kindknoten an beiden Seiten angefügt.
  Das reduziert die Anzahl unnötiger Knoten in Abbildungen und entspricht mehr der späteren Implementierung.
  Demnach wird im Folgenden der Begriff Digits verwendet, um die linken und rechten Kindknoten der Wirbelknoten zu beschreiben.
/ Measure:
  Wert, welcher aus den Elementen errechnet und kombiniert werden kann.
  Dieser dient als Suchhilfe innerhalb des Baums, durch welchen identifiziert wird, wo ein Element im Baum zu finden ist, ähnlich den Schlüsseln in @fig:b-tree.
/ Safe:
  Sichere Ebenen sind Ebenen mit 2 oder 3 Digits, ein Digit kann ohne Probleme entnommen oder hinzugefügt werden.
  Zu beachten ist dabei, dass die Sicherheit einer Ebene sich auf eine Seite bezieht, eine Ebene kann links sicher und rechts unsicher sein.
/ Unsafe:
  Unsichere Ebenen sind Ebenen mit 1 oder 4 Digits, ein Digit zu entnehmen oder hinzuzufügen kann Über- bzw. Unterlauf verursachen (Wechsel von Digits zwischen Ebenen des Baums, um die Zweigfaktoren zu bewahren).

Der Name Fingerbäume rührt daher, dass imaginär zwei Finger an die beiden Enden der Sequenz gesetzt werden.
Diese Finger ermöglichen den schnellen Zugriff an den Enden der Sequenz.
Die zuvor definierten Digits haben dabei keinen direkten Zusammenhang mit den Fingern eines Fingerbaums, trotz der etymologischen Verwandschaft beider Begriffe.
@fig:finger-tree zeigt den Aufbau eines 2-3-Fingerbaums, die in blau #cbox(blue.lighten(75%)) eingefärbten Knoten sind Wirbelknoten, die in türkis #cbox(teal.lighten(50%)) eingefärbten Knoten sind die Digits.
In grau #cbox(gray) eingefärbte Knoten sind interne Knoten.
Weiße Knoten #cbox(white) sind Elemente, die Blattknoten der Teilbäume.
Knoten, welche mehr als einer Kategorie zuzuordnen sind, sind geteilt eingefärbt.

#figure(
  figures.finger-tree.repr,
  caption: [Ein 2-3-Fingerbaum der Tiefe 3 und 21 Elementen.],
) <fig:finger-tree>

Die Tiefe ergibt sich bei 2-3-Fingerbäumen aus der Anzahl der zentralen Wirbel.
Jeder Wirbelknoten beschreiben eine Ebene $t$.
Der Baum in @fig:finger-tree hat Ebene 1 bis Ebene 3.
Aus der Tiefe der Wirbelknoten ergibt sich die Tiefe der Teilbäume (2-3-Baume) in deren Digits.
Die Elemente sind dabei nur in den Blattknoten vorzufinden.
In Ebene 1 sind Elemente direkt in den Digits enhalten --- die Teilbäume haben die Tiefe 1.
In Ebene 2 sind die Elemente in Knoten verpackt, deren Digits enthalten Teilbäume der Tiefe 2.
Die dritte Ebene enthält Knoten von Knoten von Elementen, sprich Teilbäume der Tiefe 3, und so weiter.
Dabei ist zu beachten, dass der letzte Wirbelknoten einen optionalen mittleren Einzelknoten enthalten kann. Dieses Sonderdigit wird verwendet um die Bildung interner Ebenen zu überbrückung, da diese mindestens zwei Digits enhalten müssen.
Die linken Digits jedes Wirbelknotens bilden dabei den Anfang der Sequenz, während die rechten Digits das Ende der Sequenz beschreiben.
Der mittlere Teilbaum eines Wirbelknotens bildet die Untersequenz zwischen diesen beiden  Enden ab.
Die Nummerierung der Knoten in @fig:finger-tree illustriert die Reihenfolge der Elemente.
Interne Knoten und Wirbelknoten enthalten außerdem die Suchinformationen des Unterbaums, in dessen Wurzel sie liegen.
Je nach Wahl des Typs dieser Suchinformationen kann ein 2-3-Fingerbaum als gewöhnlicher Vektor, geordnete Sequenz, Priority-Queue oder Intervallbaum verwendet werden.

Durch den speziellen Aufbau von 2-3-Fingerbäumen weisen diese im Vergleich zu gewöhnlichen 2-3-Bäumen geringere amortisierte Komplexitäten für Deque-Operationen auf.
@tbl:finger-tree-complex zeigt einen Vergleich der Komplexitäten verschiedener Operationen über $n$ Elemente.
Dabei ist zu sehen, dass `push`- und `pop`-Operationen auf 2-3-Fingerbäumen im average-case amortisiert konstantes Zeitverhalten aufweisen, im Vergleich zu dem generell logarithmischen Zeitverhalten bei gewöhnlichen 2-3-Bäumen.
Fingerbäume sind symmetrische Datenstrukturen, `push` und `pop` kann problemlos an beiden Enden durchgeführt wurden.

#let am1 = [#footnote(numbering: "*")[Amortisiert] <ft:amortized>]
#let am2 = footnote(numbering: "*", <ft:amortized>)

#figure(
  figures.complexity-comparison(
    (
      "2-3-Baum": (
        search: ($Theta(log n)$, $Theta(log n)$, $Theta(log n)$),
        insert: ($Theta(log n)$, $Theta(log n)$, $Theta(log n)$),
        remove: ($Theta(log n)$, $Theta(log n)$, $Theta(log n)$),
        push:   ($Theta(log n)$, $Theta(log n)$, $Theta(log n)$),
        pop:    ($Theta(log n)$, $Theta(log n)$, $Theta(log n)$),
      ),
      "2-3-Fingerbaum": (
        search: ($Theta(log n)$, $Theta(log n)$, $Theta(log n)$),
        insert: ($Theta(log n)$, $Theta(log n)$, $Theta(log n)$),
        remove: ($Theta(log n)$, $Theta(log n)$, $Theta(log n)$),
        push:   ($Theta(log n)$, $Theta(1)#am1$, $Theta(1)$),
        pop:    ($Theta(log n)$, $Theta(1)#am2$, $Theta(1)$),
      ),
    ),
  ),
  caption: [Die Komplexitäten von 2-3-Fingerbäumen im Vergleich zu gewöhnlichen 2-3-Bäumen.],
) <tbl:finger-tree-complex>

Ein wichtiger Bestandteil der Komplexitätsanalyse der `push`- und `pop`-Operationen von 2-3-Fingerbäumen ist die Aufschiebung der Bildung von Wirbelknoten durch Lazy Evaluation.
Bei der amortisierten Komplexitätsanalyse werden diesen Aufschiebungen Credits in höhe ihrer sicheren Digits zugewiesen, welche für die Auswertung der aufgeschobenen Unterbäume in einem späteren Zeitpunkt verwendet wird.

Eine naive C++-Definition von 2-3-Fingerbäumen ist in @lst:finger-tree beschrieben.
`T` ist der Typ der im Baum gespeicherten Elemente und `M` der Typ der Suchinformationen für interne Knoten.
Im Regelfall wären alle Klassendefinitionen über `T` und `M` per ```cpp template``` parametriert, darauf wurde verzichtet, um die Definition im Rahmen zu halten.

#figure(
  figures.finger-tree.def.old,
  caption: [Die Definition von 2-3-Fingerbäumen in C++ übersetzt.],
) <lst:finger-tree>

Die Werte des Measures werden in Unterbäumen zwischengespeichert und muss dabei ein Monoid sein, sprich, er benötigt eine assoziative Operation $f : (M, M) -> M$, welche zwei Elemente des Typs `M` zu einem kombiniert und ein Identitätselement $m_0$, sodass $f(m_0, m) = m and f(m, m_0) = m$.
Diese Kombinationsoperation wird verwendet um die zwischengespeicherten Werte der Unterbäume aus deren Werten zu kombinieren.
Die Elemente des Typen `T` müssen dann eine Abbildung von $g : T -> M$ zur Verfügung stellen um die Measures der Blattknoten zu bestimmen.

Definieren wir `M` als $NN$ mit $f$ und $m_0$ als

$
  f(l, r) &: (NN, NN) -> NN \
  f(l, r) &= l + r \
  m_0 &= 0 \
$

und $g$ als

$
  g(t) &: T -> NN \
  g(t) &= 1 \
$

so kann dieses Measure die Größe von Unterbäumen beschreiben und für den Fingerbaum können Vektor-Operationen wie `split_at`, `index` oder `length` implementiert werden @bib:hp-06[S. 15].

Für die in T4gl verwendeten Arrays sind Schlüssel nach Ordnungsrelation sortiert, daher ergibt sich aus der inherenten Sortierung die Operation $f(l, r) = r$.
Leere Unterbäume werden ignoriert, damit kein Sonderwert für $m_0$ reserviert werden muss.
Des Weiteren wird der obengenannte Monoid mitgeführt, um auf die Länge der Unterbäume effektiv zugreifen zu können.
Für die Implementierung in T4gl werden andere Measure ignoriert, da diese nicht für das Laufzeitsystem relevant sind und die Implementierung unnötig erschweren würden.

= Generische Fingerbäume <sec:finger-tree:generic>
Im Folgenden wird betrachtet, inwiefern die Zweigfaktoren von Fingerbäumen generalisierbar sind, ohne die in @tbl:finger-tree-complex beschriebenen Komplexitäten zu verschlechtern.
Höhere Zweigfaktoren der Teilbäume eines Fingerbaums reduzieren die Tiefe des Baums und können die Cache-Effizienz erhöhen.

Wir beschreiben die Struktur eines Fingerbaums durch die Minima und Maxima seiner Zweigfaktoren $k$ (Zweigfaktor der internen Knoten) und $d$ (Anzahl der Digits auf jeder Seite einer Ebene)

$
  k_min &<= k &<= k_max \
  d_min &<= d &<= d_max \
$

Die Teilbäume, welche in den Digits liegen, werden auf B-Bäume generalisiert, daher gilt für deren interne Knoten außerdem $k_min = ceil(k_max / 2)$, sowie $k_min = 2$ für deren Wurzelknoten.

Die in @lst:finger-tree gegebene Definition lässt sich dadurch erweitern, das `std::array` durch `std::vector` ersetzt wird, um die wählbaren Zweigfaktoren zu ermöglichen.
Diese können ebenfalls mit ```cpp template``` Parametern zur Kompilierzeit gesetzt werden, um den Wechsel auf Vektoren zu vermeiden.
Die Definition von `Shallow` wird dadurch erweitert, dass diese bis zu $2d_min$ aufnimmt.

#figure(
  figures.finger-tree.def.new,
  caption: [Die Definition von generischen Fingerbäumen in C++.],
) <lst:gen-finger-tree>

Des weiteren betrachten wir für die Generalisierung lediglich Fingerbäume, welche für T4gl relevant sind.
Statt generischen Measures für verschiedene Anwendungsfälle wird die Schnittstelle der Operationen auf geordnete einzigartige Schlüssel spezialisiert, aus `measure` wird der `key`, gespeicherte größten Schlüssel der Unterbäume.
Kein Schlüssel kommt mehr als einmal vor.
Während bei der generischen Variante in @lst:finger-tree Schlüssel- und Wertetyp in `T` enthalten sein müssten, sind diese hier direkt in `Node` zu finden.

Die Operationen in den Abschnitten @sec:finger-tree:search[] bis @sec:finger-tree:insert-remove[] verwenden in den meisten Fällen mit `Node` Variablen als Eingabe oder Ausgabe.
Dabei müssen alle Eingaben bei initialem Aufruf `Leaf`-Knoten sein, um die korrekte Tiefe der Unterbäume zu wahren.
Gleichermaßen geben die initialen Aufrufe dieser Algorithmen immer `Leaf`-Knoten zurück.
In beiden Fällen ermöglicht die Verwendung von `Node` statt der Typen `K` und `V` die Definition der Algorithmen durch simple Rekursion.
In der Haskell-Definition ergibt sich zur Kompilierzeit aus durch den nicht-regulären Typen `FingerTree a` @bib:hp-06[S. 3] Funktionen, welche bei initialem Aufruf keine Knoten manuell ent- oder verpacken müssen.
Eine äquivalente C++-Implementierung kann diese als interne Funktionen implementieren, welche die Eingaben in Knoten verpacken und die Ausgaben entpacken.

Da Wirbelknoten mindestens $2d_min$ Digits enthalten, muss die Bildung dieser Minimalanzahl der Elemente im letzten Wirbelknoten überbrückt werden.
Das erzielt man durch das Einhängen von Sonderdigits in den letzten Wirbelknoten, bis genug für beide Seiten vorhanden sind.
Prinzipiell gilt für den letzten Wirbelknoten

$
  0 <= d < 2d_min
$

Ein generischer Fingerbaum ist dann durch diese Minima und Maxima beschrieben.
Im Beispiel des 2-3-Fingerbaums gilt

$
  2 &<= k &<= 3 \
  1 &<= d &<= 4 \
$

== Baumtiefe <sec:finger-tree:depth>
Sei $n(t)$ die Anzahl $n$ der Elemente einer Ebene $t$, also die Anzahl aller Elemente in allen Teilbäumen der Digits eines Wirbelknotens, so gibt es ebenfalls die minimale und maximale mögliche Anzahl für diesen Wirbelknoten.
Diese ergeben sich jeweils aus den minimalen und maximalen Anzahlen der Digits, welche Teilbäme mit minimal bzw. maximal belegten Knoten enthalten.

$
  n_"lmin" (t) &= k_min^(t - 1) \
  n_"imin" (t) &= 2 d_min k_min^(t - 1) \
  n_max (t) &= 2 d_max k_max^(t - 1) \
$

$n_"lmin"$ beschreibt das Minimum des letzten Wirbelknotens, da dieser nicht an die Untergrenze $2 d_min$ gebunden ist.
$n_"imin"$ ist das Minimum interner Wirbelknoten.
Für die kumulativen Minima und Maxima aller Ebenen bis zur Ebene $t$ ergibt sich

$
  n'_"lmin" (t) &= n_"lmin" (t) + n'_"lmin" (t - 1) \
  n'_"imin" (t) &= n_"imin" (t) + n_"imin" (t - 1) + dots.c + n_"imin" (1) &= sum_(i = 1)^t n_"imin" (i) \
  n'_max (t) &= n_max (t) + n_max (t - 1) + dots.c + n_max (1) &= sum_(i = 1)^t n_min (i) \
$

Das wirkliche Minimum eines Baums der Tiefe $t$ ist daher $n'_min (t) = n'_"lmin" (t)$, da es immer einen letzten nicht internen Wirbelknoten auf Tiefe $t$ gibt.
@fig:cum-depth zeigt die Minima und Maxima von $n$ für die Baumtiefen $t in [1, 8]$ für 2-3-Fingerbäume.
Dabei zeigen die horizontalen Linien das kumulative Minimum $n'_min$ und Maximum $n'_max$ pro Ebene.

#figure(
  figures.finger-tree.ranges,
  caption: [Die möglichen Tiefen $t$ für einen 2-3-Fingerbaum mit $n$ Blattknoten.],
) <fig:cum-depth>

== Über- & Unterlaufsicherheit <sec:over-under>
Über- bzw. Unterlauf einer Ebene $t$ ist der Wechsel von Digits zwischen der Ebene $t$ und der Ebene $t + 1$.
Beim Unterlauf der Ebene $t$ sind nicht genug Digits in $t$ enthalten.
Es wird ein Digit aus der Ebene $t + 1$ entnommen und dessen Kindknoten in $t$ gelegt.
Der Überlauf einer Ebene $t$ erfolgt, wenn in $t$ zu viele Digits enthalten sind.
Es werden genug Digits aus $t$ entnommen, sodass diese in einen Knoten verpackt und als Digit in $t + 1$ gelegt werden können.
Das Verpacken und Entpacken der Digits ist nötig, um die erwarteten Baumtiefen pro Ebene zu erhalten, sowie zur Reduzierung der Häufigkeit der Über- und Unterflüsse je tiefer der Baum wird.

Die Anzahl der Digits hat dabei einen direkten Einfluss auf die amortisierten Komplexitäten von `push`- und `pop`-Operationen.

#quote(block: true, attribution: [!Hinze und !Paterson @bib:hp-06[S. 7]])[
  We classify digits of two or three elements (which are isomorphic to elements of type Node a) as safe, and those of one or four elements as dangerous.
  A deque operation may only propagate to the next level from a dangerous digit, but in doing so it makes that digit safe, so that the next operation to reach that digit will not propagate.
  Thus, at most half of the operations descend one level, at most a quarter two levels, and so on.
  Consequently, in a sequence of operations, the average cost is constant.

  The same bounds hold in a persistent setting if subtrees are suspended using lazy evaluation.
  [...]
  Because of the above properties of safe and dangerous digits, by that time enough cheap shallow operations will have been performed to pay for this expensive evaluation. 
]

Die Analyse !Hinze und !Paterson stützt sich darauf, dass persistente 2-3-Fingerbäume deren Unterbäume durch Lazy Evaluation erst dann Bilden und auswerten, wenn eine weitere Operation bis in diese Tiefe vorgehen muss.
Für die Debit-Analyse von `push` und `pop` werden dabei den Unterbäumen so viele Credits zugewiesen wie diese sichere Knoten haben.
Diese Credits werden für die spätere Auswertung verwendet, sodass die amortisierten Kosten über den realen Kosten bleiben.
Damit sich die gleiche Analyse auf generische Fingerbäume übertragen lässt, müssen generische Fingerbäume ebenfalls bei jeder Operation auf einer unsicheren Ebene eine sichere Ebene zurücklassen.

Wir erweitern den Begriff der Sicherheit einer Ebene $t$ mit $d$ Digits als

#let dd = $Delta d$

$
  d_min < d_t < d_max
$

Ist eine Ebene sicher, ist das Hinzufügen oder Wegnehmen eines Elements trivial.
Ist eine Ebene unsicher, kommt es beim Hinzufügen oder Wegnehmen zum Über- oder Unterlauf, Elemente müssen zwischen Ebenen gewechselt werden, um die obengenannten Ungleichungen einzuhalten.
Erneut gilt zu beachten, dass die Sicherheit einer Ebene nur für eine Seite der Ebene gilt.
Welche Seite, hängt davon ab, an welcher Seite eine Operation wie `push` oder `pop` durchgeführt wird.
Wir betrachten den Über- und Unterlauf einer unsicheren Ebene $t$ in eine bzw. aus einer sicheren Ebene $t + 1$.
 Über- oder Unterlauf von unsicheren Ebenen in bzw. aus unsicheren Ebenen kann rekursiv angewendet werden bis eine sichere Ebene erreicht wird.
Ähnlich den 2-3-Fingerbäumen wird durch die Umwandlung von unsicheren in sichere Ebenen dafür gesorgt, dass nur jede zweite Operation eine Ebene in dem Baum herabsteigen muss, nur jede vierte zwei Ebenen, und so weiter.
Dieses Konzept nennt sich implizite rekursive Verlangsamung (_engl._ implicit recursive slowdown) @bib:oka-98 und ist Kernbestandteil der amortisierten Komplexität Deque-Operationen (`push`/`pop`).

Damit die Digits einer Ebene $t$ in eine andere Ebene $t + 1$ überlaufen können, müssen diese in einen Digit-Knoten der Ebene $t + 1$ passen, es gilt
$
  k_min <= dd <= k_max \
$ <eq:node-constraint>

Dabei ist $dd$ die Anzahl der Digits in $t$, welche in $t + 1$ überlaufen sollen.
Essentiell ist, dass eine unsichere Ebene nach dem Überlauf wieder sicher ist, dazu muss $dd$ folgende Ungleichungen einhalten.
Die Ebene $t + 1$, kann dabei sicher bleiben oder unsicher werden.
$
  t     &: d_min <& d_max     &text(#green, - dd) text(#red, + 1) &<  d_max \
  t + 1 &: d_min <& d_(t + 1) &text(#green, + 1)                  &<= d_max \
$ <eq:over>

Gleichermaßen gelten für den Unterlauf folgende Ungleichungen.
$
  t     &: d_min <&  d_min     &text(#green, + dd) text(#red, - 1) &< d_max \
  t + 1 &: d_min <=& d_(t + 1) &text(#green, - 1)                  &< d_max \
$ <eq:under>

Betrachtet man die Extrema von $dd$ in @eq:node-constraint, folgt aus deren Subtitution in den Gleichungen @eq:over[] und @eq:under[] eine Beziehung zwischen $d$ und $k$.

$
  d_max - d_min + 1 > k_max
$ <eq:constraint>

Für eine effiziente Implementierung von Deque-Operationen haben !Hinze und !Paterson die Zweigfaktoren der Digits von denen der Knoten um $plus.minus 1$ erweitert @bib:hp-06[S. 4].
Die Definitionen $d_min = k_min - 1$ und $d_max = k_max + 1$ halten nicht generell.
Werden diese in @eq:constraint eingesetzt, ergibt sich $k_min < 3$ und folglich $ceil(k_max \/ 2) < 3$.
Unter diesen Definitionen von $d$ in Abhängigkeit von $k$ können Zweigfaktoren ab $k_max > 7$ nicht dargestellt werden.
Wählt man stattdessen $d_min = ceil(k_min \/ 2)$ (siehe @sec:finger-tree:split-concat, @eq:constraint2), ergibt sich
$
  d_max - ceil(ceil(k_max / 2) / 2) + 1 > k_max
$

Die Zweigfaktoren $d_max$ und $k_max$ sind so zu wählen, dass Werte für $dd$ gefunden werden können, für die zuvorgenannten Ungleichungen halten.
Betrachten wir 2-3-Fingerbäume, gilt $d_min = 1$, $d_max = 4$, $k_min = 2$ und $k_max = 3$, daraus ergibt sich trivialerweise $4 > 3$.

!Hinze und !Paterson entschieden sich bei Überlauf für $dd = 3$, gaben aber an, dass $dd = 2$ ebenfalls funktioniert @bib:hp-06[S. 8].
Für generische Fingerbäume zeigt sich das in @eq:node-constraint.
Beim Unterlauf hängt $dd$ von dem Zustand der Datenstruktur ab und kann nicht frei gewählt werden.
Aus den oben genannten Ungleichungen lassen sich Fingerbäume mit anderen $d_max$ und $k_max$ wählen, welche die gleichen amortisierten Komplexitäten für Deque-Operationen aufweisen.

== Suche <sec:finger-tree:search>
Um ein gesuchtes Element in einem Fingerbaum zu finden, werden ähnlich wie bei B-Bäumen Hilfswerte verwendet, um die richtigen Teilbäume zu finden.
In Fingerbäumen sind das die sogenannten Measures.
Ein Unterschied zu B-Bäumen ist, wo diese Werte vorzufinden sind.
In B-Bäumen sind direkt in den internen Knoten bei $k$ Kindknoten $k - 1$ solcher Hilfswerte zu finden, bei Fingerbäumen sind Measures stattdessen einmal pro internem Knoten vorzufinden.
Des Weiteren sind die Measures in Fingerbäumen die Kombination der Measures ihrer Kindknoten.

Für die Anwendung in T4gl werden geordnete Sequenzen benötigt, sodass der Measure interner Knoten den größten Schlüssel des Unterbaums angibt.
Ist der Measure des Unterbaums kleiner als der Suchschlüssel, so ist der Schlüssel nicht in diesem Unterbaum enthalten.

#let dsearch = math-func("digit-search")

@alg:finger-tree:search zeigt, wie durch Measures ein Fingerbaum durchsucht werden kann.
Dabei ist $dsearch$ Binärsuche anhand der Measures über eine Sequenz von Digits, gefolgt von gewöhnlicher B-Baumsuche (nach @sec:b-tree) der gefundenen Digit.
Bevor aber $dsearch$ angewendet werden kann, muss der richtige Wirbelknoten gefunden werden.

#figure(
  kind: "algorithm",
  supplement: [Algorithmus],
  figures.finger-tree.alg.search,
  caption: [Die `search` Operation auf einem Fingerbaum.],
) <alg:finger-tree:search>

Das Zeitverhalten von @alg:finger-tree:search hängt von der Tiefe des Baums ab, da im worst-case der gesuchte Schlüssel im letzten Wirbelknoten vorzufinden ist.
Die Baumtiefe steigt logarithmisch mit der Anzahl der Elemente in Abhängigkeit von den Zweigfaktoren (siehe @sec:finger-tree:depth).
Je weiter ein Schlüssel von den beiden Enden entfernt ist, desto tiefer muss die Suche in den Baum vordringen.
Dabei ist ein Schlüssel, welcher $d$ Positionen vom nächsten Ende entfernt ist $Theta(log d)$ Ebenen tief im Baum vorzufinden @bib:hp-06[S. 5], also effektiv $Theta(log n)$.

== Push & Pop <sec:finger-tree:push-pop>
Kernbestandteil von Fingerbäumen sind `push` und `pop` an beiden Seiten mit amortisierter Zeitkomplexität von $Theta(1)$.
Die Algorithmen @alg:finger-tree:push-left[] und @alg:finger-tree:pop-left[] beschreiben die Operationen an der linken Seite eines Fingerbaums, durch die Symmetrie von Fingerbäumen lassen sich diese auf die rechte Seite übertragen.

#let Deep = math-type("Deep")
#let Shallow = math-type("Shallow")

#let None = math-type("None")

@alg:finger-tree:push-left ist in zwei Fälle getrennt, je nach der Art des Wirbelknotens $t$, in welchen der Knoten $e$ eingefügt werden soll.
Ist der Wirbelknoten $Shallow$, wird der Knoten $e$ direkt als linkes Digit angefügt, sollten dabei genug Digits vorhanden sein, um einen Wirbelknoten des Typs $Deep$ zu erstellen, wird dieser erstellt, ansonsten bleibt der Fingerbaum $Shallow$.
In beiden Fällen kommt es nicht zum Überlauf.
Sollte der Wirbelknoten $Deep$ sein, wird der neue Knoten $e$ als linkes Digit angefügt.
Wird die maximale Anzahl der Digits $d_max$ überschritten, kommt es zum Überlauf.
Beim Überlauf werden $dd$ Digits in einen Knoten verpackt und in die nächste Ebene verschoben.
Bei `push` Operationen muss außerdem beachtet werden, dass der Schlüssel des eingefügten Knotens die Ordnung der Schlüssel einhält.
In der Implementierung kann diese Operation entweder als `private` markiert werden oder die Knoten vor dem Einfügen validieren.

#figure(
  kind: "algorithm",
  supplement: [Algorithmus],
  figures.finger-tree.alg.pushl,
  caption: [Die `push`-Operation an der linken Seite eines Fingerbaumes.],
) <alg:finger-tree:push-left>

@alg:finger-tree:pop-left ist auf ähnliche Weise wie @alg:finger-tree:push-left in zwei Fälle getrennt, $Shallow$ und $Deep$.
Ist der Wirbelknoten $Shallow$, wird lediglich ein Digit von links abgetrennt und zurückgegeben.
Bei null Digits wird der Wert $None$ zurückgegeben, in einer Spache wie C++ könnte das durch einen ```cpp nullptr``` oder das Werfen einer Exception umgesetzt werden.
Bei einem $Deep$ Wirbelknoten wird eine linkes Digit entfernt.
Bleiben dabei weniger als $d_min$ Digits vorhanden, erzeugt das entweder Unterlauf oder den Übergang zum $Shallow$ Fingerbaum.
Ist die nächste Ebene selbst $Shallow$ und leer, werden die linken und rechten Digits zusammengenommen und diese Ebene selbst wird $Shallow$.
Ist die nächste Ebene nicht leer, wird ein Knoten entnommen und dessen Kindknoten füllen die linken Digits auf.

Sowohl `push` als auch `pop` gehen nach dem gleichen rekursiven Über/Unterfluss-Prinzip vor und hängen daher von der Baumtiefe ab, im worst-case ist jede Ebene unsicher und sorgt für einen Über- oder Unterfluss, daher ergibt sich eine Komplexität von $Theta(log n)$.
Die amortisierte Komplexität stützt sich auf die gleiche Analyse wie diese von 2-3-Fingerbäumen, insofern Unterbäume mit Lazy Evaluation aufgeschoben werden und korrekte Zweigfaktoren gewählt wurden, können auch generische Fingerbüme `push` und `pop` in amortisiert $Theta(1)$ ausführen.

#figure(
  kind: "algorithm",
  supplement: [Algorithmus],
  figures.finger-tree.alg.popl,
  caption: [Die `pop`-Operation an der linken Seite eines Fingerbaumes.],
) <alg:finger-tree:pop-left>

Außerdem definieren wir `append` und `take` als wiederholte Versionen von `push` und `pop`.
Diese sind gleicherweise symmetrisch für die rechte Seite zu definieren.
Die Operation `take` nimmt dabei soviele Elemente aus $t$ wie möglich, aber nicht mehr als $n$.

#figure(
  kind: "algorithm",
  supplement: [Algorithmus],
  figures.finger-tree.alg.appendl,
  caption: [Die `append`-Operation an der linken Seite eines Fingerbaumes.],
) <alg:finger-tree:append-left>

Die worst-case Komplexität von @alg:finger-tree:concat ergibt sich aus der Anzahl der Elemente im Baum $n$ und derer in der Sequenz $"nodes"$ @alg:finger-tree:push-left als $Theta(abs("nodes") log n)$.

#figure(
  kind: "algorithm",
  supplement: [Algorithmus],
  figures.finger-tree.alg.takel,
  caption: [Die `take`-Operation an der linken Seite eines Fingerbaumes.],
) <alg:finger-tree:take-left>

Da @alg:finger-tree:take-left nur so viele Elemente entfernen kann wie im Baum vorhanden sind, ist der Faktor das Minimum aus der Anzahl der Elemente $n$ und dem Argument $"count"$, die Komplexität von `take` ist demnach $Theta(min("count", n) log n)$.

== Split & Concat <sec:finger-tree:split-concat>
#let Shallow = math-type("Shallow")

Das Spalten und Zusammenführen zweier Fingerbäume sind fundamentale Operationen, welche vor allem für die Implementierung von `insert` und `remove` in @sec:finger-tree:insert-remove relevant sind.
@alg:finger-tree:concat beschreibt, wie zwei Fingerbäume zusammengeführt werden, dabei muss in jeder Ebene eine Hilfssequenz $m$ übergeben werden, welche die Zusammenführung der inneren Digits der Bäume beschreibt, nachdem deren Kindknoten entpackt wurden.
Beim initalen Aufruf wird die leere Sequenz $nothing$ übergeben.
@alg:finger-tree:concat ruft sich selbst rekursiv auf, bis einer der Bäume $Shallow$ ist, in diesem  Fall degeneriert der Algorithmus zu wiederholtem `push` und terminiert.
Ähnlich wie bei @alg:finger-tree:push-left muss beachtet werden, dass die Zusammenführung der zwei Bäume die Ordnung der Schlüssel einhält.

#figure(
  kind: "algorithm",
  supplement: [Algorithmus],
  figures.finger-tree.alg.concat,
  caption: [Das Zusammenfügen zweier Fingerbäume zu einem.],
) <alg:finger-tree:concat>

Der Hilfsalgortihmus @alg:finger-tree:nodes[] verpackt Knoten für die Rekursion in die nächste Ebene in @alg:finger-tree:concat.
Die Größe der zurückgegebenen Sequenz von verpackten Knoten hat dabei direkten Einfluss auf die eigene Komplexität (durch den rekursiven Aufruf in @alg:finger-tree:concat) und auf die Komplexität der nicht rekursiven Basisfälle in @alg:finger-tree:concat.

#figure(
  kind: "algorithm",
  supplement: [Algorithmus],
  figures.finger-tree.alg.nodes,
  caption: [Ein Hilfsalgorithmus zum Verpacken von Knoten.],
) <alg:finger-tree:nodes>

Bei 2-3-Fingerbäumen lässt sich zeigen, dass die mittlere Sequenz $m$ eine Obergrenze von 4 und eine Untergrenze von 1 hat.
Zunächst betrachtet man, wie das Verpacken der Knoten sich auf deren Anzahl auswirkt.

Sei $m_t$ die Anzahl der verpackten Knoten bei Rekursionstiefe $t$, gilt

$
  n_t &= k_t + m_(t - 1) \
  m_t &= cases(
          &n_t \/ k_max            &    &bold("wenn") n_t mod k_max = 0,
    floor(&n_t \/ k_max)           &+ 1 &bold("wenn") n_t mod k_max >= k_min,
    floor(&(n_t - k_min) \/ k_max) &+ 2 &bold("wenn") n_t mod k_max < k_min,
  )
$ <eq:nodes>

dabei ist $k_t$ die Summe der Knoten links und rechts und $m_(t - 1)$ die Größe der Sequenz aus der vorherigen Tiefe.
Beim initialen Aufruf ist die Sequenz leer, daher gilt $m_0 = 0$.

Für 2-3-Fingerbäume verhält sich die Größe der mittleren Sequenz wie folgt.
Auf jeder Ebene können zwischen $2d_min = 2$ und $2d_max = 8$ Elemente zu den verpackten Elementen der vorherigen Ebene hinzukommen.
#figure(
  table(columns: 3, align: right,
    table.header[$t$][$min n_t => min m_t$][$max n_t => max m_t$],
    $0$, $0 => 0$, $0 => 0$,
    $1$, $2 + 0 => 1$, $8 + 0 => 3$,
    $2$, $2 + 1 => 1$, $8 + 3 => 4$,
    $3$, text(gray)[$2 + 1 => 1$], $8 + 4 => 4$,
    $4$, text(gray)[$2 + 1 => 1$], text(gray)[$8 + 4 => 4$],
    [...], [ad infinitum], [ad infinitum],
  ),
  caption: [
    Beim Verpacken der Knoten ergibt sich eine Obergrenze für die Anzahl der verpackten Knoten.
  ],
) <tbl:nodes-term>

Damit $n_t$ Knoten verpackt werden können, muss die übergebene Sequenz mindestens $k_min$ Knoten enthalten.
Bei 2-3-Fingerbäumen reicht $k_min = 2d_min$ aus, um in jeder Ebene beim Zusammenführen der inneren Digits mindestens einen Knoten bilden zu können.
@tbl:nodes-term zeigt, dass die Länge der mittleren Sequenz, je nach Belegung der inneren Digits, zwischen 1 und 4 liegt.
Im generischen Fall muss aus $2d_min$ mindestens ein Knoten gebildet werden.
Daraus folgt

$
  k_min <= 2d_min
$ <eq:constraint2>

Andernfalls können Fingerbäume nicht zusammengefügt werden.
Experimentelle Ergebnisse deuten darauf hin, dass bei $k_max > 5$ and $d_max = k_max + 1$ eine Obergrenze ab $m_2 = 3$ existiert, aber ein Beweis konnte nicht formuliert werden.
Ohne einen Beweis für die Obergrenze dieser Sequenz kann nicht nachgewiesen werden, dass @alg:finger-tree:concat generell in logarithmischer Zeit läuft, da über diese Sequenz im Basisfall der Rekursion iteriert wird.
Es ist unklar, ob die Sequenz, welche durch @eq:nodes erzeugt wird, eine obere asymptotische Grenze von $Omicron(1)$ hat.
Stellt sich heraus, dass diese Sequenz sich in die Größenordnung von $Theta(n)$ bewegt, tut das auch die Komplexität von @alg:finger-tree:concat.
Wird allerdings ein $t$ gefunden, ab welchem diese Sequenz für gewählte Zweigfaktoren nicht weiter ansteigt, ist der Fingerbaum valide.

Einen Fingerbaum zu teilen ist essentiell, um einzelne Elemente mit bestimmten Eigenschaften zu isolieren, wie es für `insert` und `remove` der Fall ist.
@alg:finger-tree:split zeigt, wie ein Fingerbaum $t$ so geteilt wird, dass alle Schlüssel, welche größer als $k$ sind, im rechten Baum und alle anderen im linken Baum landen.
Das folgt daraus, dass jeder Schlüssel nur einmal im Baum vorkommen darf und alle Schlüssel sortiert sind.
Die Implementierung in @bib:hp-06 kann weder garantieren, dass die zurückgegebene Teilung die einzige, noch dass diese die erste Teilung ist, da diese nicht von monoton ansteigenden Schlüsseln ausgehen kann.

#figure(
  kind: "algorithm",
  supplement: [Algorithmus],
  figures.finger-tree.alg.split,
  caption: [Die Teilung eines Fingerbaumes um einen Schlüssel.],
) <alg:finger-tree:split>

== Insert & Remove <sec:finger-tree:insert-remove>
!Hinze und !Paterson entschieden sich, `insert` und `remove` für geordnete Sequenzen durch `split`, `push` und `concat` zu implementieren.

Da in @sec:finger-tree:split-concat kein Beweis vorliegt, dass @alg:finger-tree:concat in logarithmischer Komplexität läuft, können `insert` und `remove` ebenfalls nicht nachweislich auf diese Art mit logarithmischer Laufzeit implementiert werden.
Unter der Annahme, dass so ein Beweis möglich ist, könnten `insert` und `remove` wie folgt implementiert werden.
@fig:finger-tree:insert zeigt die Implementierung von `insert`.
Dabei ist zu beachten, dass wenn ein Schlüssel bereits in $t$ existiert, dessen Wert ersetzt werden muss, da jeder Schlüssel nur maximal einmal vorhanden sein darf.

#figure(
  kind: "algorithm",
  supplement: [Algorithmus],
  figures.finger-tree.alg.insert,
  caption: [`insert` als Folge von `split`, `push` und `concat`.],
) <fig:finger-tree:insert>

In @fig:finger-tree:remove ist zu sehen, wie `remove` zu implementieren ist.
Wie auch bei Insert wird geprüft, ob der erwünschte Schlüssel im linken Baum ist.
Ähnlich der Implementierung von Algorithmus @alg:finger-tree:search[] und @alg:finger-tree:pop-left[] muss bei Nichtvorhandensein des Schlüssels $k$ ein $None$-Wert oder eine Fehler zurrückgegeben werden.

#figure(
  kind: "algorithm",
  supplement: [Algorithmus],
  figures.finger-tree.alg.remove,
  caption: [`remove` als Folge von `split` und `concat`.],
) <fig:finger-tree:remove>

Alternative Implementierungen wurden nicht untersucht, aber je nach Besetzung der Unterbäume können `insert` und `remove` auch diese Lücken ausnutzen um statt `split` und `concat` auf simple Pfadkopie zurückzugreifen.
Die Häufigkeit der Lücken hängt dabei von der Wahl von $dd$ ab.
Liegt $dd$ näher an $k_min$, können Blattknoten öfter im Nachhinein befüllt werden.

== Echtzeitanalyse
Die Einhaltung verschiedener Ungleichungen in @sec:over-under hat vor allem Einfluss auf die amortisierten Komplexitäten der Deque-Operationen.
Das amortisierte Zeitverhalten hat für die Echtzeitanalyse keinen Belang.
Allerdings kann ohne einen Beweis in @sec:finger-tree:split-concat keine Aussage darüber gemacht werden, ob die Operationen bei generischen Zweigfaktoren in ihren Komplexitätsklassen bleiben oder sogar schlechteres Verhalten als persistente B-Bäume vorweisen.

Sollte ein Beweis für logarihmische Laufzeit von @alg:finger-tree:concat gefunden werden, können Fingerbäume exzellentes Zeitverhalten im Vergleich zu gewöhnlichen CoW-Datenstrukturen ohne granulare Persistenz vorweisen.
Je nach Anforderungen des Systems könnten für verschiedene Operationen zulässige Höchstlaufzeiten festgelegt werden.
Granular persistente Baumstrukturen wie Fingerbäume könnten prinzipell mehr Elemente enthalten, bevor diese Höchstlaufzeiten pro Operationen erreicht werden.
Aus Sicht der Echtzeitanforderungen an die Operationen auf der Datenstruktur selbst, ist jede Datenstruktur mit logarithmischem Zeitverhalten vorzuziehen.
Die Wahl von Fingerbäumen über B-Bäumen ist daher eher eine Optimierung als eine Notwendigkeit.

Ohne den zuvorgenannten Beweis für die Obergrenze der inneren Sequenz bei `concat` sind jedoch entweder 2-3-Fingerbäume oder B-Bäume generischen Fingerbäumen vorzuziehen.

= SRB-Bäume
RRB-Bäume (_engl._ relaxed radix balanced trees) sind beinahe perfekt balancierte Suchbäume, welche für Sequenzdatenstrukturen wie Vektoren verwendet werden und wurden von !Bagwell _et al._ eingeführt @bib:br-11 @bib:brsu-15 @bib:stu-15.
Um das Element an einem bestimmten Index zu finden, wird bei perfekter Balancierung eines Knotens der Teilindex direkt errechnet.
Bei nicht perfekter Balancierung stützt sich die Suche auf Längeninformationen der Kindknoten.
Dazu enthält jeder interne Knoten ein Array der kummulativen Anzahlen von Werten in dessen Kindknoten.
Dabei wird nach Indizes gesucht, sprich, fortlaufende Ganzzahlwerte zwischen 0 und der Anzahl der Elemente $n$ in der Sequenz.
Im Folgenden wird eine Erweiterung von RRB-Bäumen vorgestellt, SRB-Bäume (_engl._ sparse radix balanced trees), welche für nicht fortlaufende Schlüssel verwendet werden können.
Ein SRB-Baum ist aufgebaut wie ein perfekt balancierter voll belegter RRB-Baum über alle Indizes, welche vom Schlüsseltyp dargestellt werden können, aber ohne die Knoten, die nicht belegt sind.
Im Gegensatz zu RRB-Bäumen können Teilindizes auf jeder Ebene direkt errechnet werden, ohne dass die Länge der Kindknoten bekannt sein muss.
Da aber nicht alle Knoten besetzt sind, muss auf jeder Ebene geprüft werden, ob der errechnete Teilindex auf einen besetzten Knoten zeigt.
Durch die perfekte Balancierung ist die Baumtiefe $d$ nur von der Schlüsselbreite $b$ (die Anzahl der Bits im Schlüssel) und dem Zweigfaktor $k$ abhängig.

$
  d = log_k 2^b
$ <eq:srb-tree:depth>

@fig:srb-tree zeigt einen SRB-Baum über die Sequenz `[0, 16, 65296, 65519]` für `uint16` Schlüssel und mit einem Zweigfaktor von 16.
Die belegten Knoten sind in türkis #cbox(teal.lighten(50%)) hervorgehoben.

#figure(
  figures.srb-tree,
  caption: [Ein SRB-Baum für die 16 bit Schlüssel und einen Zweigfaktor von 16.],
) <fig:srb-tree>

== Schlüsseltypen
Durch die spezifische Art der Suche können nur Schlüsseltypen verwendet werden, welche anhand von Radixsuche Teilindizes errechnen lassen.
Das beschränkt sich generell auf nicht vorzeichenbehaftete Ganzzahltypen.
Für diese Schlüssel $u$ ergibt sich ein Teilindex $i$ auf Ebene $t$ wie folgt:
$
  u / k^(d - t) mod k
$

Bei Zweigfaktoren $k = 2^l$ ergibt sich eine optimierte Rechnung, welche verschiedene Bitshifting Operationen verwendet:
$
  (u >> (l dot.op (d - t))) \& (k - 1)
$

Sollte für einen Schlüsseltyp $T$ eine bijektive Abbildung $f$ und deren Inverses $f^(-1)$ existieren und es gilt
$
  forall t_1, t_2 in T : t_1 < t_2 <=> f(t_1) < f(t_2)
$

dann kann durch diese Abbildungen auch der Schlüsseltyp $T$ durch einen SRB-Baum verwaltet werden.
Ein simples Beispiel sind vorzeichenbehaftete Schlüssel, welche einfach um ihr Minimum verschoben werden, so dass alle möglichen Werte vorzeichenunbehaftet sind.
Das setzt natürlich voraus, dass ein Minimum existiert, wie es bei Ganzzahlräpresentationen in den meisten Prozessoren der Fall ist.
Ein Minimum und Maximum für die Schlüssel dieser Bäume bedeutet auch, dass die maximale Tiefe vor der Laufzeit bekannt ist.

== Zeitverhalten
Um zur Laufzeit den Blattknoten zu finden, in welchem sich ein Schlüssel befindet, wird pro Ebene per Radixsuche der Index dieser Ebene errechnet und geprüft ob dieser Index besetzt ist.
Ist der Index nicht besetzt, endet die Suche, ansonsten fährt diese fort, bis der Blattknoten erreicht ist.
Durch die perfekte Balancierung ist die Baumtiefe nur von der Schlüsselbreite abhängig, nicht von der Anzahl der gespeicherten Elemente.
Operationen wie `insert`, `remove` oder `search` haben daher eine Zeitkomplexität von $Theta(d)$ mit $d$ aus @eq:srb-tree:depth.

Gerade unter Echzeitbedingungen erleichtert das die Analyse enorm, durch die Wahl von Höchstschlüsselbreiten kann SRB-Bäumen konstantes worst-case Zeitverhalten für alle Operationen garantiert werden.
Diese Schlüsselbreiten sind in den meisten Fällen von der Hardware vorgegeben und könnn in den üblichen Größen von 8, 16, 32, 64 und in experimentellen CPUs bis zu 128.

== Speicherauslastung
Seien $v$ der benötigte Speicher eines Werts und $p$ der benötigte Speicher eines Pointers in einem SRB-Baum.
Wir definieren die Speicherauslastung $s_U$ als das Verhältnis zwischen den belegten Speicherplätzen $s_S$ und den gesamten Speicherplätzen $s_A$, welche von der Datenstruktur angelegt wurden.

$
  s_U (p, v) = (s_S (p, v)) / (s_A (p, v)) \
$

Im Idealfall liegt $s_U$ nahe 1, sprich 100%.
Der in @fig:srb-tree gegebene SRB-Baum verbraucht pro Blattknoten $16 s_v$ Speicherplatz, ungeachtet der Belegung.
Interne Knoten benötigen $16 s_p$ Speicherplatz.
Bei ungünstiger Belegung, zum Beispiel einem Element pro möglichem Blattknoten, ergibt sich ein Speicherplatzverbrauch von $16^4 s_v + 16^3 s_p + 16^2 s_p + 16 s_p$ bei nur $16^3$ belegten Elementen.
Bei Pointern mit $p = 8 "Byte"$ und Pointern als Werten ($v = p$) folgt eine Speicherauslastung von

$
  s_U (p, v) &= (16^3 v) / (16^4 v + 16^3 p + 16^2 p + 16 p) \
  &= 0.058... \
  &approx 6% \
$

Dabei sind Verwaltungsstrukturen ignoriert, welche die Belegung der Elemente enthalten, da diese verhältnismäßig klein sind.
Für einen SRB-Baum mit einem Zweigfaktor $k$ und einer Schlüsselbreite $b$ und der sich daraus ergebenden Baumtiefe $d = ceil(log_k 2^b)$, ergibt sich eine Speicherauslastung im worst-case von:

$
  s_U (p, v) = (k^(d - 1) v) / (k^d v + sum_(n = 1)^(d - 1) k^n p)
$

Gerade wegen dieser potentiell hohen Speicherverschwendung und der Einschänkung auf die Schlüsseltypen wurden SRB-Bäume als Implementierung für T4gl-Arrays nicht weiter verfolgt.
