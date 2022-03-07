#include "wordle.hpp"

CWordle::CWordle()
{
    srand(time(NULL));
};

void CWordle::load( const std::string &&name )
{
    std::ifstream file(name);
    if( !file.good() ) return;
    while( true )
    {
        std::string word;
        file >> std::setw( 5 ) >> word;
        if( file.eof() ) break;
        for (auto & c: word) c = toupper(c);
        words.push_back( word );
    }
}

void CWordle::unload()
{
    words.clear();
}

std::string CWordle::getRandWord() const
{
    return words[ rand() % words.size() ];
}

CPuzzle CWordle::getPuzzle() const
{
    return CPuzzle( getRandWord() );
}

CPuzzle::CPuzzle( const std::string &&word )
{
    for( size_t i = 0; i < alphabet.size(); ++i )
        alphabet[i].letter = 'A' + i;

    for( size_t i = 0; i < guessWord.size(); ++i )
        guessWord[i].letter = word[i];
}

bool CPuzzle::isFinished() const
{
    if(  pastGuesses.size() >= 6 ) return true;
    for( CLetter letter: guessWord )
    {
        if( letter.eval != EEvaluation::CORRECT ) return false;
    }
    return true;
}

CAlphabet CPuzzle::getAlphabet() const
{
    return alphabet;
}

int CPuzzle::getPastGuesses() const
{
    return pastGuesses.size();
}

CWordGuess CPuzzle::guess( const std::string &word )
{
    CWordGuess newGuess;
    for( size_t i = 0; i < 5; ++i )
    {
        newGuess[i] = CLetter( word[i], EEvaluation::WRONG );
        alphabet[ word[i] - 'A' ].eval = EEvaluation::WRONG;
        guessWord[i].eval = EEvaluation::WRONG;
        for( size_t j = 0; j < 5; ++j )
        {
            if( word[i] != guessWord[j].letter ) continue;
            if( i == j )
            {
                newGuess[i].eval = EEvaluation::CORRECT;
                alphabet[ word[i] - 'A' ].eval = EEvaluation::CORRECT;
                guessWord[i].eval = EEvaluation::CORRECT;
                continue;
            }
            newGuess[i].eval = EEvaluation::MISPLACED;
            alphabet[ word[i] - 'A' ].eval = EEvaluation::MISPLACED;
        }
    }
    pastGuesses.push_back( newGuess );
    return newGuess;
}

bool CPuzzle::isWon() const
{
    if( pastGuesses.size() > 6 ) return false;
    for( CLetter letter: guessWord )
    {
        if( letter.eval != EEvaluation::CORRECT ) return false;
    }
    return true;
}

int CPuzzle::getRemainingGuesses() const
{
    return 6 - getPastGuesses();
}

std::string CPuzzle::getWord() const
{
    std::string word( 5, ' ' );
    for( size_t i = 0; i < 5; ++i )
    {
        word[i] = guessWord[i].letter;
    }
    return word;
}

std::vector<CWordGuess> CPuzzle::history() const
{
    return pastGuesses;
}

CLetter::CLetter( char letter, EEvaluation eval )
    :letter(letter), eval(eval)
{

}
