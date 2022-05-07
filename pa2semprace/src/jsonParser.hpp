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
          : std::unique_ptr<Type>( other.clone() )
  {};

  copy_ptr( const copy_ptr<Type> &other )
          : std::unique_ptr<Type>( other->clone() )
  {};
};


enum class jsonType
{
  jsonObjectType,
  jsonArrayType,
  jsonNumberType,
  jsonStringType,
  jsonBoolType,
  jsonNullType
};

class jsonValue
{
public:
  explicit jsonValue( jsonType );
  virtual ~jsonValue() = default;
  [[nodiscard]] virtual jsonValue *clone() const = 0;
  [[nodiscard]] virtual std::set<std::string> getKeys() const;
  [[nodiscard]] virtual size_t count( const std::string & ) const;
  [[nodiscard]] virtual size_t size() const;
  virtual jsonValue &operator[]( const std::string & );
  virtual const jsonValue &operator[]( const std::string & ) const;
  virtual jsonValue &operator[]( size_t );
  virtual const jsonValue &operator[]( size_t ) const;
  virtual explicit operator std::string() const; //todo explicit may be not needed
  virtual explicit operator int() const; //todo explicit may be not needed
  virtual explicit operator float() const; //todo explicit may be not needed
  virtual explicit operator double() const; //todo explicit may be not needed
  virtual explicit operator bool() const;
  static jsonValue *parseFromString( const std::string &data, size_t &position );
  static void parseWhitespace( const std::string &data, size_t &position );
  jsonType m_type;
};



class jsonObject : public jsonValue
{
public:
  jsonObject();
  [[nodiscard]] jsonValue *clone() const override;
  [[nodiscard]] size_t size() const override;
  [[nodiscard]] std::set<std::string> getKeys() const override;
  [[nodiscard]] size_t count( const std::string & ) const override;
  jsonValue &operator[]( const std::string &key ) override;
  const jsonValue &operator[]( const std::string &key ) const override;
  explicit operator bool() const override { return !m_object.empty(); };
private:
  friend class jsonValue;
  static jsonValue *parseFromString( const std::string &data, size_t &position );
  std::map<std::string, copy_ptr<jsonValue>> m_object;
};

class jsonArray : public jsonValue
{
public:
  jsonArray();
  [[nodiscard]] jsonValue *clone() const override;
  [[nodiscard]] size_t size() const override;
  jsonValue &operator[]( size_t ) override;
  const jsonValue &operator[]( size_t ) const override;
  explicit operator bool() const override { return !m_vector.empty(); };
private:
  friend class jsonValue;
  static jsonValue *parseFromString( const std::string &data, size_t &position );
  std::vector<copy_ptr<jsonValue>> m_vector;
};

class jsonString : public jsonValue
{
public:
  explicit jsonString( std::string );
  [[nodiscard]] jsonValue *clone() const override;
  explicit operator std::string() const override { return m_string; }; //todo explicit may be not needed
  explicit operator bool() const override { return !m_string.empty(); };
private:
  friend class jsonValue;
  friend class jsonObject;
  static jsonValue *parseFromString( const std::string &data, size_t &position );
  std::string m_string;
};

class jsonNumber : public jsonValue
{
public:
  explicit jsonNumber( std::string );
  [[nodiscard]] jsonValue *clone() const override;
  explicit operator int() const override { return std::stoi( m_number ); }; //todo explicit may be not needed
  explicit operator float() const override { return std::stof( m_number ); }; //todo explicit may be not needed
  explicit operator double() const override { return std::stod( m_number ); }; //todo explicit may be not needed
  explicit operator bool() const override { return std::stoi( m_number ); };
private:
  friend class jsonValue;
  static jsonValue *parseFromString( const std::string &data, size_t &position );
  std::string m_number;
};

class jsonBool : public jsonValue
{
public:
  explicit jsonBool( bool );
  [[nodiscard]] jsonValue *clone() const override;
  explicit operator bool() const override { return m_bool; };
private:
  friend class jsonValue;
  static jsonValue *parseFromString( const std::string &data, size_t &position );
  bool m_bool;
};

class jsonNull : public jsonValue
{
public:
  jsonNull();
  [[nodiscard]] jsonValue *clone() const override;
  explicit operator bool() const override { return false; };
private:
  friend class jsonValue;
  static jsonValue *parseFromString( const std::string &data, size_t &position );
};


class JSON
{
public:
  explicit JSON( const std::string &fileName );
  ~JSON();

  jsonValue &get() { return *m_top; };
  [[nodiscard]] const jsonValue &get() const { return *m_top; };
  jsonType m_type;
private:
  jsonValue *m_top;
};

