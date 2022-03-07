# Wordle

Autoři populární hry Wordle vás požádali, abyste jejich program v podřadném jazyce JS přepsali do fantastického jazyka C++ ;) Prý vám za to zaplatí v céčkách.

Wordle je hra, v níž máte uhodnout anglické slovo na 5 písmen, a máte na to 6 pokusů. Po každém pokusu program označí:

- Zeleně každé písmeno v zadaném slově, které je na stejné pozici i ve slově hádaném.
- Žlutě to písmeno v zadaném slově, které je sice v hádaném slově, ale na jiné pozici.
- Nijak všechna ostatní písmena.

Dále vám hra před každým pokusem ukazuje, která písmena jste nezkoušeli, která jsou zelená, která jsou žlutá a která jste už zkoušeli, ale v hledaném slově nejsou.

## Zadání

Vytvořte následující třídy (detaily viz ukázka níže):

`CLetter` reprezentující výsledek hádaní písmene. Bude to jednoduchá struktura obsahující písmeno a výsledek hádání z výčtového typu (viz ukázka).

`CPuzzle` reprezentující zadání. Třída si udržuje stav hry, hádané slovo a reaguje na různé dotazy (viz ukázka). Nejsložitější je metoda na hádání, která zároveň vrací, jak hádání dopadlo.

`CWordle` reprezentující herní systém. Třída bude obsahovat metody `load` (na načtení slovníku ze souboru), `unload` (na ukončení práce se třídou) a `getPuzzle`, která vrátí zadání.

Třídy mohou obsahovat další metody a členské proměnné pro vaše potřeby. Při návrhu:

* Dbejte na důsledné využívání `const`.
* Minimalizujte kopie při předávání parametrů.
* Snažte se pracovat s viditelností členů tříd (ne vše teď půjde ideálně, ale snažte se).

Další poznámky:

* Zadání nesprávných (dlouhých / krátkých) slov a zadání po skončení hry řešte tak, že výsledkem bude ohodnocení 5 zadaných mezer, a počítadlo pokusů neměňte.
* Přítomnost zadaného slova v seznamu můžete, ale nemusíte řešit. (Může to být trochu náročnější na návrh.)
* Pro náhodné vybírání slova k hádání využijte starý dobrý `rand()` z `<cstdio>`. Případně se podívejte na https://cpppatterns.com/patterns/choose-random-element.html
* Nezapomeňte na malá/velká písmena. Sjednoťte si hledaná i zadávaná slova na velká písmena; `toupper()` je váš kamarád.

## Výchozí kód

Vaši implementaci můžete doplnit do níže uvedené ukázky

```cpp

```
Pár věcí možná neznáte; nenechte se jimi vyděsit, jsou to vaši kamarádi:

* Výčtový datový typ je jen jinak pojmenované celé číslo (obvykle int), jehož hodnoty mají speciální názvy.
* Příkaz `using` zde slouží k přejmenovávání datových typů, abych nemusel psát dlouhé krkolomné názvy.
* `std::array` popisuji dále v textu; je to prostě pole pevné délky, které se dá snadno předávat a se kterým se dá snadno pracovat.
* Ranged for cyklus (`for(typ položka : kolekce)`) se používá k procházení různých kolekcí (polí, řetězců apod.) od začátku do konce, pokud to daná kolekce podporuje.

## Užitečné rady

### Řetězce v C++

Na práci s 5písmennými slovy si můžete zvolit 6prvkové pole znaků, nebo datový typ `std::string`, který si sám spravuje paměť (jak to dělá, se dozvíte v dalších týdnech). Ke znakům v `std::string` můžete přistupovat podobně jako v C přes `[index]`. Dále existuje užitečná metoda `size()`, která vrátí délku řetězce. Také lze řetězce v `std::string` porovnávat přes operátor `==`, což v C nešlo. `std::string` umí i další věci, ale teď je nepotřebujete znát.

```cpp
#include <string>
#include <iostream>

int main() {
    std::string name = "franta";
    std::string noname; // vzhledem k tomu, jak tato třída funguje, toto je prázdný řetězec
    name[0] = 'F';
    if(name == "Franta") {
        std::cout << name << " je super, jeho jméno má " << name.size() << " písmen!" << std::endl;
    }
    else {
        std::cout << name << " fuj!" << std::endl;
    }
    return 0;
}
```

### Dynamické pole

Na seznam slov pro slovník můžete využít nafukovací pole. Ale psát si je sám je šílené, navíc v C++ neexistuje realokace a s některými typy prvků bychom museli hodně čarovat. Proto ve standardní knihovně existuje užitečná třída `std::vector`. Jde o šablonu, což v tomto případě znamená, že je potřeba při uvádění typu proměnné uvést i typ prvku nafukovacího pole do špičatých závorek `<>`, např. `std::vector<int>` je nafukovací pole čísel, `std::vector<std::string>` pole řetězců atd. Také tato třída si sama spravuje svou paměť. Nám pro začátek stačí znát jen pár metod.

```cpp
#include <vector>
#include <iostream>

int main() {
    std::vector<int> numbers = { 1, 2, 3 }; // některé prvky může mít vector od začátku
    std::vector<int> nonumbers; // nebo taky nemusí; v tomhle případě bude prázdný
    
    numbers.push_back(42); // přidá na konec pole další číslo
    for(int i = 0; i < numbers.size(); i++) { // procházet pole se dá i jinak, časem si ukážeme...
        std::cout << numbers[i] << std::endl;
    }
    
    return 0;
}
```

### Předávání statických polí

Pro předávání polí pevné délky můžete využít vlastní struktury s polem dané délky, např.

```c++
struct CWordGuess {
    CLetter data[5];
};

// ...

CWordGuess g = { { 'H', WRONG }, { 'O', CORRECT }, { 'U', CORRECT }, { 'S', CORRECT }, { 'E', CORRECT } };
```

Podobně ale můžete využít šablonu `std::array`:

```c++
#include <array>

std::array<CLetter, 5> g = { { 'H', WRONG }, { 'O', CORRECT }, { 'U', CORRECT }, { 'S', CORRECT }, { 'E', CORRECT } };
```

Oproti předávání holého pole nebo pointeru mají výše uvedené možnosti tu vlastnost, že znáte velikost pole a že obsahy polí můžete kopírovat jednoduše přiřazením.

## Hodnocení úlohy

Bez načítání seznamu slov a s hádáním slova bez označování žlutých (žlutá je trochu náročná) za půl bodu za aktivitu.

S načítáním slovníku a s korektním vyhodnocením písmen na nesprávné pozici ve slově za celý bod za aktivitu.

## Poznámky

Dodaný seznam slov pochází z https://github.com/charlesreid1/five-letter-words/blob/master/sgb-words.txt
