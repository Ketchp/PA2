#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <set>
#include <list>
#include <map>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <functional>
#include <memory>
#include <numeric>
#include <optional>
#include <variant>
#include <any>
using namespace std;
#endif /* __PROGTEST__ */

template <class T>
bool default_comp( const T &rhs, const T &lhs )
{
  return rhs < lhs;
}

template <typename T_, typename C_ = less< typename T_::value_type > >
class CIndex
{
public:
  explicit CIndex( T_ haystack, C_ comp = C_{} )
    : haystack( move( haystack ) ), comp( comp )
  {};

  set<size_t> search( const T_ &needle ) const
  {
    set<size_t> matches;
    size_t position = 0;
    auto haystack_current = haystack.begin();
    while( haystack_current != haystack.end() )
    {
      auto haystack_offset = haystack_current;
      auto needle_current = needle.begin();
      while( needle_current != needle.end() )
      {
        if( !areEqual( *needle_current, *haystack_offset ) )
          break;
        ++needle_current;
        ++haystack_offset;
      }
      if( needle_current == needle.end() )
        matches.insert( position );
      ++haystack_current;
      ++position;
    }
    return matches;
  };

  bool areEqual( const typename T_::value_type &lhs,
                 const typename T_::value_type &rhs ) const
  {
    return !comp( lhs, rhs ) && !comp( rhs, lhs );
  }

private:
  T_ haystack;
  C_ comp;
};
