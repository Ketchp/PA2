#ifndef __PROGTEST__
#include "test.hpp"
#endif /* __PROGTEST__ */


// VVV BaseType implementation VVV

BaseType::BaseType( size_t size )
  : size( size )
{}

size_t BaseType::getSize() const
{
  return size;
}

bool operator==( const BaseType &lhs, const BaseType &rhs )
{
  return lhs.equalTo( rhs );
}

bool operator!=( const BaseType &lhs, const BaseType &rhs )
{
  return !( lhs == rhs );
}

BaseType &BaseType::addField( const string &name, const BaseType &field )
{
  try
  {
    return dynamic_cast<CDataTypeStruct &>( *this ).addField( name, field );
  }
  catch( bad_cast & )
  {
    stringstream typeName;
    typeName << "Cannot use addField() for type: " << *this;
    throw invalid_argument( typeName.str() );
  }
}

BaseType &BaseType::add( const string &newEnum )
{
  try
  {
    return dynamic_cast<CDataTypeEnum &>( *this ).add( newEnum );
  }
  catch( bad_cast & )
  {
    stringstream typeName;
    typeName << "Cannot use add() for type: " << *this;
    throw invalid_argument( typeName.str() );
  }
}

const BaseType &BaseType::field( const string &name ) const
{
  try
  {
    return dynamic_cast<const CDataTypeStruct &>( *this ).field( name );
  }
  catch( bad_cast & )
  {
    stringstream typeName;
    typeName << "Cannot use field() for type: " << *this;
    throw invalid_argument( typeName.str() );
  }
}

BaseType &BaseType::element() const
{
  try
  {
    return dynamic_cast<const CDataTypePtr &>( *this ).element();
  }
  catch( bad_cast & )
  {
    try
    {
      return dynamic_cast<const CDataTypeArray &>( *this ).element();
    }
    catch( bad_cast & )
    {
      stringstream typeName;
      typeName << "Cannot use element() for type: " << *this;
      throw invalid_argument( typeName.str() );
    }
  }
}

ostream &operator<<( ostream &os, const BaseType &type )
{
  return type.print( os, 0, "" );
}

/*
 * VVV CDataTypeInt implementation VVV
 */

CDataTypeInt::CDataTypeInt() : BaseType( 4 ) {}

constexpr bool CDataTypeInt::equalTo( const CDataTypeInt & )
{
  return true;
}

bool CDataTypeInt::equalTo( const BaseType &other ) const
{
  try
  {
    return equalTo( dynamic_cast<const CDataTypeInt &>( other ) );
  }
  catch( bad_cast & )
  {
    return false;
  }
}

ostream &CDataTypeInt::print( ostream &os, uint32_t indent, const string &name ) const
{
  return os << string( indent, ' ' ) << "int" << name;
}

BaseType *CDataTypeInt::clone() const
{
  return new CDataTypeInt;
}

/*
 * VVV CDataTypeDouble implementation VVV
 */

CDataTypeDouble::CDataTypeDouble() : BaseType( 8 ) {}

constexpr bool CDataTypeDouble::equalTo( const CDataTypeDouble & )
{
  return true;
}

bool CDataTypeDouble::equalTo( const BaseType &other ) const
{
  try
  {
    return equalTo( dynamic_cast<const CDataTypeDouble &>( other ) );
  }
  catch( bad_cast & )
  {
    return false;
  }
}

ostream &CDataTypeDouble::print( ostream &os, uint32_t indent, const string &name ) const
{
  return os << string( indent, ' ' ) << "double" << name;
}

BaseType *CDataTypeDouble::clone() const { return new CDataTypeDouble; }

/*
 * VVV CDataTypeEnum implementation VVV
 */

CDataTypeEnum::CDataTypeEnum() : BaseType( 4 ) {}

bool CDataTypeEnum::equalTo( const CDataTypeEnum &other ) const
{
  if( enums.size() != other.enums.size() )
    return false;
  for( auto it1 = enums.begin(), it2 = other.enums.begin();
       it1 != enums.end(); ++it1, ++it2 )
    if( *it1 != *it2 )
      return false;
  return true;
}

bool CDataTypeEnum::equalTo( const BaseType &other ) const
{
  try
  {
    return equalTo( dynamic_cast<const CDataTypeEnum &>( other ) );
  }
  catch( bad_cast & )
  {
    return false;
  }
}

ostream &CDataTypeEnum::print( ostream &os, uint32_t indent, const string &name ) const
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
  return os << name;
}

BaseType &CDataTypeEnum::add( const string &newEnum )
{
  for( const string &used : enums )
    if( used == newEnum )
      throw invalid_argument( "Duplicate enum value: " + newEnum );
  enums.emplace_back( newEnum );
  return *this;
}

BaseType *CDataTypeEnum::clone() const { return new CDataTypeEnum( *this ); }

/*
 * VVV CDataTypeStruct implementation VVV
 */

CDataTypeStruct::CDataTypeStruct() : BaseType( 0 ) {}

CDataTypeStruct::CDataTypeStruct( const CDataTypeStruct &other )
  : BaseType( other.size )
{
  for( const auto &[ name, field ] : other.fields )
    fields.emplace_back( name, unique_ptr<BaseType>( field->clone() ) );
}

