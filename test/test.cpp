#include <string>
#include <map>
#include <vector>
#include <algorithm>

using namespace std;

template <typename T1_, typename T2_>
struct BonnieAndClyde
{
  BonnieAndClyde( T1_ first, T2_ second )
    : first( move(first) ), second( move(second) )
  {};

  BonnieAndClyde operator=( BonnieAndClyde &&other ) noexcept
  {
    first = move( other.first );
    second = move( other.second );
  }

  BonnieAndClyde( BonnieAndClyde &&other ) noexcept
    : first( move( other.first ) ),
      second( move( other.second ) )
  {};

  T1_ first;
  T2_ second;
};

struct CDate
{
  CDate( uint32_t a,
         uint32_t b)
    : a(a), b(b)
  {};

  bool operator<( const CDate &other ) const
  {
    return a == other.a ? b < other.b : a < other.a;
  }
  uint32_t a,b;
};

class CSup
{
private:
  map<string, vector<BonnieAndClyde<const CDate, uint32_t>>> s;
public:
  CSup &store( const string &name,
               const CDate &expir,
               const uint32_t cnt)
  {
    auto it = s.find(name);
    if( it == s.end() )
    {
      vector<BonnieAndClyde<const CDate, uint32_t>> tmp;
      tmp.emplace_back( expir, cnt );
      s.emplace(name, move(tmp));
    }
    else
    {
      auto &vec = it->second;
      auto pos = lower_bound(
              vec.begin(), vec.end(),
              expir,
              [](const auto &p,
                 const CDate &val)
                 { return p.first < val; }
              );
      vec.emplace( pos, expir, cnt );
    }
    return *this;
  }
};

int main()
{
  CSup s;
  s.store( "100", CDate( 1, 2), 100 );
  return 0;
}