#import "/src/util.typ": *
#import "/src/figures.typ"

Da keine eindeutigen Beweise gefunden werden konnten, welche eine sichere API der C++-Implementierung generischer Fingerbäume auf Typ-Ebene umsetzten können, wurden 2-3-Fingerbäume in C++ übersetzt um deren Verhlaten mit dem von QMap zu vergleichen.
Auf eine Implementierung von SRB-Bäumen wird Anhand des Speicherverbrauchs verzichtet.

= Von Haskell zu C++
Die Implementierung von 2-3-Fingerbäumen, sowie deren Benchmarks mit Tests von `QMap` und einer persistenten B-Baum-Implementierung zur Kontrolle können unter #link("https://github.com/tingerrr/finger-tree")[GitHub:tingerrr/FingerTree] eingesehen werden.
Verweise auf die Implementierung beziehen sich auf den letzten Stand dieses Repositories, welches mit Abgabe dieser Arbeit archiviert wird.

Die ursprüngliche Implementierung von 2-3-Fingerbäumen hat drei verschiedene Zustände, welche verschiedene Invarianzen sichern:
- `Empty`: Ein leerer Zustand ohne Felder.
- `Single`: Ein Überbrückungszustand zum Speichern eines einzelnen Knotens in einer Ebene, dieser hat einen einzelnen Knoten als Feld.
- `Deep`: Die rekursive Definition der Datenstruktur.
  Dieses hat drei Felder, die Digits auf beiden Seiten und einen weiteren Fingerbaum mit nicht-regulärer Rekursion auf Typebene @bib:bm-98.
  Die Definition von `FingerTree a` verwendet nicht erneut `FingerTree a`, sondern `FingerTree (Node a)` (analog zu `FingerTree<Node<T>>` statt `FingerTree<T>`), daraus wird, zusammen mit der Definition on `Node` die Tiefe der Knoten auf jeder Ebene automatisch durch den Typ gesichert.

Ohne die Typrekursion in der `Deep`-Variante würde sich ein Fingerbaum wie eine einfach verkettete Liste verhalten, die Rekursion ist essentiell für die Performance der Datenstruktur.
In C++ ist die Definition von rekursiven Typen möglich, diese Definitionen müssen aber regulär erfolgen und wenn es um die Definition von den Feldern einer Datenstruktur handelt müssen Selbstverweise durch Indirektion erfolgen.
Eine Klasse `C<T>` kann in der eigenen Definition auf folgende Versionen von `C<T>` zurückgreifen:
- Voll-instanzierte Typen wie `C<int>` oder `C<std::string>`.
- Neu parametrierte Typkonstruktoren wie ```cpp template<typename U> class C<U>```, sofern diese nicht von `T` abhängen.
- Eigenverweise wie `C<T>` ohne _Verpacken_ von Typen wie `C<A<T>>`.

Die Definition in @lst:illegal-recursive-type wäre eine 1-1 Übersetzung der Haskell-Definition (Initialdefinition ohne Measures), diese wäre nach den oben genannten Regeln in C++ illegal.
Zunächst müssen Eigenverweise durch Indirektion erfolgen, das Feld `middle` in `Deep` müsste also ein Pointer-Typ sein.
Allerdings ist der Eigenverweis dann immernoch nicht möglich, da `Deep` auf den Typen `FingerTree<Node<T>>` verweist --- ein nicht-regulärer Eigenverweis.
Die Balancierung der Unterbäume kann in C++ nicht zur Kompilierzeit garantiert werden.
Stattdessen muss in `middle` auf `FingerTree<T>` verwiesen werden und die Baumstruktur der `Node`-Typen wird durch `Internal` und `Leaf`-Typen erzeugt.
Diese Neudefinition der `Node`-Typen hat auch einen Einfluss auf die API der Algorithmen.
Während die gleichen Algorithmen in der Haskell-Definition verwendet werden können, um einzelne Elemente vom Typ `a`, sowie deren verpackete Knoten (`Node a`, `Node (Node a)`, etc.) einzufügen oder abzutrennen, muss die C++-Definition immer Knoten einfügen und zurückgeben.
Die internen rekursiven Algorithmen werden als `private` markiert und nur von speziellen Hilfsfunktionen aufgerufen, welche die Knoten entpacken oder verpacken.

#figure(
  figures.finger-tree.def.illegal,
  caption: [Nicht-reguläre Definition von Fingerbäumen.],
) <lst:illegal-recursive-type>

In der simplifizierten Definition in @lst:gen-finger-tree ist bereits zu sehen, dass die Elemente `a` und deren generische Mesures `v` durch die expliziten Typen `K` und `V` ersetzt wurden.
Für T4gl sind andere Measures als `Key` über Schlüssel-Wert-Paare nicht relevant und wurden daher direkt eingesetzt.
Diese Ersetzung zeigt sich auch in den Algorithmen, welche einerseits direkt auf die Invarianzen von `Key` und dessen Ordnungsrelation zurückgreifen, sowie auch diese Algorithmen enthalten, welche nur für geordnete Sequenzen sinnvoll sind (`insert` und `remove`).

Da die Implementierung des 2-3-Fingerbaums persistent sein soll, werden statt gewöhnlichen Pointern `std::shared_ptr` verwendet, um auf geteilte Knoten und Unterbäume zu zeigen.
Jeder in Haskell definierte Typ `T` wird hier durch einen Klasse `T` direkt abgebildet, welche zwei Felder enthält:
- `_repr`: Einen `std::shared_ptr<TBase>`.
- `_kind`: Einen Diskriminator, welcher angibt welche Variante in `_repr` enthalten ist (insofern nötig).

