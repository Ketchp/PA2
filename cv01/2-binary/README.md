# Binární soubory v C++

Úloha pro pokročilé, řešená samostatně při cvičení a bodovaná za aktivitu. Úloha je dobrým procvičením k 1. domácí úloze na progtestu (od 3. týdne).

## Zadání

Vaším úkolem (za bod na aktivitu) bude napsat v C++ program, který zpracovává binární soubory. Konkrétně půjde o obrazový formát TGA v základní variantě (nekomprimované, bez barevných palet apod.), který si jednak zkusíte vůbec načíst, jednak zapsat.

Konkrétně napište tyto funkce:

```cpp
struct Image { /*...*/ };

// načte obrázek do paměti
Image imageInit(const char * fileName);

// zruší data, uvolní paměť...
void imageDestroy(/*...*/ image);

// vrátí šířku obrázku
int imageWidth(/*...*/ image);

// vrátí výšku obrázku
int imageHeight(/*...*/ image);

// vytvoří obrázek kruhu zadané barvy (r, g, b) na bílém pozadí o zadaném poloměru radius.
Image imageCircle(int radius, int r, int g, int b);

// otočí obrázek o zadaný úhel (povolené hodnoty jsou 0, 90, 180 a 270 stupňů)
void imageRotate(/*...*/ image, int angle);

// uloží obrázek do souboru
void imageSave(/*...*/ image, const char * fileName);
```

Využijte alespoň tyto možnosti jazyka C++:

* struktury
* proudy (`std::ifstream`, `std::ofstream`)
* operátory `new`/`delete`

Pokud jste velmi zkušení, můžete implementovat funkce jako metody třídy Image, případně i využít konstruktor / destruktor.

Stačí nám řešení bez rotace, za půl bodu za aktivitu. S rotací dostanete celý bod.

## Pokyny

### Práce s binárními soubory

Využijte datové proudy `ifstream` (pro čtení) a `ofstream` (pro zápis). Lze využít fstream, ale musíte správně nastavit režim.

Příklad:

```cpp
#include <fstream>

int main() {
    std::ifstream f("cesta/k/souboru/x"); // vytvoří proměnnou f typu ifstream, inicializuje ji cestou k souboru
    // šlo by též napsat std::ifstream f = "...";
    // konkrétní význam uvedeného zápisu budeme řešit v pozdějších týdnech
    
    // v momentě inicializace proměnné f se daný soubor otevře
    
    // ...
    
    return 0;
    // v momentě opuštění funkce dojde k destrukci f, při které se soubor uzavře
    // dojde tak např. k dokončení zápisu (pokud bychom měli f typu ofstream)
}
```

Při práci s binárními soubory nepoužívejte formátovaný vstup / výstup (přetížené operátory `>>` a `<<`), ten se hodí na textový vstup / výstup.
Místo toho používejte k tomu určené metody (= členské funkce):

```cpp
#include <fstream>

int main() {
    std::ifstream in("ctenysoubor.bin");
    std::ifstream out("zapsanysoubor.bin");
    
    // po 1 bajtu... (může být pomalé)
    // proměnná in má členskou funkci (neboli metodu) get
    char a = in.get(); // přečte 1 bajt ze vstupu
    // out má zase metodu put
    out.put(a); // zapíše 1 bajt na výstup
    
    // více bajtů naráz
    char buffer[10];
    in.read(buffer, 10); // zapíše do pole buffer 10 bajtů přečtených z in
    out.write(buffer, 10); // zapíše do out 10 bajtů z pole buffer
    
    return 0;
}
```

Můžeme využít toho, že umíme přečíst/zapsat více bajtů naráz, a místo pole bajtů poslat nějakou strukturu nebo jiný datový typ.
K tomu bude potřeba převést pointer na danou strukturu na `char *`. V C by se dal napsat `(char *)ptr`. Ale v C++ používáme `reinterpret_cast<char *>(...)`.
Zápis je to sice delší, ale o to více řve, děláme nebezpečnou operaci s pamětí, a je více zřejmý záměr programátora.
V tomto případě neprovádíme žádnou konverzi dat, jen říkáme kompilátoru, že daná data má chápat jinak, ale nic by kvůli tomu neměl dělat navíc.

```cpp
#include <fstream>
#include <iostream>

int main() {
    std::ifstream in("ctenysoubor.bin");
    
    int i;
    in.read(reinterpret_cast<char*>(&i), sizeof(i));
    // zapíše do místa v paměti, kde je proměnná i, 4 bajty z in
    // na x86/x64 máme little endian architekturu; pokud má binární formát
    // specifikaci, že čísla jsou little endian, nemusíme dělat žádné převody
    // jak jsme data načetli, tak s nimi můžeme pracovat
    // kdybychom měli big endian systém a little endian data (nebo naopak)
    // museli bychom přehodit bajty, např. pomocí bitových operací, nebo pomocí std::swap()
    
    // hodnotu i si samozřejmě můžeme i normálně vypsat
    std::cout << "i = " << i << std::endl;
    
    // existují i alternativy k reinterpret_cast, např. využití union
    // ale s union mohou být potom problémy v C++ při konstrukcích/kopiích objektů apod.
    union {
        int j;
        char c[sizeof(int)];
    };
    in.read(c, sizeof(int));
    std::cout << "j = " << j << std::endl;
    
    return 0;
}
```

