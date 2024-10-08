#import "/src/util.typ": *
#import "/src/figures.typ"

T4gl (_engl._ #strong[T]esting #strong[4]th #strong[G]eneration #strong[L]anguage) ist ein proprietäres Softwareprodukt zur Entwicklung von Testsoftware für Industrieprüfanlagen wie die LUB (#strong[L]ow Speed #strong[U]niformity and #strong[B]alance), HSU (#strong[H]igh #strong[S]peed #strong[U]niformity and Balance) oder vielen Weiteren.
T4gl steht bei der Brückner und Jarosch Ingenieurgesellschaft mbH (BJ-IG) unter Entwicklung und umfasst die folgenden Komponenten:
- Programmiersprache
  - Anwendungsspezifische Features
- Compiler
  - Statische Anaylse
  - Übersetzung in Instruktionen
- Laufzeitsystem
  - Ausführen der Instruktionen
  - Scheduling von Green Threads
  - Bereitstellung von maschinen- oder protokollspezifischen Schnittstellen

Wird ein T4gl-Script dem Compiler übergeben, startet dieser zunächst mit der statischen Analyse.
Bei der Analyse der Skripte werden bestimmte Invarianzen geprüft, wie die statische Länge bestimmter Arrays, die Typsicherheit und die syntaktische Korrektheit des Scripts.
Nach der Analyse wird das Script in eine Sequenz von _Microsteps_ (atomare Instruktionen) kompiliert.
Im Anschluss führt des Laufzeitsystem die kompilierten _Microsteps_ aus, verwaltet Speicher und Kontextwechsel der _Microsteps_ und stellt die benötigten Systemschnittstellen zur Verfügung.

= Echtzeitanforderungen <sec:realtime>
Je nach Anwendungsfall werden an das T4gl-Laufzeitsystem Echtzeitanforderungen gestellt.

#quote(block: true, attribution: [
  Peter Scholz @bib:sch-05[S. 39] unter Verweis auf DIN 44300
])[
  Unter Echtzeit versteht man den Betrieb eines Rechensystems, bei dem Programme zur Verarbeitung anfallender Daten ständig betriebsbereit sind, derart, dass die Verarbeitungsergebnisse innerhalb einer vorgegebenen Zeitspanne verfügbar sind.
  Die Daten können je nach Anwendungsfall nach einer zeitlich zufälligen Verteilung oder zu vorherbestimmten Zeitpunkten anfallen.
]

Ist ein Echtzeitsystem nicht in der Lage, eine Aufgabe in der vorgegebenen Zeit vollständig abzuarbeiten, spricht man von Verletzung der Echtzeitbedingungen, welche an das System gestellt wurden.
Je nach Strenge der Anforderungen lassen sich Echtzeitsysteme in drei verschiedene Stufen einteilen:
/ Weiches Echtzeitsystem:
  Die Verletzung der Echtzeitbedinungen führt zu degenerierter, aber nicht zerstörter Leistung des Echtzeitsystems und hat _keine_ katastrophalen Folgen @bib:lo-11[S. 6].
/ Festes Echtzeitsystem:
  Eine geringe Anzahl an Verletzungen der Echtzeitbedingungen hat katastrophale Folgen für das Echtzeitsystem  @bib:lo-11[S. 7].
/ Hartes Echtzeitsystem:
  Eine einzige Verletzung der Echtzeitbedingungen hat katastrophale Folgen für das Echtzeitsystem @bib:lo-11[S. 6].

T4gl ist ein weiches Echtzeitsystem. Für Anwendungsfälle, bei denen Echtzeitanforderungen an T4gl gestellt werden, darf die Nichteinhaltung dieser Anforderungen keine katastrophalen Folgen haben (Personensicherheit, Prüfstandssicherheit, Prüflingssicherheit).
In diesem Fall sind im schlimmsten Fall nur die Testergebnisse ungültig und müssen verworfen werden.

= T4gl-Arrays <sec:t4gl:arrays>
Bei T4gl-Arrays handelt es sich um mehrdimensionale assoziative Arrays mit Schlüsseln, welche eine voll definierte Ordnungsrelation haben.
Um ein Array in T4gl zu deklarieren, wird mindestens ein Schlüssel- und ein Wertetyp benötigt.
Auf den Wertetyp folgt in eckigen Klammern eine komma-separierte Liste von Schlüsseltypen.
Die Indizierung erfolgt wie in der Deklaration durch eckige Klammern, es muss aber nicht für alle Schlüssel ein Wert angegeben werden.
Bei Angabe von weniger Schlüsseln als in der Deklaration, wird eine Referenz auf einen Teil des Arrays zurückgegben.
Sprich, ein Array des Typs `T[U, V, W]` welches mit `[u]` indiziert wird, gibt ein Unter-Array des Typs `T[V, W]` zurück, ein solches Unter-Array kann referenziert, aber nicht gesetzt werden.
Wird in der Deklaration des Arrays ein Ganzzahlwert statt eines Typs angegeben (z.B. `T[10]`), wird das Array mit fester Größe und durchlaufenden Indizes (`0` bis `9`) als Schlüssel angelegt und mit Standardwerten befüllt.
Für ein solches Array können keine Schlüssel hinzugefügt oder entnommen werden.

