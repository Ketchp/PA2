#pragma once

#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

enum class EEvaluation {
    UNTRIED, 
    WRONG, 
    MISPLACED,
    CORRECT,
};

struct CLetter
{
    CLetter( char letter = ' ', EEvaluation eval = EEvaluation::UNTRIED );
    char letter;
    EEvaluation eval;
};

// CLetter
// - letter
// - evaluation

using CWordGuess = std::array<CLetter, 5>;
using CAlphabet = std::array<CLetter, 26>;

// CPuzzle
// - setWord(...) ?            - nastaví hádané slovo
// - ... isLost()              - zda hra skončila bez uhádnutí slova

// CWordle
// - load(...)                 - načte slovník ze souboru
// - unload()                  - uvolní slovník z paměti
// - ... getPuzzle()           - vytvoří zadání s náhodným slovem ze slovníku (případně s nějakým vámi pevně stanoveným slovem)  


class CPuzzle
{
public:
    CPuzzle( const std::string &&word );
    bool isFinished() const;          //- zda hra skončila (došly pokusy, slovo bylo uhádnuto)
    CAlphabet getAlphabet() const;    //- vrátí stavy jednotlivých písmen v abecedě jako pole 26 výsledků
    int getPastGuesses() const;       //- vrátí, kolikrát už se hádalo
    CWordGuess guess( const std::string &word ); //- vyhodnotí zadané slovo, vrátí stavy jednotlivých zadaných písmen jako pole 5 výsledků
    bool isWon() const;               //- zda bylo slovo uhádnuto
    int getRemainingGuesses() const;  //- vrátí, kolikrát se ještě může hádat
    std::string getWord() const;      //- zjistí hádané slovo
    std::vector<CWordGuess> history() const;

private:
    CAlphabet alphabet;
    CWordGuess guessWord;
    std::vector<CWordGuess> pastGuesses;
};


class CWordle
{
public:
    CWordle();
    void load( const std::string &&name );
    void unload();
    std::string getRandWord() const;
    CPuzzle getPuzzle() const;

private:
    std::vector<std::string> words;
};

