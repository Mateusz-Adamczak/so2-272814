# Systemy operacyjne 2 - projekt 2
## Wielowątkowy serwer czatu
### Autor: Mateusz Adamczak 272814



## 1. Wstęp

Celem projektu była implementacja serwera czatu między wieloma użytkownikami z wykorzystaniem wielowątkowości w języku C++.

Czat podzielony jest na dwie części - **serwer** oraz **klienta**. Serwer jest głównym modułem, do którego może podłączyć się wiele instancji klientów kiedy jest uruchomiony. Każda instancja klienta może wysyłać wiadomości, które są odbierane przez serwer, i przesyłane do pozostałych klientów. Programy są przeznaczone dla systemu operacyjnego Linux Ubuntu, i do działania wykorzystuje systemowy terminal - w osobnych oknach terminalu uruchamiany jest proces serwera oraz klientów.

## 2. Instrukcje uruchomienia

Plik Makefile zawiera zautomatyzowane polecenie kompilacji plików `chat-server.cpp` i `chat-client.cpp` w systemie operacyjnym Linux:

```
all:
	g++ -std=c++11 -pthread chat-server.cpp -o server
	g++ -std=c++11 -pthread chat-client.cpp -o client
```

Można go uruchomić poprzez wywołanie w terminalu polecenia "make" w katalogu, w którym znajdują się pliki `chat-server.cpp` i `chat-client.cpp`.

## 3. Opis kodu

Do połączenia serwera i klientów wykorzystywane są gniazda TCP. Serwer przy uruchomieniu tworzy gniazdo i oczekuje na połączenia klientów. Przy nowym połączeniu dodaje klienta do wektora `clients`, chronionego przez mechanizm wzajemnego wykluczania z klasy Mutex (identyczny jak w pierwszym projekcie), i tworzy nowy wątek dla klienta wykonujący funkcję `handleClient`, która przy odebraniu od klienta wiadomości wywołuje funkcję `broadcastMessage` wysyłającą tą wiadomość do wszystkich klientów oprócz tego, który sam ją wysłał.

Klient przy uruchomieniu próbuje połączyć się z serwerem. Po pomyślnej próbie pyta użytkownika o pseudonim którego chce używać, i w pętli umożliwia mu wysyłanie wiadomości. Wątek `receiver`, wykonujący funkcję `receiveMessages`, jest odpowiedzialny za odbieranie wiadomości przesyłanych mu przez serwer. Wpisanie polecenia `/exit` powoduje rozłączenie z serwerem przy następnym wysłaniu wiadomości przez któregoś z pozostałych użytkowników; innym dopuszczalnym sposobem rozłączenia jest terminacja procesu za pomocą skrótu klawiszowego Ctrl + C.

### 3.1 Wątki

Podczas pracy programów tworzone są następujące wątki:

* **główny wątek serwera (`main`)** - pętla przyjmująca połączenia od klientów, dodająca ich do wektora, i tworząca dla każdego z nich osobne wątki.
* **wątki klientów w programie serwera (`handleClient`)** - wątki odpowiadające poszczególnym klientom w programie serwera, przyjmujące wiadomości od klienta za którego są odpowiedzialne, i przesyłające je do pozostałych.
* **główny wątek klienta (`main`)** - pętla odczytująca z terminalu wiadomości do wysłania na czacie, i przesyłająca je do sewera.
* **wątek odbierający wiadomości (`receiveMessages`)** - wątek odbierający wiadomości przesyłane do klienta przez serwer, i wypisujący je na czacie, oraz kończący działanie w przypadku zakmnięcia połączenia lub wystąpienia błędu.

### 3.2 Sekcje krytyczne

Jedyną częścią uznawaną za sekcję krytyczną jest wektor `clients` w programie serwera, przechowujący połączonych z serwerem klientów. Procedury `lock()` oraz `unlock()` obiektu klasy `Mutex` zabezpieczają dostęp do wektora za każdym razem kiedy program uzyskuje do niego dostęp, czyli:

* przy dodawaniu nowego klienta wewnątrz pętli w funkcji `main`;
* przy usuwaniu klienta w funkcji `handleClient`;
* przy wysyłaniu wiadomości do klientów w funkcji `broadcastMessage`.

Dostęp do wektora klientów jest chroniony, żeby zapobiec konfliktom między czynnościami dodawania lub usuwania klienta, i procesem wysyłania wiadomości. Ograniczanie dostępu do listy dla tylko jednego wątku zapewnia, że nie zostanie ona zmodyfikowana w trakcie gdy `broadcastMessage` przez nią iteruje, co mogłoby doprowadzić do błędów wynikających np. z próby wysłania wiadomości do klienta, który przestał istnieć.
