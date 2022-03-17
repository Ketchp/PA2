#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cmath>
#include <iostream> 
#include <iomanip> 
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <memory>
using namespace std;
#endif /* __PROGTEST__ */

class Heap
{
public:
  // assuming transitivity, order( extracted, other ) will be true
  // for every element still in Heap when extracting
  Heap( bool( *order )( const unsigned int, const unsigned int ) )
//    : order( maxFunc )
  {
    maxFunc = order;
  };

  unsigned int peak() const
  {
    return data.size() ? data[0] : 0;
  };

  unsigned int pop()
  {
    if( !data.size() ) return 0;
    unsigned int res = data[ 0 ];
    if( data.size() > 1 )
      data[ 0 ] = data.back();
    data.pop_back();
    sinkDown( 0 );
    return res;
  };

  void insert( unsigned int value )
  {
    data.push_back( value );
    bubbleUp( data.size() - 1 );
  };

  size_t size() const
  {
    return data.size();
  };

private:
  vector<unsigned int> data;
  bool ( *maxFunc )( const unsigned int lhs, const unsigned int rhs );
  void bubbleUp( size_t idx )
  {
    if( idx == 0 ) return;
    if( !maxFunc( data[ ( idx - 1 ) / 2 ], data[ idx ] ) )
    {
      swap( data[ idx ], data[ ( idx - 1 ) / 2 ]);
      bubbleUp( ( idx - 1 ) / 2 );
    }
  };

  void sinkDown( const size_t idx )
  {
    // bottom
    if( data.size() <= idx * 2 + 1 ) return;

    // only one child
    if( data.size() == idx * 2 + 2 )
    {
      // is child in order
      if( !maxFunc( data[ idx ], data[ idx * 2 + 1 ] ) )
        swap( data[ idx ], data[ idx * 2 + 1 ] );
      return;
    }
    // correct order
    if( maxFunc( data[ idx ], data[ idx * 2 + 1 ] ) &&
        maxFunc( data[ idx ], data[ idx * 2 + 2 ] ) ) return;
    // swap with bigger one
    size_t next = maxFunc( data[ idx * 2 + 1 ], data[ idx * 2 + 2 ] ) ? idx * 2 + 1 : idx * 2 + 2;

    swap( data[ idx ], data[ next ] );
    sinkDown( next );
  };

  static void swap( unsigned int &a, unsigned int &b )
  {
    unsigned int temp = a;
    a = b;
    b = temp;
  };
};

template <typename T_key, typename T_value>
class AVLTree
{
public:
  AVLTree( int( *compFunc )( const T_key &, const T_key &) )
    : root( nullptr ), m_size( 0 )
//    : compare( compFunc )
  {
    compare = compFunc;
  };
  ~AVLTree()
  {
    delete root;
  };
  bool contain( const T_key &needle ) const
  {
    return find( needle );
  };

  T_value at( const T_key &needle ) const
  {
    node *leaf = find( needle );
    if( leaf ) return leaf->value;
    return T_value();
  };

  void insert( const T_key &key, const T_value &value )
  {
    root = addNode( root, key, value );
  };

  void remove( const T_key &key )
  {
    root = deleteNode( root, key );
  };

  size_t size() const
  {
    return m_size;
  };

  bool first( T_key &start ) const
  {
    return first( start, root );
  };

  bool next( T_key &currentNeedle ) const
  {
    return next( currentNeedle, root );
  };

private:
  struct node
  {
    node *left; 
    node *right;
    T_key key;
    T_value value;
    int height;

    node( T_key key, T_value value )
      : left( nullptr ), right( nullptr ), key( key ), value( value ), height( 1 )
    {};

    ~node()
    {
      delete left;
      delete right;
    };

    node &operator=( node &&other )
    {
      left = other.left;
      right = other.right;
      key = move( other.key );
      value = move( other.value );
      height = other.height;
      other.left = other.right = nullptr;
      return *this;
    };

    node &steal( node &victim )
    {
      key = move( victim.key );
      value = move( victim.value );
      return *this;
    }

    node &copyKeyValue( node &other )
    {
      key = other.key;
      value = other.value;
      return *this;
    }

    void updateHeight()
    {
      const int heightL = left ? left->height : 0;
      const int heightR = right ? right->height : 0;
      height = 1 + max( heightL, heightR );
    }

    int balance() const
    {
      const int heightL = left ? left->height : 0;
      const int heightR = right ? right->height : 0;
      return heightL - heightR;
    }

    /*   a      b
     *  /        \
     * b   --->   a
     *  \        /
     *   c      c
     */
    node *rightRotate()
    {
      node *a = this;
      node *b = left;
      node *c = left->right;

      a->left = c;
      b->right = a;
      a->updateHeight();
      b->updateHeight();
      return b;
    };

