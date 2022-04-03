//#include "test.hpp"

#ifndef __PROGTEST__
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <iostream>
using namespace std;
#endif /* __PROGTEST__ */

template < class T >
class sharedPtr
{
public:
  sharedPtr() = default;
  sharedPtr( const sharedPtr<T> & );
  explicit sharedPtr( T * );
  ~sharedPtr();
  const T *operator->() const;
  sharedPtr<T> &operator=( const sharedPtr<T> & );
  explicit operator bool ();
private:
  T *data = nullptr;
  uint32_t *counter = new uint32_t(1);
};

template < class T >
sharedPtr<T> fromRaw( T * );

class CFile
{
public:
  CFile() = default;
  bool seek( uint32_t offset );
  uint32_t read( uint8_t *dst, uint32_t bytes );
  uint32_t write( const uint8_t *src, uint32_t bytes );
  void truncate();
  uint32_t fileSize() const;
  void addVersion();
  bool undoVersion();
  void printHistory();
private:
  struct Change
  {
    sharedPtr<Change> previous;

    uint32_t seekPosition = 0;
    uint32_t truncatePosition = 0;
    uint32_t writePosition = 0;
    uint32_t dataLength = 0;
    const uint8_t *dataPtr = nullptr;
    bool isVersion = false;


    Change() = default;
    // WRITE change constructor
    Change( const sharedPtr<Change> &prev,
            const uint8_t *data, uint32_t position, uint32_t dataLength, uint32_t fileLength );
    // SEEK/TRUNCATE/VERSION change constructor
    Change( const sharedPtr<Change> &prev, uint32_t newPosition,
            uint32_t newFileLength, bool isVersion = false );
    ~Change();
  };

  uint32_t _position = 0;
  uint32_t _fileSize = 0;

  sharedPtr< Change > current;
};


template <class T>
sharedPtr<T>::sharedPtr( const sharedPtr<T> &other )
  : data( other.data ), counter( other.counter )
{
  ++*counter;
}

template <class T>
sharedPtr<T>::sharedPtr( T *ptr )
  : data( ptr )
{}

template <class T>
sharedPtr<T>::~sharedPtr()
{
  if( --*counter )
    return;
  delete data;
  delete counter;
}

template <class T>
const T *sharedPtr<T>::operator->() const
{
  return data;
}

template <class T>
sharedPtr<T> &sharedPtr<T>::operator=( const sharedPtr<T> &other )
{
  if( this == &other || data == other.data )
    return *this;
  if( !--*counter )
  {
    delete counter;
    delete data;
  }
  data = other.data;
  counter = other.counter;
  ++*counter;
  return *this;
}

template<class T>
sharedPtr<T>::operator bool() {
  return data;
}

template<class T>
sharedPtr<T> fromRaw( T *ptr )
{
  return sharedPtr<T>( ptr );
}


bool CFile::seek( uint32_t offset )
{
  if( offset < 0 || _fileSize < offset )
    return false;
  _position = offset;
  current = fromRaw( new Change( current, _position, _fileSize ) );
  return true;
}

uint32_t CFile::read( uint8_t *dst, uint32_t bytes )
{
  bytes = min( bytes, _fileSize - _position );
  bool *filled = new bool[ bytes ];
  for( uint32_t i = 0; i < bytes; ++i )
    filled[ i ] = false;
  sharedPtr<Change> past = current;
  for( uint32_t pos = 0; pos < bytes; ++pos )
  {
    while( !filled[ pos ] )
    {
      while( past->dataLength == 0 )
        past = past->previous;
      uint32_t start, end;
      start = max(past->writePosition, _position);
      end = min(past->writePosition + past->dataLength, _position + bytes);
      if( end < start )
      {
        past = past->previous;
        continue;
      }
      for (uint32_t idx = start; idx < end; ++idx)
      {
        if( filled[ idx - _position ] )
          continue;
        dst[ idx - _position ] = past->dataPtr[ idx - past->writePosition ];
        filled[ idx - _position ] = true;
      }
      past = past->previous;
    }
  }
  delete[] filled;
  _position += bytes;
  return bytes;
}

uint32_t CFile::write( const uint8_t *src, uint32_t bytes )
{
  _fileSize = max( _fileSize, _position + bytes );
  current = fromRaw( new Change( current, src, _position,
                                 bytes, _fileSize ) );
  _position += bytes;
  return bytes;
}

void CFile::truncate()
{
  current = fromRaw( new Change( current, _position, _position ) );
  _fileSize = _position;
}

uint32_t CFile::fileSize() const
{
  return _fileSize;
}

void CFile::addVersion()
{
  current = fromRaw ( new Change( current, _position, _fileSize, true ) );
}

bool CFile::undoVersion()
{
  sharedPtr< Change > temp = current;
  while( temp && !temp->isVersion )
    temp = temp->previous;
  if( !temp )
    return false;
  _position = temp->seekPosition;
  _fileSize = temp->truncatePosition;
  current = temp->previous;
  return true;
}

void CFile::printHistory()
{
  sharedPtr< Change > temp = current;
  cout << _position << " / " << _fileSize << endl;
  while( temp )
  {
    cout << temp->dataLength << " -> "
         << temp->writePosition << ", "
         << temp->seekPosition << " / "
         << temp->truncatePosition << " - "
         << temp->isVersion << endl;
    temp = temp->previous;
  }
  cout << endl;
}

CFile::Change::Change( const sharedPtr<Change> &prev, const uint8_t *data,
                       uint32_t position, uint32_t dataLength, uint32_t fileLength )
  : previous( prev ), seekPosition( position + dataLength ),
    truncatePosition( fileLength ), writePosition( position ),
    dataLength( dataLength ), dataPtr( new uint8_t[dataLength] )
{
  memcpy( (void *)dataPtr, (const void *)data, dataLength * sizeof( *data ) );
}

CFile::Change::Change( const sharedPtr<Change> &prev, uint32_t newPosition,
                       uint32_t newFileLength, bool isVersion)
  : previous( prev ), seekPosition( newPosition ),
    truncatePosition( newFileLength ), isVersion( isVersion )
{}

CFile::Change::~Change()
{
  delete[] dataPtr;
}

