#include "wordle.hpp"
#include <iostream>
#include <string>
#include <array>

void clearOutput( int lines )
{
    for( int i = 0; i < lines; ++i )
        printf( "\x1b[A\x1b[2K" );
}

void printLetter(const CLetter & letter) {
    static const char symbols[] = { ' ', 'x', '?', '!' };
    switch ( letter.eval )
    {
    case EEvaluation::CORRECT:
        std::cout << "\x1B[32m";
        break;
    case EEvaluation::MISPLACED:
        std::cout << "\x1B[33m";
        break;
    case EEvaluation::WRONG:
        std::cout << "\x1B[30m";
        break;
    case EEvaluation::UNTRIED:
        std::cout << "\x1B[0m";
    }
    std::cout << letter.letter << "\x1B[0m ";
}

void printAlphabet(const CAlphabet & alphabet) {
    std::cout << "Stav pismen:" + std::string( 26, ' ' ) << std::endl;
    for(const CLetter & letter : alphabet) {
        printLetter(letter);
    }
    std::cout << std::endl;
}

void printResult(const CWordGuess & guess) {
    for(const CLetter & letter : guess) {
        printLetter(letter);
    }
    std::cout << std::endl;
}

void play(CPuzzle & puzzle) {
    while(!puzzle.isFinished()) {
        if( puzzle.getPastGuesses() )
        {
            clearOutput( puzzle.getPastGuesses() + 3 );
        }
        printAlphabet( puzzle.getAlphabet() );
        for( const CWordGuess &guess: puzzle.history() )
            printResult( guess );
        std::cout << (puzzle.getPastGuesses() + 1) << ". pokus. Zadejte slovo:" << std::endl;
        std::string word;
        if(! (std::cin >> word) ) { // co by se mohlo stát, kdybych zde tento if neměl a jen načítal?
            std::cout << "Sbohem... :_(" << std::endl;
            return;
        }
        for (auto & c: word) c = toupper(c);
        puzzle.guess(word);
    }
    if(puzzle.isWon()) {
        std::cout << "Vyborne! Zvladli jste to za " << puzzle.getPastGuesses() << " pokusu, ";
        if(puzzle.getRemainingGuesses() == 0) {
            std::cout << "na posledni pokus." << std::endl;
        }
        else {
            std::cout << "zbyvalo vam jeste " << puzzle.getRemainingGuesses() << " pokusu." << std::endl; 
        }
    }
    else {
        std::cout << "Prohrali jste :_( Hledane slovo bylo: " << puzzle.getWord() << std::endl;
    }
}

bool wantsToPlay() {
    std::cout << "Chcete dalsi hru? A/cokoliv jineho" << std::endl;
    char c;
    std::cin >> c;
    return (bool)std::cin && c == 'a' || c == 'A';
}

int main() {
    CWordle wordle;
    wordle.load("words.txt");
    do {
        CPuzzle puzzle = wordle.getPuzzle();
        play(puzzle);
    } while(wantsToPlay());
    wordle.unload();
    return 0;
}