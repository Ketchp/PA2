//#include "test.hpp"
#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <queue>
#include <stack>
#include <deque>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <memory>
using namespace std;
#endif /* __PROGTEST__ */

class fuzzyString: public string
{
public:
  explicit fuzzyString( string s ): string( move( s ) ) {};

  using string::string;

  struct wordParts
  {
    string prefix, postfix;
    char middle;
  };

  struct iterator
  {
    using iterator_category = input_iterator_tag;
    using value_type = wordParts;
    using pointer_type = const wordParts *;
    using reference_type = const wordParts &;

    // BEGIN iterator
    explicit iterator( const string &iterated )
            : iteratedString( iterated ),
              currentState{ { iterated.begin(), iterated.end() - !iterated.empty() },
                            "",
                            iterated.empty() ? '\0' : iterated[ iterated.size() - 1 ] }
    {};

    // END iterator
    iterator( const string &iterated, int )
            : iteratedString( iterated ),
              missingIdx( iterated.size() )
    {};

    reference_type operator*() const { return currentState; };
    pointer_type operator->() const { return &currentState; };

    iterator &operator++()
    {
      if( missingIdx == iteratedString.size() )
        return *this;
      ++missingIdx;
      currentState.prefix.pop_back();
      currentState.postfix.insert( 0, 1, iteratedString[  iteratedString.size() - missingIdx ] );
      currentState.middle = iteratedString[ iteratedString.size() - missingIdx - 1 ];
      return *this;
    };

    iterator operator++( int )
    {
      iterator temp( *this );
      ++*this;
      return temp;
    };

    const string &iteratedString;
    uint32_t missingIdx = 0;
    value_type currentState;
  };

  friend bool operator==( const iterator &lhs, const iterator &rhs )
  {
    return lhs.missingIdx == rhs.missingIdx &&
           &lhs.iteratedString == &rhs.iteratedString;
  };
  friend bool operator!=( const iterator &lhs, const iterator &rhs ) { return !( lhs == rhs ); };

  iterator begin() { return iterator( *this ); };
  iterator end() { return { *this, 0 }; };
};

class CDate
{
public:
  CDate ( int y, int m, int d ) noexcept;
  friend bool operator<( const CDate &lhs, const CDate &rhs );
  friend bool operator>(const CDate &lhs, const CDate &rhs) { return rhs < lhs; };
  friend bool operator<=(const CDate &lhs, const CDate &rhs) { return !( lhs > rhs ); };
  friend bool operator>=(const CDate &lhs, const CDate &rhs) { return !( lhs < rhs ); };

private:
  int year, month, day;
};


class CSupermarket
{
public:
  CSupermarket &store( const string &name, const CDate &expiryDate, int count );
  void sell( list< pair<string,int> > &shoppingList );
  [[nodiscard]] list< pair<string,int> > expired( const CDate &date ) const;


private:
  struct item
  {
    CDate expiryDate;
    int count;
  };

  friend bool operator<( const item &lhs, const item &rhs );
  friend bool operator>( const item &lhs, const item &rhs ) { return rhs < lhs; };
  friend bool operator<=( const item &lhs, const item &rhs ) { return !( lhs > rhs ); };
  friend bool operator>=( const item &lhs, const item &rhs ) { return !( lhs < rhs ); };

  friend bool operator<( const item &lhs, const CDate &rhs );
  friend bool operator>( const item &lhs, const CDate &rhs ) { return rhs < lhs; };
  friend bool operator<=( const item &lhs, const CDate &rhs ) { return !( lhs > rhs ); };
  friend bool operator>=( const item &lhs, const CDate &rhs ) { return !( lhs < rhs ); };

  friend bool operator<( const CDate &lhs, const item &rhs );
  friend bool operator>( const CDate &lhs, const item &rhs ) { return rhs < lhs; };
  friend bool operator<=( const CDate &lhs, const item &rhs ) { return !( lhs > rhs ); };
  friend bool operator>=( const CDate &lhs, const item &rhs ) { return !( lhs < rhs ); };


  [[nodiscard]] string resolveName( const string &name ) const;

  /**
   * @return true if requested count of items have been sold
   * @return false if not enough items in inventory
   */
  bool sellItem( const string &name, int &count );

  void removeItem( const string &name );

  static int countExpired( const vector<item> &items, const CDate &date );

  map< string, vector<item> > inventory;
  //   prefix:      postfix:  middle char
  map< string, map< string, set< char > > > typoResolver;
};

CDate::CDate( int y, int m, int d ) noexcept
  : year( y ), month( m ), day( d )
{}

