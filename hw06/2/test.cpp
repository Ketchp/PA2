#ifndef __PROGTEST__
#include "test.hpp"
#endif /* __PROGTEST__ */


CBaseType::CBaseType( size_t size )
  : size( size )
{}

size_t CBaseType::getSize() const
{
  return size;
}

bool operator==( const CBaseType &lhs, const CBaseType &rhs )
{
  return lhs.equalTo( rhs );
}

bool operator!=( const CBaseType &lhs, const CBaseType &rhs )
{
  return !( lhs == rhs );
}

CBaseType &CBaseType::add( const string & )
{
  stringstream typeName( "Cannot use add() for type: "s, ios_base::out | ios_base::ate );
  typeName << *this;
  throw invalid_argument( typeName.str() );
}

const CBaseType &CBaseType::field( const string & ) const
{
  stringstream typeName( "Cannot use field() for type: "s, ios_base::out | ios_base::ate );
  typeName << *this;
  throw invalid_argument( typeName.str() );
}

CBaseType &CBaseType::element() const
{
  stringstream typeName( "Cannot use element() for type: "s, ios_base::out | ios_base::ate );
  typeName << *this;
  throw invalid_argument( typeName.str() );
}

ostream &operator<<( ostream &os, const CBaseType &type )
{
  return type.print( os, 0, ""s );
}

CDataTypeInt::CDataTypeInt()
  : CBaseType( 4 )
{}

constexpr bool CDataTypeInt::equalTo( const CDataTypeInt & )
{
  return true;
}

bool CDataTypeInt::equalTo( const CBaseType &other ) const
{
  auto intType = dynamic_cast<const CDataTypeInt *>( &other );
  if( intType )
    return equalTo( *intType );
  return false;
}

ostream &CDataTypeInt::print( ostream &os, uint32_t indent, const string &name ) const
{
  return os << string( indent, ' ' ) << "int" << name;
}

CBaseType *CDataTypeInt::clone() const
{
  return new CDataTypeInt;
}


CDataTypeDouble::CDataTypeDouble()
  : CBaseType( 8 )
{}

constexpr bool CDataTypeDouble::equalTo( const CDataTypeDouble & )
{
  return true;
}

bool CDataTypeDouble::equalTo( const CBaseType &other ) const
{
  auto doubleType = dynamic_cast<const CDataTypeDouble *>( &other );
  if( doubleType )
    return equalTo( *doubleType );
  return false;
}

ostream &CDataTypeDouble::print( ostream &os, uint32_t indent, const string &name ) const
{
  return os << string( indent, ' ' ) << "double" << name;
}

CBaseType *CDataTypeDouble::clone() const
{
  return new CDataTypeDouble;
}


CDataTypeEnum::CDataTypeEnum()
  : CBaseType( 4 )
{}

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

bool CDataTypeEnum::equalTo( const CBaseType &other ) const
{
  auto enumType = dynamic_cast<const CDataTypeEnum *>( &other );
  if( enumType )
    return equalTo( *enumType );
  return false;
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
  os << string( indent, ' ' ) << '}' << name;
  return os;
}

CBaseType &CDataTypeEnum::add( const string &newEnum )
{
  for( const string &used : enums )
    if( used == newEnum )
      throw invalid_argument( "Duplicate enum value: "s + newEnum );
  enums.emplace_back( newEnum );
  return *this;
}

CBaseType *CDataTypeEnum::clone() const
{
  return new CDataTypeEnum( *this );
}


CDataTypeStruct::CDataTypeStruct()
  : CBaseType( 0 )
{}

CDataTypeStruct &CDataTypeStruct::addField( string name, const CBaseType &field )
{
  if( findField( name ) )
    throw invalid_argument( "Duplicate field: "s + move( name ) );

  fields.emplace_back( make_pair( move( name ), copy_ptr<CBaseType>( field ) ) );
  size += field.getSize();
  return *this;
}

const CBaseType &CDataTypeStruct::field( const string &name ) const
{
  const CBaseType *fieldPtr = findField( name );
  if( fieldPtr )
    return *fieldPtr;
  throw invalid_argument( "Unknown field: "s + name );
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

bool CDataTypeStruct::equalTo( const CBaseType &other ) const
{
  auto structType = dynamic_cast<const CDataTypeStruct *>( &other );
  if( structType )
    return equalTo( *structType );
  return false;
}

ostream &CDataTypeStruct::print( ostream &os, uint32_t indent, const string &name ) const
{
  os << string( indent, ' ' ) << "struct\n";
  os << string( indent, ' ' ) << "{\n";

  for( const auto &[ field_name, field ] : fields )
    field->print( os, indent + 2, ' ' + field_name ) << ";\n";

  os << string( indent, ' ' ) << '}' << name;
  return os;
}

CBaseType *CDataTypeStruct::clone() const
{
  return new CDataTypeStruct( *this );
}

const CBaseType *CDataTypeStruct::findField( const string &needleName ) const
{
  for( const auto &[ name, ptr ] : fields )
    if( name == needleName )
      return ptr.get();
  return nullptr;
}

CDataTypePtr::CDataTypePtr( const CBaseType &type )
  : CBaseType( 8 ), type( type )
{}

CBaseType &CDataTypePtr::element() const
{
  return *type;
}

bool CDataTypePtr::equalTo( const CDataTypePtr &other ) const
{
  return *type == *other.type;
}

bool CDataTypePtr::equalTo( const CBaseType &other ) const
{
  auto ptrType = dynamic_cast<const CDataTypePtr *>( &other );
  if( ptrType )
    return equalTo( *ptrType );
  return false;
}

ostream &CDataTypePtr::print( ostream &os, uint32_t indent, const string &name ) const
{
  auto arrayType = dynamic_cast< const CDataTypeArray * >( type.get() );
  if( arrayType )
    return arrayType->print( os, indent, "(*"s + name + ")"s );
  return type->print( os, indent, "*"s + name );
}

CBaseType *CDataTypePtr::clone() const { return new CDataTypePtr( *this ); }


CDataTypeArray::CDataTypeArray( size_t arraySize, const CBaseType &type )
  : CBaseType( arraySize * type.getSize() ),
    type( type ), arraySize( arraySize )
{}

CBaseType &CDataTypeArray::element() const
{
  return *type;
}

bool CDataTypeArray::equalTo( const CDataTypeArray &other ) const
{
  return arraySize == other.arraySize && *type == *other.type;
}

bool CDataTypeArray::equalTo( const CBaseType &other ) const
{
  auto arrayType = dynamic_cast<const CDataTypeArray *>( &other );
  if( arrayType )
    return equalTo( *arrayType );
  return false;
}

ostream &CDataTypeArray::print( ostream &os, uint32_t indent, const string &name ) const
{
  return type->print( os, indent, name + "[" + to_string( arraySize ) + "]" );
}

CBaseType *CDataTypeArray::clone() const { return new CDataTypeArray( *this ); }


