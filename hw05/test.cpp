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
  if( inventory.find( name ) == inventory.end() )
  {
    typoResolver.resize( max( typoResolver.capacity(), name.size() ) );
    auto &resolver = typoResolver[ name.size() - 1 ];
    for( const auto &[fuzzy, idx]: fuzzyString( name ) )
    {
      if( resolver.size() != name.size() )
        resolver.resize( name.size() );
      resolver[ idx ].emplace( fuzzy, name[ idx ] );
    }
  }

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

string CSupermarket::resolveName( const string &name ) const
{
  if( inventory.find( name ) != inventory.end() || typoResolver.size() < name.size() )
    return name;

  string match;

  auto &resolver = typoResolver[ name.size() - 1 ];
  for( const auto &[ fuzzy, idx ] : fuzzyString( name ) )
  {
    auto it = resolver[ idx ].lower_bound( fuzzy );
    if( it == resolver[ idx ].end() || it->first != fuzzy )
      continue;

    if( !match.empty() )
      return name;

    match = name;
    match[ idx ] = it->second;
    if( ++it != resolver[ idx ].end() && it->first == fuzzy )
      return name;
  }
  return match.empty() ? name : match;
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
  vector< multimap< string, char > > &resolver = typoResolver[ name.size() - 1 ];

  for( const auto &[ fuzzy, idx ] : fuzzyString( name ) )
    for( auto [ range_start, range_end ] = resolver[idx].equal_range( fuzzy );
          range_start != range_end; )
    {
      string original = range_start->first;
      original.insert( idx, 1, range_start->second );
      if( original == name )
        range_start = resolver[ idx ].erase( range_start );
      else
        ++range_start;
    }
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