Tímto způsobem lze i zapisovat/číst celé struktury. Musíte si ale samozřejmě dát pozor na velikosti a polohy členských proměnných, aby skutečně odpovídaly specifikaci daného formátu.

Dalšími zajímavými metodami jsou `seekg` a `seekp`, kterými lze skákat uvnitř souboru na konkrétní pozice (adresy, offsety atd.).

### Datový formát TGA

Jde o jeden z nejjednodušších binárních formátů, které zároveň může začátečník snadno využít. Soubor se skládá z hlavičky, potom z nějakých volitelných bloků (které ale v této úloze používat nebudeme), a potom z obrazových dat.

My si vybereme podmnožinu formátu, bez kompresí, bez barevných palet, s přímou reprezentací jednotlivých pixelů obrázku.

Hlavička TGA vypadá takto:

<table>
<tr><th>Offset</th><th>Název</th><th>Velikost (B)</th><th>Význam a datový typ</th></tr>
<tr><td>0</td><td>ID length</td><td>1</td><td>Délka volitelného ID bloku v bajtech (umístěn za hlavičkou). Uvažujte 0.</td></tr>
<tr><td>1</td><td>Color map type</td><td>1</td><td>Typ barevné palety. Umisťuje se za ID blok, my ale žádnou používat nebudeme, takže to bude 0.</td></tr>
<tr><td>2</td><td>Image type</td><td>1</td><td>Typ obrázku a komprese. My použijeme hodnotu 2, nekomprimovaný true-color obrázek.</td></tr>
<tr><td>3</td><td>Color map specification</td><td>5</td><td>Detaily barevné palety. Nezajímají nás, klidně samé 0.</td></tr>
<tr><td>8</td><td>X-ový počátek</td><td>2</td><td>16bitové celé číslo (short) udávající vodorovnou polohu "počátku" obrázku. Nezajímá nás, použijeme 0.</td></tr>
<tr><td>10</td><td>Y-ový počátek</td><td>2</td><td>16bitové celé číslo (short) udávající svislou polohu "počátku" obrázku. Nezajímá nás, použijeme 0.</td></tr>
<tr><td>12</td><td>Image width</td><td>2</td><td>16bitové celé číslo (short) udávající šířku obrázku.</td></tr>
<tr><td>14</td><td>Image height</td><td>2</td><td>16bitové celé číslo (short) udávající výšku obrázku.</td></tr>
<tr><td>16</td><td>Pixel depth</td><td>1</td><td>Počet bitů na pixel. Budeme uvažovat hodnotu 24.</td></tr>
<tr><td>17</td><td>Image descriptor</td><td>1</td><td>4. a 5. bit popisují překlopení obrázku podle X/Y osy. V ukázkových datech je nemáme, použijeme 0.</td></tr>
</table>

Dále následují obrazová data. Jednotlivé řádky za sebou, v každém řádku za sebou jednotlivé pixely, každý pixel ze 3 bajtů (protože máme 24bpp hloubku). 3 bajty pixelu reprezentují jednotlivé barevné složky v pořadí BGR (modrá, zelená, červená).

Všechna čísla jsou little endian (tedy prakticky pro vás: nemusíte je na zdejších počítačích otáčet).

### Další tipy

Image nemusí být jediná struktura ve Vašem kódu.

Můžete s obrázkem pracovat jako s 1D polem nebo s 2D polem, je to na vás. (Každý přístup má svá pro a proti.)
* Pro 1D pole se vyplatí napsat si funkci na mapování souřadnic; výhodou je možnost načtení celého souboru nadvakrát (hlavička + obsah)
* Pro 2D pole si musíte hlídat jeho správnou alokaci; nevýhodou je nutnost číst soubor po řádcích, výhodou přehlednější zápis operací

Otáčení lze implementovat přímo, ale také lze rozložit do několika snáze řešitelných kroků (transpozice matice, zrcadlení).

Pokud používáte IDE, nezapomeňte v konfiguraci spuštění vašeho programu nastavit správně pracovní adresář. Jinak to přiložené soubory nenajde!

Pro výpis binárních souborů se obvykle hodí mít po ruce hexadecimální reprezentaci, kde každý bajt tvoří dvě hexadecimální číslice. Lze využít příkaz `hexdump -C <soubor>`, který vypíše soubor v klasickém formátu offset, bajty v hexadecimální podobě, jejich ASCII interpretace.

Obrázky TGA lze vygenerovat např. knihovnou ImageMagick, příkazem `convert <obrázek> +matte <obrázek.tga>` (nemusí být dostupný v učebně).

Validitu obrázku vám prověří většina prohlížečů obrázků (webové prohlížeče ani např. CLion bohužel ne).

I když nestihnete úlohu vyřešit na hodině, zkuste si to doma. Můžete také zkusit přidat podporu pro RLE kompresi (image type = 10), která se hodí na jednolité barevné plochy.  

### Závěrečná poznámka k řešení

Řešení této úlohy záměrně nebudeme zveřejňovat. Zpracování binárních souborů je 1. domácí úloha na progtestu, a v některých letech jsou tématem obrázky, v jiných letech jsou to archivy, MKV kontejner apod. Je tedy možné, že bychom zveřejněním řešení způsobili nějakou nezamýšlenou částečnou kolizi.

V tomto dokumentu máte ale plno užitečných rad, jak s binárními soubory pracovat.

### Odkazy

Popis formátu TGA: https://www.gamers.org/dEngine/quake3/TGA.txt