    /* a          b
     *  \        /
     *   b ---> a
     *  /        \
     * c          c
     */
    node *leftRotate()
    {
      node *a = this;
      node *b = right;
      node *c = right->left;

      a->right = c;
      b->left = a;
      a->updateHeight();
      b->updateHeight();
      return b;
    };
  };

  bool first( T_key &start, node *top ) const
  {
    if( !top ) return false;
    if( top->left )
      return first( start, top->left );
    start = top->key;
    return true;
  };

  bool next( T_key &currentNeedle, node *top ) const
  {
    if( !top ) return false;
    const int c = compare( top->key, currentNeedle );
    if( c > 0 )
    {
      if( !next( currentNeedle, top->left ) )
        currentNeedle = top->key;
      return true;
    }
    else if( c == 0 )
      return first( currentNeedle, top->right );
    else
      return next( currentNeedle, top->right );
  };

  node *addNode( node *top, const T_key &key, const T_value value )
  {
    if( !top )
    {
      ++m_size;
      return new node( key, value );
    }
    const int c = compare( top->key, key );
    if( c < 0 )
      top->right = addNode( top->right, key, value );
    else if( c > 0 )
      top->left = addNode( top->left, key, value );
    else
    {
      top->value = value;
      return top;
    }
    top->updateHeight();

    const int balance = top->balance();

    if( balance > 1 && compare( key, top->left->key ) < 0 )
      return top->rightRotate();
    if( balance < -1 && compare( key, top->right->key ) > 0 )
      return top->leftRotate();

    if( balance > 1 && compare( key, top->left->key ) > 0 )
    {
      top->left = top->left->leftRotate();
      return top->rightRotate();
    }
    if( balance < -1 && compare( key, top->right->key ) < 0 )
    {
      top->right = top->right->rightRotate();
      return top->leftRotate();
    }

    return top;
  };

  node *deleteNode( node* top, const T_key &key )
  {
    if( !top )
      return top;
    
    const int c = compare( top->key, key );
    if( c < 0 )
      top->right = deleteNode( top->right, key );
    else if( c > 0 )
      top->left = deleteNode( top->left, key );
    else
    {
      --m_size;
      if( !top->left || !top->right )
      {
        node *ancestor = top->left ? top->left : top-> right;
        if( !ancestor )
        {
          delete top;
          return nullptr;
        }
        *top = move( *ancestor );
        delete ancestor;
      }
      else
      {
        node *smallest = top->right;
        while( smallest->left )
          smallest = smallest->left;

        top->copyKeyValue( *smallest );

        top->right = deleteNode( top->right, smallest->key );
      }
    }
    if( !top )
      return top;
    top->updateHeight();

    if( top->balance() > 1 && top->left->balance() >= 0 )
      return top->rightRotate();
    if( top->balance() < -1 && top->right->balance() <= 0 )
      return top->leftRotate();

    if( top->balance() > 1 && top->left->balance() < 0 )
    {
      top->left = top->left->leftRotate();
      return top->rightRotate();
    }
    if( top->balance() < -1 && top->right->balance() > 0 )
    {
      top->right = top->right->rightRotate();
      return top->leftRotate();
    }
    return top;
  };

  node *find( const T_key &needle ) const
  {
    node *current = root;
    while( current )
    {
      const int c = compare( current->key, needle );
      if( c < 0 ) current = current->right;
      else if( c > 0 ) current = current->left;
      else break;
    }
    return current;
  }

  int ( *compare )( const T_key &, const T_key & );
  node *root;
  size_t m_size;
};

class CVATRegister
{
public:
  CVATRegister( void )
    : nameRecords( nameCompare ), idRecords( idCompare ),
      topHeap( minFunc ), bottomHeap( maxFunc )
  {};

  ~CVATRegister( void )
  {
    string id;
    if( !idRecords.first( id )) return;

    do
    {
      delete idRecords.at( id );
    } while( idRecords.next( id ) );
  };

  bool newCompany( const string &name, const string &addr, const string &taxID )
  {
    if( idRecords.contain( taxID ) || nameRecords.contain( pair<string, string>( name, addr ) ) ) return false;
    Company *targetCompany = new Company{ name, addr, taxID };
    nameRecords.insert( pair<string, string>( name, addr ), targetCompany );
    idRecords.insert( taxID, targetCompany );
    return true;
  };

  bool cancelCompany( const string &name, const string &addr )
  {
    Company *targetCompany = nameRecords.at( pair<string, string>( name, addr ) );
    return cancelCompany( targetCompany );
  };
  
