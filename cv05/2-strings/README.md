# Rozšíření úlohy ze 3. týdne.

## Část první
Rozhodli jste se uskutečnit svůj dlouho trvajcící sen a chcete si naprogramovat vlastní třídu, která bude reprezentovat řetězec. Standardní knihovna již obsahuje `std::string`, ale Vy mu moc nevěříte a proto jste se rozhodli, že si vytvoříte vlastní namespace `my_std` a v něm vlastní třídu `string` pro řetězce. Rozhraní by mělo být shodné s `std::string`, dále si ho můžete libovolně rozšířit a přidat nějaké vlastní privátní metody. Minimální požadované rozhraní je takové, že bude vyhovovat aktuálnímu použití, tedy takové, že nebude potřeba ŽÁDNÉ změny v již vytvořeném kódu, jen správné dodání deklarace a definice této třídy. Jediné třídy (soubory) v aktuálním projektu, které využívají `std::string`, jsou `SStudent` a `CTeacher`. Napište si i vlastní testy.

Při definování metod v implementačním souboru se Vám může hodit následující nápověda:
`char & my_std::string::operator[] ( size_t idx )`.
* Jméno operátoru se skládá z několika částí, kde každá část popisuje "cestu" k tomuto operátoru.
* `my_std` označuje jmenný prostor (námi označený jako namespace), ve kterém se někde nachází `string::operator[]`.
* `string` v našem případě již označuje jméno naší třídy.
* `operator[]` určuje název operátoru, který používáme. Samozřejmě bychom toto mohli nahradit za libovolný název nějaké jiné metody. 

V hlavičkovém souboru máte již některé deklarace přidané, nejsou to ale všechny vhodné metody, které by se mohly hodit (a ty navržené nemusí být správně).
Pro testování můžete využít volání `make run` a `make valgrind`. 

## Část druhá
Nyní jsme již plně spokojeni s naší implementací "vylepšeného" řetězce. Ve druhé části, abyste si dále vyzkoušeli, zdali jste opravdu kopírování a vytváření hlubokých kopií pochopili správně, musíte dodat možnosti kopírování a přesouvání obsahu instancí CList. Dříve jsme tyto možnosti potlačili klíčovým slovem `= delete`, nyní se k tomu ale vrátíme a naprogramujeme. Vhodně upravte hlavičkový soubor třídy `CList` a doplňte definice v implementačním souboru.

Jakmile budete mít tuto funkcionalitu hotovou, použijte volání `make copy`, které otevře dříve potlačené testy. Pokud Váš program nebude mít funkcionality naimplementované, program se nepodaří přeložit. Pokud vše definujete, program se zkompiluje a následně se spustí s valgrindem, abyste si byli jisti, že Váš program má očekávané chování.