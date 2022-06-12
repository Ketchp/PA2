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

/**
 * Smart pointer class that creates copy of
 * managed object on copying with clone method of managed object.
 * @tparam Type Type of managed object
 */
template <typename Type>
class copy_ptr : public std::unique_ptr<Type>
{
public:
  using std::unique_ptr<Type>::unique_ptr;

  /**
   * Initialises object with copy od other.
   * @param other Instance of Type
   */
  explicit copy_ptr( const Type &other )
    : std::unique_ptr<Type>( other.clone() ){}

  /**
   * Copy constructor from other.
   * @param other
   */
  copy_ptr( const copy_ptr<Type> &other )
    : std::unique_ptr<Type>( other->clone() ){}
};

/**
 * Enumeration class for all JSON types
 */
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

/**
 * Base abstract class for all jsonValues. Interface class.
 */
class CJsonValue
{
public:
  /**
   * @param type Json type
   */
  explicit CJsonValue( EJsonType type );

  virtual ~CJsonValue() = default;

  /**
   * Creates clone of json value.
   * @return Cloned object pointer.
   */
  [[nodiscard]] virtual CJsonValue *clone() const = 0;

  /**
   * @param key
   * @return Count key in json object.
   */
  [[nodiscard]] virtual size_t count( const std::string &key ) const;

  /**
   * @return Size of json container type.
   */
  [[nodiscard]] virtual size_t size() const;

  /**
   * Access with string key.
   * @param key
   * @return Reference to object at key position.
   */
  virtual CJsonValue &operator[]( const std::string &key );

  /**
   * Access with string key.
   * @param key
   * @return Const reference to object at key position.
   */
  virtual const CJsonValue &operator[]( const std::string &key ) const;

  /**
   * Access with index.
   * @param index
   * @return Reference to object at position index.
   */
  virtual CJsonValue &operator[]( size_t index );

  /**
   * Access with index.
   * @param index
   * @return Const reference to object at position index.
   */
  virtual const CJsonValue &operator[]( size_t index ) const;

  /**
   * Performs upwards dynamic cast.
   * @return Const json object reference.
   */
  [[nodiscard]] const CJsonObject &getObject() const;

  /**
   * Performs upwards dynamic cast.
   * @return Const json array reference.
   */
  [[nodiscard]] const CJsonArray &getArray() const;

  /**
   * Performs upwards dynamic cast.
   * @return Const json string reference.
   */
  [[nodiscard, maybe_unused]] const CJsonString &getJsonString() const;

  /**
   * Performs upwards dynamic cast.
   * @return Const json number reference.
   */
  [[nodiscard, maybe_unused]] const CJsonNumber &getJsonNumber() const;

  /**
   * Performs upwards dynamic cast.
   * @return Const json bool reference.
   */
  [[nodiscard, maybe_unused]] const CJsonBool &getJsonBool() const;

  /**
   * Performs upwards dynamic cast.
   * @return Const json null reference.
   */
  [[nodiscard, maybe_unused]] const CJsonNull &getJsonNull() const;

  /**
   * @return String.
   */
  [[nodiscard]] virtual const std::string &toString() const;

  /**
   * @return Integer.
   */
  [[nodiscard, maybe_unused]] virtual int toInt() const;

  /**
   * @return Double.
   */
  [[nodiscard]] virtual double toDouble() const;

  /**
   * @return Bool.
   */
  virtual explicit operator bool() const;

  /**
   * Parses jsonValue from data, starting at position.
   * @param data
   * @param position initial parsing position.
   * @return Pointer to jsonValue, pointer must be deleted after use.
   */
  [[nodiscard]]static CJsonValue *parseFromString( const std::string &data, size_t &position );

  /**
   * Parses whitespaces from data.
   * @param data
   * @param position initial parsing position.
   */
  static void parseWhitespace( const std::string &data, size_t &position );

  /**
   * Type of object.
   */
  EJsonType m_type;
};


/**
 * Class representing json object.
 */
class CJsonObject : public CJsonValue
{
public:
  /**
   * Constructs empty Json object.
   */
  CJsonObject();

  /**
   * Creates clone of json value.
   * @return Cloned object pointer.
   */
  [[nodiscard]] CJsonValue *clone() const override;

  /**
   * @param key
   * @return Count key in json object.
   */
  [[nodiscard]] size_t count( const std::string &key ) const override;

  /**
   * @return Size of json object.
   */
  [[nodiscard]] size_t size() const override;

  /**
   * Access with string key.
   * @param key
   * @return Reference to object at key position.
   */
  CJsonValue &operator[]( const std::string &key ) override;

  /**
   * Access with string key.
   * @param key
   * @return Const reference to object at key position.
   */
  const CJsonValue &operator[]( const std::string &key ) const override;

  /**
   * @return true if object contains values.
   */
  explicit operator bool() const override;

private:
  friend class CJsonValue;

  /**
   * Parses json object from data.
   * @param data
   * @param position initial position
   * @return Pointer to object.
   */
  [[nodiscard]] static CJsonValue *parseFromString( const std::string &data, size_t &position );

