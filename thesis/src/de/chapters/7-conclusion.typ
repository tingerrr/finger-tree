#import "/src/util.typ": *
#import "/src/figures.typ"

= Ergebnis
Aus den Benchmarks in @chap:benchmarks kann geschlossen werden, dass persistente Baumdatenstrukturen vor allem im Worst Case bessere Performance liefern können.
In ihrer jetzigen Implementierung sind 2-3-Fingerbäume keine gute Wahl für die Storage-Datenstruktur von T4gl-Arrays.
Eine simple B-Baum Implementierung ohne Optimierung war in der Lage in den untersuchten Szenarios vergleichbare oder bessere Performance als QMaps zu bieten.
Unter Betrachtung weiterer Szenarien wird davon ausgegangen, dass dieser Trend fortbesteht und das worst-case Zeitverhalten von T4gl-Arrays drastisch verbessern kann.

= Optimierungen
Verschiedene Optimierungen können die Performance der 2-3-Fingerbaum-Implementierung verbessern.
Allerdings ist unklar, ob diese ausreichen, um die Performance der persistenten B-Baum-Implementierung zu erreichen, welche ähnlich unoptimiert implementiert wurde.

== Pfadkopie
Die Implementierung von `insert` der 2-3-Fingerbäume stützt sich auf eine simple, aber langsame Abfolge von `split`, `push` und `concat`.
Es ist allerdings möglich stattdessen eine Variante mit Pfadkopie und internem Überlauf zu implementieren.
Bei Einfügen eines Blattknotens in einer unsicheren Ebene, kann das maximal zu einem neuen Knoten pro Ebene führen, ähnlich dem worst-case von `push`.

== Lazy-Evaluation
Das Aufschieben von Operationen durch Lazy Evaluation hat einen direkten Einfluss auf die amortisierten Komplexitäten der Deque-Operationen @bib:hp-06[S. 7].
Da für die Echtzeitanalyse der Datenstruktur nur die worst-case Komplexitäten relevant sind, wurde diese allerdings vernachlässigt.

Zur generellen Verbesserung der durschnittlichen Komplexitäten der Implementierung ist die Verwendung von Lazy Evaluation unabdingbar.

== Generalisierung & Cache-Effizienz
Der Cache einer CPU ist ein kleiner Speicher, zwischen CPU und RAM, welcher generell schneller zu lesen und schreiben ist.
Ist ein Wert nicht in dem CPU-Cache, wird in den meisten Fällen beim Lesen einer Adresse im RAM der umliegende Speicher mit in den Cache gelesen.
Das ist einer der Gründe, warum Arrays als besonders schnelle Datenstrukturen gelten.
Wiederholte Lese- und Schreibzugriffe im gleichen Speicherbereich können häufig auf den Cache zurückgreifen.
In Präsenz von Indirektion, also der Verwendung von Pointern wie bei Baumstrukturen, können Lese- und Schreibzugriffe in den Speicher öfter auf Bereiche zeigen, welche nicht in dem Cache liegen, dabei spricht man von einem Cache-Miss.

@sec:finger-tree:generic beschreibt einen Versuch die Cache-Effizienz von Fingerbäumen zu erhöhen, indem durch höhere Zweigfaktoren die Tiefe der Bäume reduziert wird.
Durch die geringere Tiefe sollen die rekursiven Algorithmen welche den Baumknoten folgen weniger oft Cache-Misses verursachen.

Für verschiedene Teile der generalisierten Zweigfaktoren von Fingerbäumen konnten keine Beweise vorgelegt werden.
Es wurden allerdings auch keine Beweise gefunden oder erarbeitet, welche die generalisierung auf höhere Zweigfaktoren gänzlich ausschließen.
Je nach Stand der Beweise könnten generalisierte Varianten von Fingerbäumen in Zukunft in T4gl eingesetzt werden.
Unklar ist, ob der Aufwand der Generalisierung sich mit der verbesserten Cache-Effizienz aufwiegen lässt.

Die schlechten Ergebnisse der 2-3-Fingerbaum scheinen eine direkte Folge der naiven Implementierung zu sein, da die in @bib:hp-06[S. 20] gegebenen Benchmarks exzellente Performance vorweisen.
Dabei ist allerdings unklar, wie stark der Einfluss von Lazy Evaluation in Haskell sich auf die Ergebnisse der Benchmarks auswirkt.

== Vererbung & Virtual Dispatch
Wird eine Klasse in C++ vererbt und besitzt überschreibbare Methoden, gelten diese als virtuell.
Hat eine vererbare Klasse eine Methode ohne eine Implementierung, gilt die Methode, sowie die Klasse selbst als abstrakt.
Virtuelle Methoden müssen bei ihrem Aufruf zur Laufzeit zunächst die richtige Implementierung der Methode im Virtual Table finden.
Das erfolgt durch eine sogenannte Virtual Table, auf welchen jede abstrakte Klasse und deren erbende Klassen verweisen.
Dabei werden für die CPU wichtige Optimierungen erschwert, wie Branch Prediction, Instruction Caching oder Instruction Prefetching.

