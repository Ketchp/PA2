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

  struct iterator
  {
    using iterator_category = input_iterator_tag;
    using difference_type = int32_t;
    using value_type = pair<const string&, int>;
    using pointer = pair<const string*, int>;
    using reference = pair<const string&, int>;

    // BEGIN iterator
    explicit iterator( const string &iterated )
            : currentFuzzy( !iterated.empty() ? iterated.begin() + 1 : iterated.begin(),
                            iterated.end() ),
              iteratedString( iterated )
    {};

    // END iterator
    iterator( const string &iterated, int )
            : iteratedString( iterated ),
              missingIdx( iterated.size() )
    {};

    reference operator*() const { return { currentFuzzy, missingIdx }; };

    pointer operator->() const { return { &currentFuzzy, missingIdx }; };

    iterator &operator++()
    {
      if( missingIdx == iteratedString.size() )
        return *this;

      currentFuzzy[ missingIdx ] = iteratedString[ missingIdx ];
      ++missingIdx;
      return *this;
    };

    iterator operator++( int )
    {
      iterator temp( *this );
      ++*this;
      return temp;
    };

    string currentFuzzy;
    const string &iteratedString;
    uint32_t missingIdx = 0;
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
  vector< vector< multimap< string, char > > > typoResolver;
};