Varianten werden als ableitende Klassen von `TBase` implementiert.
Alle Schreibzugriffe auf `T` sorgen dann für eine flache Kopie der im `_repr`-Felds verwiesenen Instanz.
Damit wird sicher gegangen, dass Persistenz erhalten wird (siehe @sec:invis-pers für mögliche Optimierungen).
Diese Trennung ist auch nötig, um den Übergang einer Variante in die andere möglich zu machen, ohne das der Nutzer der Klasse diese selbst verwalten muss.

Die folgenden Definitionen und Codeausschnitte verzichten aus Platzgründen auf redundante Sprachkonstrukte wie etwa die Template-Parameterdeklarationen ```cpp template <typename K, typename V>```, da diese prinzipiell an fast jeder Klasse vorkommen.
Es wird außerdem darauf verzichtet für jede Klasse die Getter der Felder zu definieren.
Für alle Felder aller Typen werden Getter wiefolgt definiert:
Sei `_field` ein Feld vom Typen `Type` der Klasse `Class`, so exisitert eine Methode ```cpp auto Class::field() const -> const& Type```.

== Node
Damit die rekursiven Definitionen der Algorithmen in der Lage sind einen einzelnen Wert in Form eines Knotens zurück zu geben, muss es eine `NodeLeaf`-Variante geben, welche nur einen Wert und dessen Schlüssel enthält.
Zur Bildung der rekursiven Struktur gibt es eine `NodeDeep`-Variante, welche sowohl `Node2` als auch `Node3` als `std::vector` abbildet.
Wie zuvor bereits erklärt, erben diese von eine gemeinsamen Klasse `NodeBase`, welche durch `Node` persistent verwaltet wird.

Für die Knoten ist kein Übergang zwischen verschiedenen Zuständen nötig, Blattknoten werden durch die Hilfsfunktionen in `FingerTree` erstellt oder aufgelöst und interne Knoten werden innerhalb der rekursiven Algorithmen erstellt oder aufgelöst.
Dadurch ist die API und Implementierung von Knoten verhältnismäßig simpel.

#figure(
  figures.finger-tree.def.node,
  caption: [Die Definition der `Node`-Klassen.],
) <lst:finger-tree-node>

@lst:finger-tree-node zeigt die Definition der `Node`-Klassen.
Die Felder `_size` und `_key` in `NodeDeep` sind die akkumulierten Werte der Kinder dieses Knotens, sprich die Summe von `_size` und der größte Schlüssel der Kindknoten.
Für `LeafNode` liefert der Getter für `_size` direkt 1 und durch die inherente Sortierung der Schlüssel ist der größte Schlüssel immer der im rechten Kindknoten.

== Digits
Die Digits eines tiefen 2-3-Fingerbaums könnten, ähnlich wie in @bib:hp-06[S. 8] vorgeschlagen, als vier Varianten mit jeweils 1-4 Knoten als Feldern umgesetzt werden.
Das würde allerdings nur unnötig die Implementierung erschweren.
Daher gibt es für Digits nur eine Variante, `DigitsBase`.
@lst:finger-tree-digits zeigt die Definition von `Digits` und `DigitsBase`.
Da `Node` selbst bereits Persistenz umsetzt, könnte `Digits` darauf verzichten und diese Knoten direkt als `std::vector` speichern und diesen kopieren.
Auf eine solche Optimierung wurde aus Zeitgründen vorerst verzichtet, da ständiges Kopieren von Vektoren, wenn auch klein (1-4 Elemente), teurer sein kann als die geringere Verzögerung durch weniger Indirektion.
Dadurch ist die Definition auch einfacher mit denen der anderen Klassen zu vergleichen.

#figure(
  figures.finger-tree.def.digits,
  caption: [Die Definition der `Digits`-Klassen.],
) <lst:finger-tree-digits>

Ähnlich wie auch in @lst:finger-tree-node, gibt es akkumulierte `_size`- und `_key`-Felder, diese haben den gleichen Zweck wie die in `NodeDeep`.
Da es nur eine Variante gibt, kann auf das `_kind`-Feld in `Digits` verzichtet werden.
Die API von `Digits` verhält sich ähnlich wie die einer `Deque`, mit spezialisierten Funktionen für `push` und `pop` an beiden Seiten.
Desweiteren werden Funktionen bereitgestellt, um den Über- und Unterlauf zu vereinfachen; diese entpacken oder verpacken mehrere Knoten.

== FingerTree
@lst:finger-tree-self zeigt die gleiche Struktur wie die vorherigen Definitionen und baut auf denen von `Node` und `Digits` auf.
Direkt fällt auf, dass die Variante `Empty` nutzlos erscheint, die Abwesenheit von Knoten könnte auch durch ```cpp _repr == nullptr``` abgebildet werden.
Damit wird auch verhindert, dass Move-Konstruktoren von `FingerTree` alte Instanzen in einem uninitialisierten Stand lassen, wie es bei `Digits` und `Node` der Fall ist.
Ähnlich wie bei `Digits` wird auf solch eine Optimierung verzichtet.
Das vereinfacht den Vergleich zu den Haskell-Definitionen.

#figure(
  figures.finger-tree.def.self,
  caption: [Die Definition der `FingerTree`-Klassen.],
) <lst:finger-tree-self>