  bool cancelCompany( const string &taxID )
  {
    Company *targetCompany = idRecords.at( taxID );
    return cancelCompany( targetCompany );
  };

  bool invoice( const string &taxID, unsigned int amount )
  {
    Company *targetCompany = idRecords.at( taxID );
    return invoice( targetCompany, amount );
  };

  bool invoice( const string &name, const string &addr, unsigned int amount )
  {
    Company *targetCompany = nameRecords.at( pair<string, string>( name, addr ) );
    return invoice( targetCompany, amount );
  };

  bool audit( const string &name, const string &addr, unsigned int &sumIncome ) const
  {
    Company *targetCompany = nameRecords.at( pair<string, string>( name, addr ) );
    return audit( targetCompany, sumIncome );
  };

  bool audit( const string &taxID, unsigned int &sumIncome ) const
  {
    Company *targetCompany = idRecords.at( taxID );
    return audit( targetCompany, sumIncome );
  };

  bool firstCompany( string &name, string &addr ) const
  {
    pair<string, string> first;
    if( !nameRecords.first( first ) ) return false;
    name = first.first;
    addr = first.second;
    return true;
  };

  bool nextCompany( string &name, string &addr ) const
  {
    pair<string, string> current( name, addr );
    if( !nameRecords.next( current ) ) return false;
    name = current.first;
    addr = current.second;
    return true;
  };

  unsigned int medianInvoice( void ) const
  {
    return topHeap.peak();
  };

private:
  struct Company
  {
    string name, address, id;
    unsigned long invoiceSum;
  };

  bool cancelCompany( Company *target )
  {
    if( !target ) return false;
    nameRecords.remove( pair<string,string>( move(target->name), move(target->address) ) );
    idRecords.remove( target->id );
    delete target;
    return true;
  };

  bool invoice( Company *target, unsigned int amount )
  {
    if( !target ) return false;
    target->invoiceSum += amount;
    updateMedian( amount );
    return true;
  };

/* 
 * +-----------+-------------------+----------------------------+-------------------+
 * |   sizes   | amount >= top.min | top.min > amount > bot.max | bot.max >= amount |  
 * +-----------+-------------------+----------------------------+-------------------+     
 * | top > bot |  top.min -> bot   |       amount -> bot        |   amount -> bot   |
 * |           |   amount -> top   |                            |                   |
 * +-----------+-------------------+----------------------------+-------------------+
 * | top = bot |   amount -> top   |       amount -> top        |  bot.max -> top   |
 * |           |                   |                            |   amount -> bot   |
 * +-----------+-------------------+----------------------------+-------------------+
 */
  void updateMedian( unsigned int amount )
  {
    if( topHeap.size() > bottomHeap.size() )
    {
      if( amount >= topHeap.peak() )
      {
        bottomHeap.insert( topHeap.pop() );
        topHeap.insert( amount );
      }
      else bottomHeap.insert( amount );
    }
    else
    {
      if( !bottomHeap.size() || amount > bottomHeap.peak() )
        topHeap.insert( amount );
      else
      {
        topHeap.insert( bottomHeap.pop() );
        bottomHeap.insert( amount );
      }
    }
  };


  bool audit( Company *target, unsigned int &sumIncome ) const
  {
    if( !target ) return false;
    sumIncome = target->invoiceSum;
    return true;
  };

  static int nameCompare( const pair<string,string> &lhs, const pair<string,string> &rhs )
  {
    if( caseInsCompare( lhs.first, rhs.first ) )
      return caseInsCompare( lhs.first, rhs.first );
    return caseInsCompare( lhs.second, rhs.second );
  };

  static int caseInsCompare( const string &lhs, const string &rhs )
  {
    auto itL = lhs.cbegin(), itR = rhs.cbegin();
    while( itL != lhs.cend() && itR != rhs.cend() )
    {
      if( tolower( *itL ) < tolower( *itR ) ) return -1;
      if( tolower( *itL ) > tolower( *itR ) ) return 1;
      ++itL; ++itR;
    }
    return (itL != lhs.cend()) - (itR != rhs.cend());
  };

  static int idCompare( const string &lhs, const string &rhs )
  {
    return lhs.compare( rhs );
  };

  static bool minFunc( const unsigned int lhs, const unsigned int rhs )
  {
    return lhs <= rhs;
  };

  static bool maxFunc( const unsigned int lhs, const unsigned int rhs )
  {
    return lhs >= rhs;
  };

  AVLTree<pair<string, string>, Company *> nameRecords;
  AVLTree<string, Company *> idRecords;
  Heap topHeap, bottomHeap;
};