#figure(
  figures.t4gl.ex.array1,
  caption: [
    Beispiele für Deklaration und Indizierung von T4gl-Arrays.
  ],
) <lst:t4gl-ex>

Bei den in @lst:t4gl-ex gegebenen Deklarationen werden, je nach den angegebenen Typen, verschiedene Datenstrukturen vom Laufzeitsystem gewählt.
Diese ähneln den C++-Varianten in @tbl:t4gl-array-analogies, wobei `T` und `U` Typen sind und `N` eine Zahl aus $NN^+$.
Die Deklaration von `staticArray` weist den Compiler an, ein T4gl-Array mit 10 Standardwerten für den `String` Typ (die leere Zeichenkette `""`) für die Schlüssel 0 bis einschließlich 9 anzulegen.
Es handelt sich um eine Sonderform des T4gl-Arrays, welches eine dichte festgelegte Schlüsselverteilung hat (es entspricht einem gewöhnlichen Array).

#figure(
  figures.t4gl.analogies,
  caption: [
    Semantische Analogien in C++ zu spezifischen Varianten von T4gl-Arrays.
  ],
) <tbl:t4gl-array-analogies>

Die Datenspeicherung im Laufzeitsystem kann nicht direkt ein statisches Array (`std::array<T, 10>`) verwenden, da T4gl nicht direkt in C++ übersetzt und kompiliert wird.
Intern werden, je nach Schlüsseltyp, pro Dimension entweder ein Vektor oder ein geordnetes assoziatives Array angelegt.
T4gl-Arrays verhalten sich wie Referenztypen, wird ein Array `a2` durch ein anderes Array `a1` initialisiert, teilen sich diese die gleichen Daten.
Schreibzugriffe in einer Instanz sind auch in der anderen lesbar (demonstiert in @lst:t4gl-ref).

#figure(
  figures.t4gl.ex.array2,
  caption: [Demonstration von Referenzverhalten von T4gl-Arrays.],
) <lst:t4gl-ref>

Im Gegensatz zu dem Referenzverhalten der Arrays aus Sicht des T4gl-Programmierers steht die Implementierug durch QMaps.
Bei diesen handelt es sich um Copy-On-Write (CoW) Datenstrukturen, mehrere Instanzen teilen sich die gleichen Daten und erlauben zunächst nur Lesezugriffe darauf.
Muss eine Instanz einen Schreibzugriff durchführen, wird vorher sichergestellt, dass diese Instanz der einzige Referent der Daten ist, wenn nötig durch eine Kopie der gesamten Daten.
Dadurch sind Kopien von QMaps initial sehr effizient, es muss lediglich die Referenzzahl der Daten erhöht werden.
Die Kosten der Kopie zeigt sich erst dann, wenn ein Scheibzugriff nötig ist.

Damit sich T4gl-Arrays trotzdem wie Referenzdaten verhalten, teilen sie sich nicht direkt die Daten mit CoW-Semantik, sondern QMaps durch eine weitere Indirektionsebene.
T4gl-Arrays bestehen aus Komponenten in drei Ebenen:
+ T4gl: Die Instanzen aus Sicht des T4gl-Programmierers (z.B. die Variable `a1` in @lst:t4gl-ref).
+ Indirektionsebene: Die Daten aus Sicht des T4gl-Programmierers, die Ebene zwischen T4gl-Array Instanzen und deren Daten.
  Dabei handelt es sich um Qt-Datentypen wie QMap oder QVector.
+ Speicherebene: Die Daten aus Sicht des T4gl-Laufzeitsystems. Diese Ebene ist für den T4gl-Programmierer unsichtbar.

Zwischen den Ebenen 1 und 2 kommt geteilte Schreibfähigkeit durch Referenzzählung zum Einsatz, mehrere T4gl-Instanzen teilen sich eine Qt-Instanz und können von dieser lesen, als auch in sie schreiben, ungeachtet der Anzahl der Referenten.
Zwischen den Ebenen 2 und 3 kommt CoW + Referenzzählung zum Einsatz, mehrere Qt-Instanz teilen sich die gleichen Daten, Schreibzugriffe auf die Daten sorgen vorher dafür, dass die Qt-Instanz der einzige Referent ist, wenn nötig durch eine Kopie.
Wir definieren je nach Tiefe drei Typen von Kopien:
/ Typ-1 (flache Kopie):
  Eine Kopie der T4gl-Instanz erstellt lediglich eine neue Instanz, welche auf die gleiche Qt-Instanz zeigt.
  Dies wird in T4gl durch Initialisierung von Arrays durch existierende Instanzen oder die Übergabe von Arrays an normale Funktionen hervorgerufen.
  Eine flache Kopie ist immer eine $Theta(1)$-Operation.
