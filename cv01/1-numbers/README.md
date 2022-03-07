# Jednoduchý program pro práci s čísly

Úloha pro začátečníky, rozebíraná společně před tabulí.

## Základní zadání

Vaším úkolem bude převést starší program pro práci s čísly z C do C++. Soubor si nejprve prostudujte, identifikujte nepřehledná/problematická místa.

Zejména se zaměřte na části s prací s pamětí, následně se vstupem a výstupem, tedy používání proudů namísto C funkcí `scanf` a `printf`. Vhodně využijte datový typ `bool`, který Vám C++ nabízí. Proměnným, které jsou označené `const`, ponechte `const` a případně, pokud by bylo potřeba, další dodejte. Rozmyslete si, proč odebírání klíčového slova `const` není dobrý nápad.

Koncepty, které si v této úloze procvičíte:

* Operátory **new** a **delete**, které zjednoduší zápis práce s dynamickou pamětí, za určitou cenu. 
  * **nullptr** jako nulová hodnota pointeru nezaměnitelná s číslem; což se může hodit k rozlišení volání.

* **Reference**, které zpřehlední kód odstraněním spousty zbytečných `*` uvnitř funkcí a `&` při jejich volání.
  * Předávat parametr kopií někdy nelze (co např. nemůžeme kopírovat?), pointer zase není praktický (poznáme v dalších týdnech), reference se může hodit více. 

* **Proudy** jako náhrada funkcí pro práci se vstupem a výstupem.

## Rozšíření

* Upravte program tak, aby místo s čísly pracoval s dominovými kostkami (tedy s neuspořádanými dvojicemi čísel).
* Vypište počet unikátních dominových kostek.

Koncepty, které si v této úloze procvičíte:

* Abstraktní myšlení (máme pole něčeho, je jedno čeho, stačí přepsat pár věcí)
* Načítání / výpis struktur z / do proudů (malá příprava na budoucí přetěžování operátorů `>>` a `<<`)
* Základní **algoritmy** ve standardní knihovně C++ (malá příprava na budoucí přetěžování operátoru `<`)
