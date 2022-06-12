#pragma once

#include <cstdio>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <string>
#include <iostream>
#include <cassert>
#include <fstream>


template <typename Type>
class copy_ptr : public std::unique_ptr<Type>
{
public:
  using std::unique_ptr<Type>::unique_ptr;

  explicit copy_ptr( const Type &other )
          : std::unique_ptr<Type>( other.clone() ){}


  copy_ptr( const copy_ptr<Type> &other )
          : std::unique_ptr<Type>( other->clone() ){}
};

enum class EJsonType
{
  jsonObjectType [[maybe_unused]],
  jsonArrayType [[maybe_unused]],
  jsonNumberType [[maybe_unused]],
  jsonStringType [[maybe_unused]],
  jsonBoolType [[maybe_unused]],
  jsonNullType [[maybe_unused]]
};

class CJsonObject;

class CJsonArray;

class CJsonNumber;

class CJsonString;

class CJsonBool;

class CJsonNull;

class CJsonValue
{
public:
  explicit CJsonValue( EJsonType );

  virtual ~CJsonValue() = default;

  [[nodiscard]] virtual CJsonValue *clone() const = 0;

  [[nodiscard]] virtual size_t count( const std::string & ) const;

  [[nodiscard]] virtual size_t size() const;

  virtual CJsonValue &operator[]( const std::string & );

  virtual const CJsonValue &operator[]( const std::string & ) const;

  virtual CJsonValue &operator[]( size_t );

  virtual const CJsonValue &operator[]( size_t ) const;

  [[nodiscard]] const CJsonObject &getObject() const;

  [[nodiscard]] const CJsonArray &getArray() const;

  [[nodiscard, maybe_unused]] const CJsonString &getJsonString() const;

  [[nodiscard, maybe_unused]] const CJsonNumber &getJsonNumber() const;

  [[nodiscard, maybe_unused]] const CJsonBool &getJsonBool() const;

  [[nodiscard, maybe_unused]] const CJsonNull &getJsonNull() const;

  [[nodiscard]] virtual const std::string &toString() const;

  [[nodiscard, maybe_unused]] virtual int toInt() const;

  [[nodiscard]] virtual double toDouble() const;

  virtual explicit operator bool() const;

  static CJsonValue *parseFromString( const std::string &data, size_t &position );

  static void parseWhitespace( const std::string &data, size_t &position );

  EJsonType m_type;
};


class CJsonObject : public CJsonValue
{
public:
  CJsonObject();

  [[nodiscard]] CJsonValue *clone() const override;

  [[nodiscard]] size_t count( const std::string & ) const override;

  [[nodiscard]] size_t size() const override;

  CJsonValue &operator[]( const std::string &key ) override;

  const CJsonValue &operator[]( const std::string &key ) const override;

  explicit operator bool() const override;

private:
  friend class CJsonValue;

  static CJsonValue *parseFromString( const std::string &data, size_t &position );

  std::map<std::string, copy_ptr<CJsonValue>> m_object;
};

class CJsonArray : public CJsonValue
{
public:
  CJsonArray();

  [[nodiscard]] CJsonValue *clone() const override;

  [[nodiscard]] size_t size() const override;

  CJsonValue &operator[]( size_t ) override;

  const CJsonValue &operator[]( size_t ) const override;

  explicit operator bool() const override;

private:
  friend class CJsonValue;

  static CJsonValue *parseFromString( const std::string &data, size_t &position );

  std::vector<copy_ptr<CJsonValue>> m_vector;
};

class CJsonString : public CJsonValue
{
public:
  explicit CJsonString( std::string );

  [[nodiscard]] CJsonValue *clone() const override;

  [[nodiscard]] const std::string &toString() const override;

  [[nodiscard]] explicit operator bool() const override;

private:
  friend class CJsonValue;

  friend class CJsonObject;

  static CJsonValue *parseFromString( const std::string &data, size_t &position );

  std::string m_string;
};

class CJsonNumber : public CJsonValue
{
public:
  explicit CJsonNumber( std::string );

  [[nodiscard]] CJsonValue *clone() const override;

  [[nodiscard]] int toInt() const override;

  [[nodiscard]] double toDouble() const override;

  [[nodiscard]] explicit operator bool() const override;

private:
  friend class CJsonValue;

  static CJsonValue *parseFromString( const std::string &data, size_t &position );

  std::string m_number;
};

class CJsonBool : public CJsonValue
{
public:
  explicit CJsonBool( bool );

  [[nodiscard]] CJsonValue *clone() const override;

  explicit operator bool() const override{ return m_bool; };
private:
  friend class CJsonValue;

  static CJsonValue *parseFromString( const std::string &data, size_t &position );

  bool m_bool;
};

class CJsonNull : public CJsonValue
{
public:
  CJsonNull();

  [[nodiscard]] CJsonValue *clone() const override;

  explicit operator bool() const override{ return false; };
private:
  friend class CJsonValue;

  static CJsonValue *parseFromString( const std::string &data, size_t &position );
};


class CJsonDocument
{
public:
  explicit CJsonDocument( const std::string &fileName );

  ~CJsonDocument();

  CJsonValue &get(){ return *m_top; };

  [[nodiscard]] const CJsonValue &get() const{ return *m_top; };
  EJsonType m_type;
private:
  CJsonValue *m_top;
};