bool operator<( const CDate &lhs, const CDate &rhs )
{
  if( lhs.year < rhs.year ) return true;
  if( lhs.year > rhs.year ) return false;

  if( lhs.month < rhs.month ) return true;
  if( lhs.month > rhs.month ) return false;

  if( lhs.day < rhs.day ) return true;
  return false;
}

bool operator<( const CSupermarket::item &lhs, const CSupermarket::item &rhs )
{
  return lhs.expiryDate > rhs.expiryDate;
}

bool operator<( const CSupermarket::item &lhs, const CDate &rhs )
{
  return lhs.expiryDate < rhs;
}

bool operator<( const CDate &lhs, const CSupermarket::item &rhs )
{
  return lhs < rhs.expiryDate;
}

CSupermarket &CSupermarket::store( const string &name, const CDate &expiryDate, int count )
{
  item newItem{ expiryDate, count };
  if( inventory.find( name ) == inventory.end() )
  {
    for( auto const &[ prefix, postfix, middle ]: fuzzyString( name ) )
    {
      typoResolver[ prefix ][ postfix ].insert( middle );
    }
  }
  vector<item> &items = inventory[ name ];
  items.insert( lower_bound( items.begin(), items.end(), newItem ), newItem );
  return *this;
}

void CSupermarket::sell( list< pair<string, int> > &shoppingList )
{
  vector< string > names;
  names.reserve( shoppingList.size() );
  for( const auto &[ name, count ]: shoppingList )
    names.emplace_back( resolveName( name ) );

  uint32_t idx = 0;
  for( auto currentRequest = shoppingList.begin();
       currentRequest != shoppingList.end(); ++idx )
  {
    if ( sellItem( names[ idx ] ,currentRequest->second ) )
      currentRequest = shoppingList.erase(currentRequest);
    else
      ++currentRequest;
  }
}

list< pair<string, int> > CSupermarket::expired( const CDate &date ) const
{
  list<pair<string, int>> result;
  for( const auto &[ name, items ]: inventory )
  {
    int count = countExpired( items, date );
    if( count )
      result.emplace_back( name, count );
  }
  result.sort( [](const pair<string, int> &lhs, const pair<string, int> &rhs)
  {
    return lhs.second > rhs.second;
  } );
  return result;
}

string CSupermarket::resolveName( const string &fuzzy ) const
{
  if( inventory.find( fuzzy ) != inventory.end() )
    return fuzzy;

  string match;
  for( const auto &[ prefix, postfix, middle ]: fuzzyString( fuzzy ) )
  {
    const auto prefixIterator = typoResolver.find( prefix );
    if( prefixIterator == typoResolver.end() )
      continue;

    const auto &postfixResolver = prefixIterator->second;
    const auto postfixIterator = postfixResolver.find( postfix );
    if( postfixIterator == postfixResolver.end() )
      continue;

    const set<char> &possibleChars = postfixIterator->second;
    if( possibleChars.empty() )
      continue;
    if( possibleChars.size() != 1 || !match.empty() )
      return fuzzy;
    match = fuzzy;
    match[ prefix.size() ] = *possibleChars.begin();
  }

  return match.empty() ? fuzzy : match;
}

bool CSupermarket::sellItem( const string &name, int &count )
{
  auto it = inventory.find( name );

  if( it == inventory.end() || !count )
    return !count;

  vector<item> &items = it->second;
  while( !items.empty() && count )
  {
    item &oldestItem = items.back();
    if( count < oldestItem.count )
    {
      oldestItem.count -= count;
      count = 0;
      break;
    }
    count -= oldestItem.count;
    items.pop_back();
  }

  if( items.empty() )
    removeItem( name );

  return !count;
}

void CSupermarket::removeItem( const string &name )
{
  for( const auto &[ prefix, postfix, middle ]: fuzzyString( name ) )
  {
    auto prefixIterator = typoResolver.find( prefix );
    if( prefixIterator == typoResolver.end() )
      continue;

    auto &postfixResolver = prefixIterator->second;
    auto postfixIterator = postfixResolver.find( postfix );
    if( postfixIterator == postfixResolver.end() )
      continue;

    set<char> &possibleChars = postfixIterator->second;
    possibleChars.erase( middle );
    if( !possibleChars.empty() )
      continue;
    postfixResolver.erase( postfixIterator );
    if( !postfixResolver.empty() )
      continue;
    typoResolver.erase( prefixIterator );
  }
  inventory.erase( name );
}

int CSupermarket::countExpired( const vector<item> &items, const CDate &date )
{
  int count = 0;
  for( auto current = items.crbegin();
        current != items.crend() && date > *current;
        ++current )
    count += current->count;
  return count;
}