/ Typ-2:
  Eine Kopie der T4gl-Instanz *und* der Qt-Instanz, welche beide auf die gleichen Daten zeigen.
  Wird eine tiefe Kopie durch das Laufzeitsystem selbst hervorgerufen, erfolgt die Kopie der Daten verspätet beim nächten Schreibzugriff auf eine der Qt-Instanzen.
  Halbtiefe Kopien sind Operationen konstanter Zeitkomplexität $Theta(1)$.
  Aus einer halbtiefen Kopie und einem Schreibzugriff folgt eine volltiefe Kopie.
/ Typ-3 (tiefe Kopie):
  Eine Kopie der T4gl-Instanz, der Qt-Instanz *und* der Daten.
  Beim expliziten Aufruf der Methode `clone` durch den T4gl Programmierer werden die Daten ohne Verzögerung kopiert.
  Tiefe Kopien sind Operationen linearer Zeitkomplexität $Theta(n)$.

Bei einer Typ-1-Kopie der Instanz `a` in @fig:t4gl-indirection:new ergibt sich die Instanz `b` in @fig:t4gl-indirection:shallow.
Eine Typ-2-Kopie hingegen führt zur Instanz `c` in @fig:t4gl-indirection:deep.
Obwohl eine tiefe Kopie zunächst nur auf Ebene 1 und 2 Instanzen kopiert, erfolgt die Typ-3-Kopie der Daten auf Ebene 3 beim ersten Schreibzugriff einer der Instanzen (@fig:t4gl-indirection:mut).

In seltenen Fällen kann es dazu führen, dass wie in @fig:t4gl-indirection:deep eine Typ-2-Kopie angelegt wurde, aber nie ein Schreibzugriff auf `a` oder `c` durchgeführt wird, während beide Instanzen existieren.
Sprich, es kommt zur Typ-2-Kopie, aber nicht zur Typ-3-Kopie.
Diese Fälle sind nicht nur selten, sondern meist auch Fehler der Implementierung.
Bei korrektem Betrieb des Laufzeitsystems sind Typ-2-Kopien kurzlebig und immer von Typ-3-Kopien gefolgt, daher betrachten wir im folgenden auch Typ-2-Kopien als Operationen linearer Zeitkomplexität $Theta(n)$, da diese bei korrektem Betrieb fast immer zu Typ-3 Kopien führen.

#subpar.grid(
  figure(figures.t4gl.layers.new, caption: [
    Ein T4gl Array nach Initalisierung. \ \
  ]), <fig:t4gl-indirection:new>,
  figure(figures.t4gl.layers.shallow, caption: [
    Zwei T4gl-Arrays teilen sich eine C++ Instanz nach Typ-1 Kopie.
  ]), <fig:t4gl-indirection:shallow>,
  figure(figures.t4gl.layers.deep-new, caption: [
    Zwei T4gl-Arrays teilen sich die gleichen Daten nach Typ-2 Kopie. \ \
  ]), <fig:t4gl-indirection:deep>,
  figure(figures.t4gl.layers.deep-mut, caption: [
    Zwei T4gl-Arrays teilen sich keine Daten nach Typ-2 Kopie und Schreibzugriff.
  ]), <fig:t4gl-indirection:mut>,
  columns: 2,
  caption: [Die drei Ebenen von T4gl-Arrays in verschiedenen Stadien der Datenteilung.],
  label: <fig:t4gl-indirection>,
)

Ein kritischer Anwendungsfall für T4gl-Arrays ist die Übergabe einer rollenden Historie von Werten einer Variable.
Wenn diese vom Laufzeitsystem erfassten Werte an den T4gl-Programmierer übergeben werden, wird eine Typ-2-Kopie erstellt.
Die T4gl-Instanz, welche an den Programmierer übergeben wird, sowie die interne Qt-Instanz teilen sich Daten, welche vom Laufzeitsystem zwangsläufig beim nächsten Schreibzugriff kopiert werden müssen.
Es kommt zur Typ-3-Kopie, und, daraus folgend, zu einem nicht-trivialem zeitlichem Aufwand von $Theta(n)$.
Das gilt für jeden ersten Schreibzugriff, welcher nach einer Übergabe der Daten an den T4gl-Programmierer erfolgt.
