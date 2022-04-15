#pragma once

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
  explicit BaseType( size_t size );
  virtual ~BaseType() = default;
  virtual ostream &print( ostream &os, uint32_t indent, const string &name ) const = 0;

  [[nodiscard]] virtual bool equalTo( const BaseType &other ) const = 0;

  [[nodiscard]] virtual BaseType *clone() const = 0;

  [[nodiscard]] size_t getSize() const;

  virtual BaseType &addField( const string &name, const BaseType &field );

  virtual BaseType &add( const string &newEnum );
  [[nodiscard]] virtual const BaseType &field( const string &name ) const;
  [[nodiscard]] virtual BaseType &element() const;

  // friends
  friend bool operator==( const BaseType &lhs, const BaseType &rhs );
  friend bool operator!=( const BaseType &lhs, const BaseType &rhs );
  friend ostream &operator<<( ostream &os, const BaseType &type );
protected:
  size_t size;
};

class CDataTypeInt : public BaseType
{
public:
  CDataTypeInt();

  [[nodiscard]] constexpr static bool equalTo( const CDataTypeInt &other );
  [[nodiscard]] bool equalTo( const BaseType &other ) const override;
  [[nodiscard]] BaseType *clone() const override;
  ostream &print( ostream &os, uint32_t indent, const string &name ) const override;
};

class CDataTypeDouble : public BaseType
{
public:
  CDataTypeDouble();

  [[nodiscard]] constexpr static bool equalTo( const CDataTypeDouble &other );
  [[nodiscard]] bool equalTo( const BaseType &other ) const override;
  [[nodiscard]] BaseType *clone() const override;
  ostream &print( ostream &os, uint32_t indent, const string &name ) const override;
};

class CDataTypeEnum : public BaseType
{
public:
  CDataTypeEnum();

  BaseType &add( const string &newEnum ) override;

  [[nodiscard]] bool equalTo( const CDataTypeEnum &other ) const;
  [[nodiscard]] bool equalTo( const BaseType &other ) const override;
  [[nodiscard]] BaseType *clone() const override;
  ostream &print( ostream &os, uint32_t indent, const string &name ) const override;

private:
  vector<string> enums;
};

class CDataTypeStruct : public BaseType
{
public:
  CDataTypeStruct();
  CDataTypeStruct( const CDataTypeStruct &other );
  CDataTypeStruct &operator=( const CDataTypeStruct &other );

  CDataTypeStruct &addField( const string &name, const BaseType &field ) override;
  [[nodiscard]] const BaseType &field( const string &name ) const override;

  [[nodiscard]] bool equalTo( const CDataTypeStruct &other ) const;
  [[nodiscard]] bool equalTo( const BaseType &other ) const override;
  [[nodiscard]] BaseType *clone() const override;
  ostream &print( ostream &os, uint32_t indent, const string &name ) const override;

private:
  [[nodiscard]] const BaseType *findField( const string &needleName ) const;
  vector<pair<string, unique_ptr<BaseType>>> fields;
};


class CDataTypePtr : public BaseType
{
public:
  explicit CDataTypePtr( const BaseType &type );
  CDataTypePtr( const CDataTypePtr &other );
  CDataTypePtr &operator=( const CDataTypePtr &other );
  ~CDataTypePtr() override;

  [[nodiscard]] BaseType &element() const override;

  [[nodiscard]] bool equalTo( const CDataTypePtr &other ) const;
  [[nodiscard]] bool equalTo( const BaseType &other ) const override;
  [[nodiscard]] BaseType *clone() const override;
  ostream &print( ostream &os, uint32_t indent, const string &name ) const override;

private:
  BaseType *type;
};

class CDataTypeArray : public BaseType
{
public:
  CDataTypeArray( size_t array_size, const BaseType &type );
  CDataTypeArray( const CDataTypeArray &other );
  CDataTypeArray &operator=( const CDataTypeArray &other );
  ~CDataTypeArray() override;

  [[nodiscard]] BaseType &element() const override;

  [[nodiscard]] bool equalTo( const CDataTypeArray &other ) const;
  [[nodiscard]] bool equalTo( const BaseType &other ) const override;
  [[nodiscard]] BaseType *clone() const override;
  ostream &print( ostream &os, uint32_t indent, const string &name ) const override;

private:
  BaseType *type;
  size_t array_size;
};