  /**
   * Map of objects.
   */
  std::map<std::string, copy_ptr<CJsonValue>> m_object;
};


/**
 * Class representing json array.
 */
class CJsonArray : public CJsonValue
{
public:
  /**
   * Initialises empty array.
   */
  CJsonArray();

  /**
   * Creates clone of json value.
   * @return Cloned object pointer.
   */
  [[nodiscard]] CJsonValue *clone() const override;

  /**
   * @return Size of array.
   */
  [[nodiscard]] size_t size() const override;

  /**
   * @param index
   * @return Reference to object at index.
   */
  CJsonValue &operator[]( size_t index ) override;

  /**
   * @param index
   * @return Const reference to object at index.
   */
  const CJsonValue &operator[]( size_t index ) const override;

  /**
   * @return true if array contains values.
   */
  explicit operator bool() const override;

private:
  friend class CJsonValue;

  /**
   * Parses json array from data.
   * @param data
   * @param position initial position
   * @return Pointer to array.
   */
  [[nodiscard]] static CJsonValue *parseFromString( const std::string &data, size_t &position );

  /**
   * Vector of json values.
   */
  std::vector<copy_ptr<CJsonValue>> m_vector;
};


/**
 * Class representing json string.
 */
class CJsonString : public CJsonValue
{
public:
  /**
   * Initialises json string as str.
   * @param str string
   */
  explicit CJsonString( std::string str );

  /**
   * Creates clone of json value.
   * @return Cloned object pointer.
   */
  [[nodiscard]] CJsonValue *clone() const override;

  /**
   * @return String.
   */
  [[nodiscard]] const std::string &toString() const override;

  /**
   * @return true if string is not empty.
   */
  [[nodiscard]] explicit operator bool() const override;

private:
  friend class CJsonValue;

  friend class CJsonObject;

  /**
   * Parses json string from data.
   * @param data
   * @param position initial position
   * @return Pointer to string.
   */
  [[nodiscard]] static CJsonValue *parseFromString( const std::string &data, size_t &position );

  /**
   * String.
   */
  std::string m_string;
};


/**
 * Class representing json number.
 */
class CJsonNumber : public CJsonValue
{
public:
  /**
   * Creates json number with string representation num.
   * @param num
   */
  explicit CJsonNumber( std::string num );

  /**
   * Creates clone of json value.
   * @return Cloned object pointer.
   */
  [[nodiscard]] CJsonValue *clone() const override;

  /**
   * @return Integer representation of json number.
   */
  [[nodiscard]] int toInt() const override;

  /**
   * @return Double representation of json number.
   */
  [[nodiscard]] double toDouble() const override;

  /**
   * @return true if number is non zero.
   */
  [[nodiscard]] explicit operator bool() const override;

private:
  friend class CJsonValue;

  /**
   * Parses json number from data.
   * @param data
   * @param position initial position
   * @return Pointer to json number.
   */
  [[nodiscard]] static CJsonValue *parseFromString( const std::string &data, size_t &position );

  /**
   * String number as found in json file.
   */
  std::string m_number;
};


/**
 * Class representing json bool.
 */
class CJsonBool : public CJsonValue
{
public:
  /**
   * Creates json bool with value
   * @param value
   */
  explicit CJsonBool( bool value );

  /**
   * Creates clone of json value.
   * @return Cloned object pointer.
   */
  [[nodiscard]] CJsonValue *clone() const override;

  /**
   * @return value of bool.
   */
  explicit operator bool() const override{ return m_bool; };
private:
  friend class CJsonValue;

  /**
 * Parses json bool from data.
 * @param data
 * @param position initial position
 * @return Pointer to json bool.
 */
  [[nodiscard]] static CJsonValue *parseFromString( const std::string &data, size_t &position );

  /**
   * Value.
   */
  bool m_bool;
};


/**
 * Class representing json null.
 */
class CJsonNull : public CJsonValue
{
public:
  /**
   * Initialises null object.
   */
  CJsonNull();

  /**
   * Creates clone of json value.
   * @return Cloned object pointer.
   */
  [[nodiscard]] CJsonValue *clone() const override;

  explicit operator bool() const override{ return false; };
private:
  friend class CJsonValue;

  /**
   * Parses null object from data.
   * @param data
   * @param position initial position
   * @return Pointer to json null.
   */
  [[nodiscard]] static CJsonValue *parseFromString( const std::string &data, size_t &position );
};

/**
 * Class for loading json documents from files.
 */
class CJsonDocument
{
public:
  /**
   * Parses json document from file.
   * @param fileName
   */
  explicit CJsonDocument( const std::string &fileName );

  ~CJsonDocument();

  /**
   * @return Handle to level json value.
   */
  CJsonValue &get(){ return *m_top; };

  /**
   * @return Const handle to level json value.
   */
  [[nodiscard]] const CJsonValue &get() const{ return *m_top; };

  /**
   * Type of top level json type.
   */
  EJsonType m_type;
private:
  /**
   * Top level json value.
   */
  CJsonValue *m_top;
};

