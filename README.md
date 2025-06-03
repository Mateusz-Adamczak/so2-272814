# Systemy operacyjne 2 - projekt 1
## Problem ucztujących filozofów
### Autor: Mateusz Adamczak 272814



## 1. Wstęp

Celem projektu była implementacja problemu ucztujących filozofów oraz jego rozwiązania za pomocą wielowątkowości w języku C++. Jednym z założeń projektu była ręczna implementacja obsługi sekcji krytycznych.

Problem ucztujących filozofów stanowi reprezentację zarządzania zasobami dla wielowątkowości. Opisuje sytuację, gdzie N filozofów siedzi przy stole, na którym znajduje się N widelców (gdzie N >= 2; tradycyjnie przyjmuje się liczbę 5, aczkolwiek program pozwala na wybranie innej liczby). Filozofowie na przemian myślą i jedzą, przy czym do jedzenia potrzebują sztućców znajdujących się po ich lewej i prawej stronie, w związku z czym dla pary sąsiadujących filozofów jeden z widelców jest wspólny. W sytuacji, gdzie każdy filozof podniesie jeden widelec, będzie w nieskończoność oczekiwał na drugi, co doprowadzi do zakleszczenia (deadlock) - sytuacji, gdzie niemożliwe jest dokonanie postępu.

Spośród kilku rozwiązań ten program posiada implementację rozwiązania zakładającego wprowadzenie arbitra w postaci kelnera, który kontroluje dostęp do widelców - zamiast samemu podnosić widelce, filozofowie proszą o to kelnera. Ręcznie zaprogramowany mechanizm wzajemnego wykluczania (mutual exclusion - mutex) zapobiega jednoczesnym dokonywaniu zmian w statusie dostępności widelców przez kilku filozofów naraz, a mechanizm kolejki w procedurze przydzielania dostępu do zasobów zapewnia, że tylko jeden filozof jednocześnie ma dostęp do widelców (ma tą wyłączność do momentu uzyskania obu), a także poprzez zasadę "kto pierwszy ten lepszy" (FIFO - First In First Out) gwarantuje, że każdy filozof który poprosi o widelce w końcu będzie miał swoją kolej na skorzystanie z nich, co zapobiega zagłodzeniu - sytuacji, gdzie dany element nigdy nie doczeka się swojej okazji na dostęp do zasobów.

## 2. Instrukcje uruchomienia

Plik Makefile zawiera zautomatyzowane polecenie kompilacji pliku main.cpp w systemie operacyjnym Linux:

g++ -std=c++11 -pthread main.cpp -o dining

Można go uruchomić poprzez wywołanie w terminalu polecenia "make" w katalogu, w którym znajduje się plik main.cpp.

## 3. Opis kodu

Kod wykorzystuje następujące biblioteki:

* **iostream** - do obsługi wejścia (liczby filozofów i polecenia "stop") oraz wyjścia (statusy filozofów) w konsoli;
* **thread** - do tworzenia wątków;
* **vector** - do wektorów przechowujących statusy widelców w obiekcie klasy *Waiter*, oraz wątków odpowiadających filozofom;
* **chrono** - do generowania losowych odstępów czasu, w których filozofowie proszą o dostęp i go zwalniają;
* **queue** - do mechanizmu kolejki stosowanego przy przydzielaniu dostępu.

W programie występują następujące klasy:
* **Mutex** - ręczna implementacja (zgodnie z założeniami projektu) mechanizmu wzajemnego wykluczania. Posiada zmienną prywatną *locked*, oraz procedury publiczne *lock*, która ustawia zmienną *locked* na wartość prawdziwą, czeka na zmianę na fałszywą i po każdym nieudanym sprawdzeniu usypia wątek na 1 milisekundę, oraz *unlock* która zmienia wartość na fałszywą. Są to działania atomiczne, tzn. wykonywane w sposób niepodzielny.
* **Waiter** - implementacja kelnera (arbitra), kontrolującego dostęp do widelców przez filozofów. Do sekcji *private* należą:
  * obiekt *lock* klasy *Mutex*, blokujący dostęp dla wątków które wywołują procedury klasy kiedy jedna już ma dostęp do zasobów;
  * vector *forks* przechowujący stany widelców;
  * kolejka *queue*, sterująca dostępem filozofów do zasobów.  
Do sekcji *public* należą:
  * procedura *requestAccess()*, która dodaje filozofa do kolejki, i czeka aż znajdzie się na początku kolejki oraz jego widelce będą dostępne, po czym je zabiera; momenty operacji na kolejce oraz modyfikacji statusu widelców są zabezpieczone przez mutex;
  * procedura *releaseAccess()*, która ustawia status widelców filozofa na dostępne; ta operacja jest zabezpieczona przez mutex.
