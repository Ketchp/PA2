#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

#include <algorithm>
#include <functional>

#include <stdexcept>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cstdint>
#include <cctype>
#include <cassert>
using namespace std;

template <typename key_T>
class CSet
{
public:
  // default constructor
  CSet() = default;

  // copy constructor
  CSet( const CSet &other )
    : size( other.size )
  {
    top = new Node( *other.top );
  };

  // operator=
  CSet &operator=( const CSet &other )
  {
    if( this == &other )
      return *this;
    delete top;
    top = new Node( *other.top );
    size = other.size;
    return *this;
  };

  // destructor
  ~CSet()
  {
    delete top;
  };

  // Insert
  bool Insert( key_T key )
  {
    if( top )
    {
      auto [ newTop, result ] = top->insert( move( key ) );
      top = newTop;
      size += result;
      return result;
    }
    top = new Node( move( key ) );
    size = 1;
    return true;
  };

  // Remove
  bool Remove( const key_T &key )
  {
    if( !top )
      return false;
    auto [ newTop, deletedNode ] = top->remove( key );
    top = newTop;
    bool success = deletedNode;
    size -= success;
    delete deletedNode;
    return success;
  };

  // Contains
  bool Contains( const key_T &key ) const
  {
    if( !top )
      return false;
    return top->contains( key );
  };

  // Size
  uint32_t Size() const
  {
    return size;
  };
private:
  struct Node
  {
    // leaf constructor
    explicit Node( key_T &&key )
      : key( key )
    {};

    Node( const Node &other )
      : left( other.left ? new Node( *other.left ) : nullptr ),
        right( other.right ? new Node( *other.right ) : nullptr ),
        height( other.height ),
        key( other.key )
    {};

    Node &operator=( const Node &other )
    {
      if( this == &other )
        return *this;
      delete left;
      delete right;
      key = other.key;
      left = new Node( *other.left );
      right = new Node( *other.right );
      height = other.height;
      return *this;
    };

    ~Node()
    {
      delete left;
      delete right;
    };

    pair< Node *, bool > insert( key_T &&newKey )
    {
      int comp = compareKeys( key, newKey );
      if( comp == 0 )
        return { this, false };
      bool result = true;
      if( comp < 0 )
      {
        if( right )
        {
          auto [ newRight, success ] = right->insert( move( newKey ) );
          right = newRight;
          result = success;
        }
        else
          right = new Node( move( newKey ) );
      }
      else
      {
        if( left )
        {
          auto [ newLeft, success ] = left->insert( move( newKey ) );
          left = newLeft;
          result = success;
        }
        else
          left = new Node( move( newKey ) );
      }

      return { balanceTree(), result };
    };

    // ( new top, extracted node )
    pair< Node *, Node * > remove( const key_T &target )
    {
      int32_t comp = compareKeys( key, target );
      Node *deletedNode;
      if( comp < 0 )
      {
        if( !right )
          return { this, nullptr };
        auto [ newRight, deleted ] = right->remove( target );
        right = newRight;
        deletedNode = deleted;
      }
      else if( comp > 0 )
      {
        if( !left )
          return { this, nullptr };
        auto [ newLeft, deleted ] = left->remove( target );
        left = newLeft;
        deletedNode = deleted;
      }
      else
      {
        if( !right )
        {
          Node *temp = left;
          left = nullptr;
          return { temp, this };
        }
        else
        {
          Node *successor = getNext();
          auto [ newRight, deleted ] = right->remove( successor->key );
          right = newRight;
          swapKey( *deleted );
          deletedNode = deleted;
        }
      }
      return { balanceTree(), deletedNode };
    }

    Node * getNext()
    {
      Node *current = right;
      while( current->left )
        current = current->left;
      return current;
    }

    void swapKey( Node &other )
    {
      key_T temp( other.key );
      other.key = key;
      key = temp;
    }
    
    Node *balanceTree()
    {
      updateHeight();
      int32_t diff = getDiff();

      // Left-Left
      if( diff > 1 && left->getDiff() >= 0 )
        return rightRotate();

      // Right-Right
      if( diff < -1 && right->getDiff() <= 0 )
        return leftRotate();

      // Left-Right
      if( diff > 1 && left->getDiff() < 0 )
      {
        left = left->leftRotate();
        return rightRotate();
      }

      // Right-Left
      if( diff < -1 && right->getDiff() > 0 )
      {
        right = right->rightRotate();
        return leftRotate();
      }
      return this;
    }

    bool contains( const key_T &needle ) const
    {
      int32_t comp = compareKeys( key, needle );
      if( comp == 0 )
        return true;
      if( comp < 0 )
        return right && right->contains( needle );
      return left && left->contains( needle );
    }

    /*   x          y
     *    \        /
     *     y  ->  x
     *    /        \
     *   z          z
     */
    Node *leftRotate()
    {

      Node *y = right;
      Node *z = y->left;

      y->left = this;
      right = z;

      updateHeight();
      y->updateHeight();

      return y;
    };

    /*   x       y
     *  /         \
     * y    ->     x
     *  \         /
     *   z       z
     */
    Node *rightRotate()
    {
      Node *y = left;
      Node *z = y->right;

      y->right = this;
      left = z;

      updateHeight();
      y->updateHeight();

      return y;
    };

    void updateHeight()
    {
      uint32_t lh = left ? left->height : 0,
              rh = right ? right->height : 0;
      height = 1 + max( lh, rh );
    };

    int32_t getDiff()
    {
      int32_t lh = left ? left->height : 0,
              rh = right ? right->height : 0;
      return lh - rh;
    };

    static int compareKeys( const key_T &lhs, const key_T &rhs )
    {
      return ( rhs < lhs ) - ( lhs < rhs );
    };

    Node *left = nullptr, *right = nullptr;
    uint32_t height = 1;
    key_T key;
  };
  uint32_t size = 0;
  Node *top = nullptr;
};
