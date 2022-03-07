# Kruhy v obilí

Vaším úkolem bude naprogramovat jednoduchou třídu `CCircle` modelující kruh a podpůrnou třídu `CPoint`, kterou třída `CCircle` využívá. Pro jednoduchost můžeme použít sturkturu pro implementaci bodu. Kruhy se mohou přesouvat a naším cílem je, zjistit, jestli zadaný bod leží uvnitř kruhu a na jeho okraji, nebo mimo něj. Začněte nejprve návrhem struktur/tříd a seznamu potřebných metod; poté dopište těla.


## Bližší specifikace

Vaším úkolem bude rozmyslet si a napsat:

* **Reprezentaci souřadnic ve 2D prostoru.**
  * Víme, že to může libovolné celé číslo v maximimálním rozsahu \{-5'000'000, ..., 5'000'000\}. Zvolte vhodnou dostatečně úspornou reprezentaci.
  * Rozmyslete si, které operace s body budete potřebovat dělat, například odčítání bodů by se mohlo hodit.
  * Mohla by se hodit i vhodná metoda, která Vám zjistí vzdálenost od počátku.

* **Reprezentaci třídy kruhu.**
  * Abychom si ušetřili práci při volání metod, tak některé metody budou splňovat takzvaný fluent interface. To znamená, že můžete mít instanci `x` datového typu `X` a budete moci řetězit volání metod `x.methodA().methodB();`
  * Dále bychom chtěli možnost přesouvat kruh, občas se totiž může stát, že si někdo splete souřadnice a kruh se objeví na úplně špatném místě.
  * Poslední část této třídy je dotaz na daný kruh a zjistit, jestli se nějaký bod objevuje vně tohoto kruhu, nebo ne.

* **Pokud budete mít čas navíc**
  * Naimplementujte novou třídu/strukturu `CTriangle`, která bude mít pouze jednu funkci a to k otestování, jestli 3 předané body mohou tvořit trojúhelník.
  * To znamená, že není potřeba mít instanci této třídy, ale můžeme tuto funkci volat libovolně jen s jejím jménem.
  * Diskutujte, proč jsme jen nevytvořili jednu "hloupou" funkci, ale namísto toho jsme naimplementovali část třídy.


## Ukázkový výpis

V této úloze výpisy používat nebudeme, pokud budete potřebovat, nějaký další výpis si určitě můžete přidat. Testování probíhá formou jednoduchých assertů, které mají za úkol otestovat vhodnost Vámi zvoleného návrhu. Pokud budete potřebovat, doplňte si i své další asserty. 