* **Philosopher** - implementacja filozofa, zgodnie z założeniami problemu. Do sekcji *private* należą:
  * zmienna *id*, stanowiąca unikatowy identyfikator wątku filozofa;
  * zmienne *forkLeft* i *forkRight*, zawierające numery lewego i prawego widelca przysługującego filozofowi;
  * odniesienie do instancji klasy *Waiter*, stanowiącej arbitra przypisanych mu filozofów;
  * odniesienie do zmiennej *volatile bool running*, decydującej czy symulacja ma trwać.  
Do sekcji *public* należą:
  * konstruktor klasy, przypisujący wartości opisanym wyżej zmiennym;
  * procedura *dine()* wywoływana jako realizowana przez wątek; w pętli *while* wykonywanej dopóki zmienna *running* jest prawdziwa filozof myśli (procedura *think()*), prosi o dostęp do sztućców (waiter.requestAccess()), je (procedura *eat()*) i zwraca sztuśce (*waiter.releaseAccess()*).
  * procedura *think()* odpowiadająca czynności rozmyślania przez filozofa; wyświetla komunikat i czeka przez 1 do 5 sekund na powrót do procedury *dine()* (wartość generowana losowo);
  * procedura *eat()* odpowiadająca czynności jedzenia; wyświetla komunikat i czeka przez 1 do 5 sekund na powrót do procedury *dine()* (wartość generowana losowo).

W programie zdefiniowana jest procedura *commandLoop()*. Jest ona wykorzystywana przez wątek odpowiedzialny za przyjmowanie komend od użytkownika. Jedyną rozpoznawaną przez program komendą jest "stop", która ustawia wartość zmiennej *running* na *false*, co daje filozofom znak, żeby po zakończeniu obecnego cyklu *dine()* nie wykonywały kolejnego. Traktując sytuację przedstawioną w treści problemu dosłownie, wydanie polecenia "stop" można rozumieć jako powiedzenie filozofom, że lokal zaraz zostanie zamknięty i mają dokończyć swoją porcję jedzenia, zwrócić sztućce, i opuścić lokal (zakończyć obecność wątku w programie).

Procedura *commandLoop()* przyjmuje za pomocą *std::cin* polecenia, zapisywanie w zmiennej *command*, i jeśli polecenie to "stop" ustawia wartość zmiennej *running* na *false*. W przeciwnym wypadku wyświetla komunikat o nieznanej komendzie i pracuje dalej.

Funkcja *main()*, wykonywana przy uruchamianiu programu, najpierw za pomocą *std::cin* pobiera liczbę filozofów, którą ma przyjąć symulacja, i zapisuje ją w zmiennej *philosophersCount*. Jeśli wartość jest mniejsza od 2, program przerywa działanie - dla jednego filozofa problem nie ma sensu, ponieważ nie będzie konkurował sam ze sobą o zasoby; dodatkowo liczba widelców jest równa liczbie filozofów, a według założeń filozof potrzebuje dwóch widelców do jedzenia.  

Następnie tworzona jest instancja kelnera i odpowiednia ilość wątków filozofów, a także deklarowana jest zmienna *volatile bool running*. Słowo kluczowe *volatile* zapewnia bezpośredni odczyt wartości z pamięci, pomijając optymalizacje kompilatora. Tworzony jest również wątek odpowiedzialny za przyjmowanie komend.  


Po utworzeniu wątków odpowiadających pętli przyjmującej komendy oraz filozofom za pomocą procedury *join()* program otrzymuje informację, że przed zakończeniem działania ma czekać na zakończenie pracy wątków, co odbywa się poprzez zmiany wartości zmiennej *running* na false za pomocą polecenia "stop", ponieważ zarówno wątek obsługujący komendy jak i wątki filozofów sprawdzają tą wartość w pętli *while*. Po zakończeniu działania wszystkich wątków wyświetlany jest komunikat o zakończeniu symulacji, i program kończy działanie.

### 3.1 Wątki

Podczas pracy programu tworzone są następujące wątki:

* **wątek pętli dla poleceń** - za pomocą *std::cin* przyjmuje polecenia sterujące działaniem programu; jedynym rozpoznawanym przez program poleceniem jest "stop", dające sygnał do zakończenia symulacji problemu.
* **wątki filozofów** - osobne wątki dla każdego filozofa, działąjące równolegle i podejmujące próby uzyskania dostępu do widelców (zasobów) w losowych odstępach czasu.

### 3.2 Sekcje krytyczne

W kodzie programu można wyróżnić dwie sekcje krytyczne:

* **requestAccess()** - modyfikowanie statusu widelców na zajęte oraz zawartości kolejki, tj.
  * dodanie filozofa do kolejki;
  * usunięcie filozofa z kolejki, kiedy nadejdzie jego kolej i jego widelce są dostępne;
  * przyznanie dostępu do widelców - zmiana ich statusu na zajęte;
* **releaseAccess()** - modyfikowanie statusu widelców na dostępne.

Wszystkie sekcje krytyczne są zabezpieczane procedurami *lock()* oraz *unlock()* obiektu klasy *Mutex*.
