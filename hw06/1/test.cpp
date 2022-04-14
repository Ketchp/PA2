#ifndef __PROGTEST__
#include <cstring>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <deque>
#include <stdexcept>
#include <algorithm>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <memory>
using namespace std;
#endif /* __PROGTEST__ */

class BaseType
{
public:
  explicit BaseType( size_t size ) : size( size ) {};
  virtual size_t getSize() const { return size; };
  friend bool operator==( const BaseType &lhs, const BaseType &rhs )
    { return lhs.equalTo( rhs ); };
  friend bool operator!=( const BaseType &lhs, const BaseType &rhs )
    { return !( lhs == rhs ); };
  virtual bool equalTo( const BaseType &other ) const = 0;
  virtual void print( ostream &os, uint32_t indent ) const = 0;
  friend ostream &operator<<( ostream &os, const BaseType &type )
  {
    type.print( os, 0 );
    return os << '\n';
  };
  virtual ~BaseType() = default;
  virtual BaseType *clone() const = 0;
protected:
  size_t size;
};

class CDataTypeInt : public BaseType
{
public:
  CDataTypeInt() : BaseType( 4 ) {};
  bool equalTo( const BaseType &other ) const override
  {
    try
    {
      dynamic_cast<const CDataTypeInt &>( other );
      return true;
    }
    catch( bad_cast & )
    {
      return false;
    }
  };
  void print( ostream &os, uint32_t indent ) const override
  {
    os << string( indent, ' ' ) << "int";
  };
  BaseType *clone() const override { return new CDataTypeInt; };
};
class CDataTypeDouble : public BaseType
{
public:
  CDataTypeDouble() : BaseType( 8 ) {};
  bool equalTo( const BaseType &other ) const override
  {
    try
    {
      dynamic_cast<const CDataTypeDouble &>( other );
      return true;
    }
    catch( bad_cast & )
    {
      return false;
    }
  }
  void print( ostream &os, uint32_t indent ) const override
  {
    os << string( indent, ' ' ) << "double";
  };
  BaseType *clone() const override { return new CDataTypeDouble; };
};
class CDataTypeEnum : public BaseType
{
public:
  CDataTypeEnum() : BaseType( 4 ) {};
  bool equalTo( const BaseType &other ) const override
  {
    try
    {
      const CDataTypeEnum &rhs = dynamic_cast<const CDataTypeEnum &>( other );
      if( enums.size() != rhs.enums.size() )
        return false;
      for( auto it1 = enums.begin(), it2 = rhs.enums.begin();
              it1 != enums.end(); ++it1, ++it2 )
        if( *it1 != *it2 )
          return false;
      return true;
    }
    catch( bad_cast & )
    {
      return false;
    }
  };

  void print( ostream &os, uint32_t indent ) const override
  {
    os << string( indent, ' ' ) << "enum\n";
    os << string( indent, ' ' ) << "{\n";
    if( !enums.empty() )
    {
      for( auto it = enums.begin(); it < enums.end() - 1; ++it )
        os << string( indent + 2, ' ' ) << *it << ",\n";
      os << string( indent + 2, ' ' ) << enums.back() << '\n';
    }
    os << string( indent, ' ' ) << '}';
  };
  CDataTypeEnum &add( string newEnum )
  {
    for( const string &used : enums )
      if( used == newEnum )
        throw invalid_argument( "Duplicate enum value: " + newEnum );
    enums.emplace_back( move( newEnum ) );
    return *this;
  };
  BaseType *clone() const override { return new CDataTypeEnum( *this ); };
private:
  vector<string> enums;
};
class CDataTypeStruct : public BaseType
{
public:
  CDataTypeStruct() : BaseType( 0 ) {};
  CDataTypeStruct( const CDataTypeStruct &other )
    : BaseType( other.size )
  {
    for( const auto &[ name, field ] : other.fields )
      fields.emplace_back( name, unique_ptr<BaseType>( field->clone() ) );
  }
  CDataTypeStruct &operator=( const CDataTypeStruct &other )
  {
    if( this == &other )
      return *this;
    fields.resize( 0 );
    for( const auto &[ name, field ] : other.fields )
      fields.emplace_back( name, unique_ptr<BaseType>( field->clone() ) );
    return *this;
  };

  CDataTypeStruct &addField( const string &name, const BaseType &field )
  {
    if( findField( name ) )
      throw invalid_argument( "Duplicate field: " + name );
      // todo: throw exception
    fields.emplace_back( name, unique_ptr<BaseType>( field.clone() ) );
    size += field.getSize();
    return *this;
  };

  const BaseType &field( const string &name ) const
  {
    const BaseType *field_ptr = findField( name );
    if( field_ptr )
      return *field_ptr;
    throw invalid_argument( "Unknown field: " + name );
  };

  bool equalTo( const BaseType &other ) const override
  {
    try
    {
      const CDataTypeStruct &rhs = dynamic_cast<const CDataTypeStruct &>( other );
      if( fields.size() != rhs.fields.size() )
        return false;
      for( auto it1 = fields.begin(), it2 = rhs.fields.begin();
           it1 != fields.end(); ++it1, ++it2 )
        if( *it1->second != *it2->second )
          return false;
      return true;
    }
    catch( bad_cast & )
    {
      return false;
    }
  };

  void print( ostream &os, uint32_t indent ) const override
  {
    os << string( indent, ' ' ) << "struct\n";
    os << string( indent, ' ' ) << "{\n";
    for( const auto &[ name, field ] : fields )
    {
      os << string( indent, ' ' );
      field->print( os, indent + 2 );
      os << ' ' << name << ";\n";
    }
    os << string( indent, ' ' ) << '}';
  };
  BaseType *clone() const override { return new CDataTypeStruct( *this ); };
private:
  const BaseType *findField( const string &needleName ) const
  {
    for( const auto &[ name, ptr ] : fields )
      if( name == needleName )
        return ptr.get();
    return nullptr;
  }
  vector<pair<string, unique_ptr<BaseType>>> fields;
};
