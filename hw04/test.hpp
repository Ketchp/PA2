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