Die in @lst:finger-tree gegebene Definition verwendet Vererbung der Klasse `FingerTree` zur Darstellung der verschiedenen Varianten.
Daraus folgt, dass Fingerbäume nicht mehr direkt verwendet werden können, eine `FingerTree`-Instanz selbst ist nutzlos ohne die Felder und Implementierung der vererbenden Varianten.
Instanzen von `FingerTree` müssen durch Indirektion übergeben werden, da diese generell auf deren erbende Variante verweisen.
Die Operationen auf den verschiedenen Varianten von `FingerTree` müssten entweder durch vorsichtiges casten der Pointer oder durch einheitliche API anhand virtueller Methoden erfolgen.
Ersteres ist unergonomisch und fehleranfällig, `FingerTree` wird zwangsläufig zur virtuellen Klasse, daraus folgt:
- dass für viele Methoden Virtual Dispatch verwendet werden muss
- und dass jeder Zugriff auf einen `FingerTree` zunächst die Indirektion auflösen muss (Pointerdereferenzierung).
Zweiteres ist nicht für alle Operationen sinnvoll, manche Operationen wie `split` sind nur für eine Variante sinnvoll implementiert.

Um zu vermeiden, dass jeder Aufruf essentieller Funktionen wie `pop` und `pop` auf Virtual Dispatch zurückgreifen muss, können statt virtuellen Methoden durch gezieltes casten auf die korrekte Variante Cache Misses vermieden werden.
Die Auswahl der Klasse kann durch das Mitführen eines Diskriminators erfolgen, welcher angibt, auf welche Variante verwiesen wird.
Damit wird sowohl die Existenz des Virtual Table Pointers in allen Instanzen, sowie auch die doppelte Indirektion dadurch vermieden.
Besonders häufige Pfade, wie die der `Deep`-Variante, können dem CPU als heiß vorgeschlagen werden, um diese bei der Branch Prediction zu bevorzugen.

== Memory-Layout
Bei C++ hat jeder Datentyp für den Speicher zwei relevante Metriken, dessen Größe und dessen Alignment.
Das Alignment eines Datentyps gibt an, auf welchen Adressen im Speicher ein Wert gelegt werden darf.
Ist das Alignment eines Typs `T` 8, kann dieser nur auf die Adressen gelegt werden, welche Vielfache von 8 sind, also `0x0`, `0x8`, `0x10`, `0x18` und so weiter.
Da komplexe Datentypen aus anderen Datentypen bestehen, müssen auch diese im Speicher korrekt angelegt werden, deren Alignment wirkt sich auf das des komplexen Datentyps aus.
Des Weiteren werden Felder in Deklarationreihenfolge angelegt.
Damit die Alignments der einzelnen Feldertypen eingehalten werden, werden wenn nötig vom Compiler unbenutze Bytes zwischen Feldern eingefügt.
Das nennt sich Padding.
Durch clevere Sortierung der Felder können Paddingbytes durch kleinere Felder gefüllt werden.
Padding zu reduzieren, reduziert die Größe des komplexen Datentyps.
Reduziert man die Größe des Datentyps, erhöht man die Anzahl der Elemente, welche von der CPU in deren Cache geladen werden können.

== Spezialisierte Allokatoren
Eine mögliche Darstellung von Graphen ist es, Knoten in Arrays zu speichern und deren Verbindungen in Adjazenzlisten zu speichern.
Dadurch können mehr Knoten in den CPU-Cache geladen werden als durch rekursive Definitionen.
Da Bäume lediglich Sonderformen von Graphen sind, kann das auch auf die meisten Baumdatenstrukturen angewendet werden.
Das bedeutet aber auch, dass alle Knoten kopiert werden müssten, welche von einem Graph erwaltet werden wenn dieser kopiert wird.
Das steht gegen das Konzept der Pfadkopie in persistenten Bäumen.

Eine Alternative, welche die Knoten eines Baums nah beieinander im Speicher anlegen könnten ohne die Struktur der Bäume zu zerstören, sind Allokatoren, welche einen kleinen Speicherbereich für die Knoten der Bäume verwalten.
Somit könnte die Cache-Effizienz von 2-3-Fingerbäumen erhöht werden, ohne besonders große Änderungen an deren Implementierung vorzunehmen.

== Unsichtbare Persistenz <sec:invis-pers>
Da die Persistenz von 2-3-Fingerbäumen durch die API der Klassen versteckt wird, können diese auch auf Persistenz verzichten, wenn es sich nicht auf andere Instanzen auswirken kann.
Wenn eine Instanz des Typs `T` der einzige Referent auf die in `_repr` verwiesene Instanz ist, kann diese problemlos direkt in diese Instanz schreiben, ohne vorher eine Kopie anzufertigen.
Das ist das Kernprinzip von vielen Datenstrukuren, welche auf CoW-Persistenz basieren.
Dazu gehören auch die Qt-Datenstrukturen, welche ursprünglich in T4gl zum Einsatz kamen.

Das hat allerdings einen Einfluss auf die Art, auf welche eine solche Klasse verwendet werden kann.
Wird eine Instanz `t1` angelegt und eine Referenz oder ein Pointer `&t1` wird an einen anderen Thread übergeben, kann zwischen dem Abgleich von `_repr.use_count() == 1` und der direkten Beschreibung der Instanz in `*_repr` eine Kopie von `t1` auf dem anderen Thread angeleg werden.
Sprich, der `use_count` kann sich zwischen dem Ablgleich und dem Schreibzugriff verändern.
Das hat zur Folge, dass der Schreibzugriff in beiden Instanzen `t1` und `t2` sichtbar wird, obwohl der Schreibzugriff nach der Kopie von `t1` stattfand.
Um auszunutzen zu können, dass eine Instanz der einzige Referent der Daten in `*_repr` ist, dürfen keine Referenzen oder Pointer zu Instanzen von `T` an andere Threads übergeben werden.
Stattdessen sollten diese Instanzen im Ursprungsthread kopiert werden, bevor sie an einen anderen Thread übergeben werden.
