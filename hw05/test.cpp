#include "test.hpp"


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
  for( const auto &[ name, _ ]: inventory )
  {
    if( !isTypo( name, fuzzy ) )
      continue;

    if( !match.empty() )
      return fuzzy;
    match = name;
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

bool CSupermarket::isTypo( const string &a, const string &b )
{
  if( a.size() != b.size() )
    return false;
  bool mismatch = false;
  for( uint32_t idx = 0; idx < a.size(); ++idx )
  {
    if( a[ idx ] == b[ idx ] )
      continue;
    if( mismatch )
      return false;
    mismatch = true;
  }
  return mismatch;
}