CDataTypeStruct &CDataTypeStruct::operator=( const CDataTypeStruct &other )
{
  if( this == &other )
    return *this;
  fields.resize( 0 );
  fields.reserve( other.fields.size() );
  for( const auto &[ name, field ] : other.fields )
    fields.emplace_back( name, unique_ptr<BaseType>( field->clone() ) );
  return *this;
}

CDataTypeStruct &CDataTypeStruct::addField( const string &name, const BaseType &field )
{
  if( findField( name ) )
    throw invalid_argument( "Duplicate field: " + name );

  fields.emplace_back( name, unique_ptr<BaseType>( field.clone() ) );
  size += field.getSize();
  return *this;
}

const BaseType &CDataTypeStruct::field( const string &name ) const
{
  const BaseType *field_ptr = findField( name );
  if( field_ptr )
    return *field_ptr;
  throw invalid_argument( "Unknown field: " + name );
}

bool CDataTypeStruct::equalTo( const CDataTypeStruct &other ) const
{
  if( fields.size() != other.fields.size() )
    return false;
  auto it1 = fields.begin();
  auto it2 = other.fields.begin();
  while( it1 != fields.end() )
  {
    if( *it1->second != *it2->second )
      return false;
    ++it1;
    ++it2;
  }
  return true;
}

bool CDataTypeStruct::equalTo( const BaseType &other ) const
{
  try
  {
    return equalTo( dynamic_cast<const CDataTypeStruct &>( other ) );
  }
  catch( bad_cast & )
  {
    return false;
  }
}

ostream &CDataTypeStruct::print( ostream &os, uint32_t indent, const string &name ) const
{
  os << string( indent, ' ' ) << "struct\n";
  os << string( indent, ' ' ) << "{\n";
  for( const auto &[ field_name, field ] : fields )
    field->print( os, indent + 2, ' ' + field_name ) << ";\n";

  os << string( indent, ' ' ) << '}';
  return os << name;
}

BaseType *CDataTypeStruct::clone() const { return new CDataTypeStruct( *this ); }

const BaseType *CDataTypeStruct::findField( const string &needleName ) const
{
  for( const auto &[ name, ptr ] : fields )
    if( name == needleName )
      return ptr.get();
  return nullptr;
}

CDataTypePtr::CDataTypePtr( const BaseType &type )
  : BaseType( 8 ), type( type.clone() )
{}

CDataTypePtr::CDataTypePtr( const CDataTypePtr &other )
  : BaseType( 8 ), type( other.type->clone() )
{}

CDataTypePtr &CDataTypePtr::operator=( const CDataTypePtr &other )
{
  if( this == &other )
    return *this;
  delete type;
  type = other.type->clone();
  return *this;
}

CDataTypePtr::~CDataTypePtr()
{
  delete type;
}

BaseType &CDataTypePtr::element() const
{
  return *type;
}

bool CDataTypePtr::equalTo( const CDataTypePtr &other ) const
{
  return *type == *other.type;
}

bool CDataTypePtr::equalTo( const BaseType &other ) const
{
  try
  {
    return equalTo( dynamic_cast<const CDataTypePtr &>( other ) );
  }
  catch( bad_cast & )
  {
    return false;
  }
}

ostream &CDataTypePtr::print( ostream &os, uint32_t indent, const string &name ) const
{
  try
  {
    return dynamic_cast< const CDataTypeArray & >( *type ).print( os, indent, "(*" + name + ")" );
  }
  catch( bad_cast & )
  {
    return type->print( os, indent, "*" + name );
  }
}

BaseType *CDataTypePtr::clone() const { return new CDataTypePtr( *this ); }


CDataTypeArray::CDataTypeArray( size_t array_size, const BaseType &type )
  : BaseType( type.getSize() * array_size ),
    type( type.clone() ),
    array_size( array_size )
{}

CDataTypeArray::CDataTypeArray( const CDataTypeArray &other )
  : BaseType( other.getSize() ),
    type( other.type->clone() ),
    array_size( other.array_size )
{}

CDataTypeArray &CDataTypeArray::operator=( const CDataTypeArray &other )
{
  if( this == &other )
    return *this;
  delete type;
  type = other.type->clone();
  size = other.getSize();
  return *this;
}

CDataTypeArray::~CDataTypeArray()
{
  delete type;
}

BaseType &CDataTypeArray::element() const
{
  return *type;
}

bool CDataTypeArray::equalTo( const CDataTypeArray &other ) const
{
  return *type == *other.type && array_size == other.array_size;
}

bool CDataTypeArray::equalTo( const BaseType &other ) const
{
  try
  {
    return equalTo( dynamic_cast< const CDataTypeArray & >( other ) );
  }
  catch( bad_cast & )
  {
    return false;
  }
}

ostream &CDataTypeArray::print( ostream &os, uint32_t indent, const string &name ) const
{
  return type->print( os, indent, name + "[" + to_string( array_size ) + "]" );
}

BaseType *CDataTypeArray::clone() const { return new CDataTypeArray( *this ); }


