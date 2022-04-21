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


/**
 * @class copy_ptr
 * @extends unique_ptr
 * @tparam Type: type of object pointer will point to
 * @brief Automatically creates copy of object on stack,
 *        and deletes it when it comes out of scope.
 */
template <typename Type>
class copy_ptr : public unique_ptr<Type>
{
public:
  using unique_ptr<Type>::unique_ptr;

  /**
   * @brief Create copy from object reference.
   */
  explicit copy_ptr( const Type &other )
          : unique_ptr<Type>( other.clone() )
  {};

  copy_ptr( const copy_ptr<Type> &other )
          : unique_ptr<Type>( other->clone() )
  {};
};

/**
 * @class CBaseType
 * @brief Base type with interfaces for derived types.
 */
class CBaseType
{
public:
  explicit CBaseType( size_t size );
  virtual ~CBaseType() = default;

  /**
   * @return Size of file.
   */
  [[nodiscard]] size_t getSize() const;

  /**
   * @see CDataTypeEnum::add
   * @throws invalid_argument if add is called on object that does not supports adding.
   */
  virtual CBaseType &add( const string &newEnum );

  /**
   * @see CDataTypeStruct::field
   * @throws invalid_argument if field is called on object that does not supports field access.
   */
  [[nodiscard]] virtual const CBaseType &field( const string &name ) const;

  /**
   * @see CDataTypePtr::element
   * @see CDataTypeArray::element
   * @throws invalid_argument if element is called on object that does not supports element access.
   */
  [[nodiscard]] virtual CBaseType &element() const;

  /**
   * @brief Prints variable 'declaration' to stream.
   * @param[in, out] os Output stream.
   * @param[in] indent Indent of every line in output.
   * @param[in] name Name of variable. For complex types like pointer to array of int pointers,
   *                 the name is used to print sub-type. In this example CDataTypeInt::print()
   *                 with name = *(* var)[] would be called resulting in print of int*(* var)[]
   * @return Output stream.
   */
  virtual ostream &print( ostream &os, uint32_t indent, const string &name ) const = 0;

  /**
   * @brief Tests two types for equality.
   * @return true if types are identical.
   */
  [[nodiscard]] virtual bool equalTo( const CBaseType &other ) const = 0;

  /**
   * @note Object needs to be deleted after use.
   * @return Deep copy of object.
   */
  [[nodiscard]] virtual CBaseType *clone() const = 0;

  /**
   * @brief Tests two for [in]equality.
   */
  friend bool operator==( const CBaseType &lhs, const CBaseType &rhs );
  friend bool operator!=( const CBaseType &lhs, const CBaseType &rhs );

  /**
   * @brief Prints variable 'declaration' to stream.
   * @param[in, out] os Output stream.
   * @param[in] type Type object.
   * @return Output stream.
   */
  friend ostream &operator<<( ostream &os, const CBaseType &type );
protected:
  /**
   * Size of type in memory.
   */
  size_t size;
};


class CDataTypeInt : public CBaseType
{
public:
  CDataTypeInt();

  /**
   * @see CBaseType::print
   */
  ostream &print( ostream &os, uint32_t indent, const string &name ) const override;

  /**
   * @see CBaseType::equalTo
   */
  [[nodiscard]] bool equalTo( const CBaseType &other ) const override;

  /**
   * @see CBaseType::clone
   */
  [[nodiscard]] CBaseType *clone() const override;
private:
  /**
   * @brief Comparison with another CDataTypeInt.
   * @param[in] other int type.
   * @return true( two int types are always equal ).
   */
  [[nodiscard]] constexpr static bool equalTo( const CDataTypeInt &other );
};


class CDataTypeDouble : public CBaseType
{
public:
  CDataTypeDouble();

  /**
   * @see CBaseType::print
   */
  ostream &print( ostream &os, uint32_t indent, const string &name ) const override;

  /**
   * @see CBaseType::equalTo
   */
  [[nodiscard]] bool equalTo( const CBaseType &other ) const override;

  /**
   * @see CBaseType::clone
   */
  [[nodiscard]] CBaseType *clone() const override;
private:
  /**
   * @brief Comparison with another CDataTypeDouble.
   * @param[in] other double type.
   * @return true( two double types are always equal ).
   */
  [[nodiscard]] constexpr static bool equalTo( const CDataTypeDouble &other );
};


class CDataTypeEnum : public CBaseType
{
public:
  CDataTypeEnum();

