# Zlomky

## Zadání

Vašim úkolem je doplnit třídu `CFraction`, která reprezentuje redukovaný zlomek tj. aby největší společný dělitel jmenovatele a čitatele bylo 1. Dále aby dokázala rozumně efektivně pracovat se zlomky tak, jak jsme zvyklí z matematiky.

* Zvolte vhodnou reprezentaci hodnot; předpokládejme, že zde mohou být i mnohamiliardové hodnoty.
* Vytvořte vhodný konstruktor pro zadání zlomku jen pomocí čitatele, tedy celého čísla, nebo i s navazujícím jmenovatelem. 
* Naprogramujte + - * / tak, aby fungoval i v kombinaci s int na libovolné straně; jak to napsat co nejmenším počtem funkcí? Hint: Využijte toho, že konstruktory se používají i pro přetypování.
* Napište přetížené operátory +=, -=, *= a /=.
* Napište všechny porovnávací/relační operátory.
* Naprogramujete operátory pro vstup a výstup. Jako oddělovač čitatele a jmenovatele použijte lomeno `/`. Na vstupu pracujte s možností, že chybí jmenovatel. _Hint: může se hodit metoda peek()_ 
* Pokud se při čtení ze streamu něco pokazí, nastavte streamu failbit. _Hint: vhodně použijte metodu `setstate` s parametrem `std::ios::failbit`._

### Postup
* Rozmyslete, jaké *atributy* musí třída CFraction mít
* Rozmyslete, jak budete vypadat *konstruktor*
* Rozmyslete, které *operátory* bude třeba *přetížit*
* Rozmyslete, jak vytvořit funkcionalitu načítání a vypisování hodnoty zlomku ze/do streamů. _(Hint: klíčové slovo friend)_

