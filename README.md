# Kompilator

Kod kompilatora do języka z projektu JFTT

# Przygotowanie i używanie kompilatora

* make
* ./kompilator \<nazwa-pliku-wejsciowego\> \<nazwa-pliku-wyjsciowego\>

# Autor

Adrian Wilhelmi 268479

# Narzędzia

* bison 3.8.2
* flex 2.6.4
* GNU Make 4.3
* g++ 13.2.0

# Pliki

* parser.y - parser
* lexer.l - lekser
* ast.hpp - obiektowa wysokopoziomowa implementacja drzewa stworzonego przez parser
* ast.cpp - implementacja funkcji członkowskich klasy AST
* ast\_visitor.hpp - wzorzec wizytora do generowania kodu z węzłów AST
* ast\_code\_generator.hpp - implementacja wizytora. Przechodzi po węzłach AST oraz generuje odpowiedni kod, obsługuje błędy w kodzie z pliku wejściowego.
* main.cpp - uruchamia parser oraz generator kodu dla zadanego pliku wejściowego i zapisuje wygenerowany kod do podanego pliku wyjściowego
* Makefile - kompiluje cały projekt