  /**
   * @brief Adds new name to enum.
   * @throws invalid_argument if name already exists in enum.
   * @param[in] newEnum Name of new enum.
   * @return BaseType reference to updated enum.
   */
  CBaseType &add( const string &newEnum ) override;

  /**
   * @see CBaseType::print
   */
  ostream &print( ostream &os, uint32_t indent, const string &name ) const override;

  /**
   * @see CBaseType::equalTo
   */
  [[nodiscard]] bool equalTo( const CBaseType &other ) const override;

  /**
   * @see CBaseType::clone
   */
  [[nodiscard]] CBaseType *clone() const override;
private:
  /**
   * @brief Comparison with another CDataTypeEnum.
   * @param[in] other enum type.
   * @return true if all enum are of same name.
   * @return false if there are different number of enums or
   *               if enums named differently or
   *               if enums are in different order.
   */
  [[nodiscard]] bool equalTo( const CDataTypeEnum &other ) const;

  /**
   * Vector of enum names.
   */
  vector<string> enums;
};


class CDataTypeStruct : public CBaseType
{
public:
  CDataTypeStruct();

  /**
   * @brief Inserts new field to struct.
   * @throws invalid_argument if field with same name already exists.
   * @param[in] name Name of field.
   * @param[in] field Type of field.
   * @return Updated struct.
   */
  CDataTypeStruct &addField( string name, const CBaseType &field );

  /**
   * @brief Searches for field by its name.
   * @throws invalid_argument if field with requested name does not exists.
   * @param[in] name Name of searched field.
   * @return field with requested name.
   */
  [[nodiscard]] const CBaseType &field( const string &name ) const override;

  /**
   * @see CBaseType::print
   */
  ostream &print( ostream &os, uint32_t indent, const string &name ) const override;

  /**
   * @see CBaseType::equalTo
   */
  [[nodiscard]] bool equalTo( const CBaseType &other ) const override;

  /**
   * @see CBaseType::clone
   */
  [[nodiscard]] CBaseType *clone() const override;
private:
  /**
   * @brief Comparison with another CDataTypeStruct.
   * @note Field names are not compared.
   * @param[in] other struct type.
   * @return true if all fields on same positions are equal.
   */
  [[nodiscard]] bool equalTo( const CDataTypeStruct &other ) const;

  /**
   * @param[in] needleName Name of searched field.
   * @return pointer to field.
   * @return nullptr if there is no field with name needleName.
   */
  [[nodiscard]] const CBaseType *findField( const string &needleName ) const;

  /**
   * Vector of { field name, field type } pairs.
   */
  vector<pair<string, copy_ptr<CBaseType>>> fields;
};


class CDataTypePtr : public CBaseType
{
public:
  explicit CDataTypePtr( const CBaseType &type );

  /**
   * @return type that pointer points to.
   */
  [[nodiscard]] CBaseType &element() const override;

  /**
   * @see CBaseType::print
   */
  ostream &print( ostream &os, uint32_t indent, const string &name ) const override;

  /**
   * @see CBaseType::equalTo
   */
  [[nodiscard]] bool equalTo( const CBaseType &other ) const override;

  /**
   * @see CBaseType::clone
   */
  [[nodiscard]] CBaseType *clone() const override;
private:
  /**
   * @brief Comparison with another CDataTypePtr.
   * @param[in] other pointer type.
   * @return true if pointers point to same type.
   */
  [[nodiscard]] bool equalTo( const CDataTypePtr &other ) const;

  /**
   * Type to which pointer points.
   */
  copy_ptr<CBaseType> type;
};


class CDataTypeArray : public CBaseType
{
public:
  CDataTypeArray( size_t arraySize, const CBaseType &type );

  /**
   * @return type of array elements.
   */
  [[nodiscard]] CBaseType &element() const override;

  /**
   * @see CBaseType::print
   */
  ostream &print( ostream &os, uint32_t indent, const string &name ) const override;

  /**
   * @see CBaseType::equalTo
   */
  [[nodiscard]] bool equalTo( const CBaseType &other ) const override;

  /**
   * @see CBaseType::clone
   */
  [[nodiscard]] CBaseType *clone() const override;
private:
  /**
   * @brief Comparison with another CDataTypeArray.
   * @param[in] other array type.
   * @return true if array have same element and are of same length.
   */
  [[nodiscard]] bool equalTo( const CDataTypeArray &other ) const;

  /**
   * Type of array elements.
   */
  copy_ptr<CBaseType> type;

  /**
   * Length of array.
   */
  size_t arraySize;
};
