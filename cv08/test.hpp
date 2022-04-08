#pragma once

#ifndef __PROGTEST__
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <algorithm>
#include <functional>
#include <memory>
#include <stdexcept>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cctype>
#include <cstdint>
#include <cassert>
using namespace std;
#endif /* __PROGTEST__ */

template <class T>
class copy_ptr: public unique_ptr<T>
{
public:
  using unique_ptr<T>::unique_ptr;

  copy_ptr<T>( const copy_ptr<T> &other ) : unique_ptr<T>( other->clone() ) {}

  copy_ptr<T> &operator=( copy_ptr<T> other )
  {
    unique_ptr<T>::swap( other );
    return *this;
  }
};


class baseFile
{
public:
  virtual int Size() const = 0;
  virtual baseFile *clone() const = 0;
  virtual ~baseFile() = default;
  virtual ostream &print( ostream &os ) const = 0;
};


class CFile : public baseFile
{
public:
  CFile( const string &hash, int fileSize );
  ~CFile() = default;

  int Size() const override;

  CFile &Change( const string &newHash, int newFileSize );
private:
  baseFile *clone() const override;
  ostream &print( ostream &os ) const override;
  string hash;
  int fileSize;
};

class CLink : public baseFile
{
public:
  CLink( const string &path );
  ~CLink() = default;

  int Size() const override;

  CLink Change( const string &newPath );
private:
  baseFile *clone() const override;
  ostream &print( ostream &os ) const override;
  string path;
};

class CDirectory : public baseFile
{
public:
  int Size() const override;

  CDirectory &Change( const string &newFileName, const baseFile &newFile );

  CDirectory &Change( const string &newFileName, const nullptr_t );

  const baseFile &Get( const string &fileName ) const;

  baseFile &Get( const string &fileName );

  friend ostream &operator<<( ostream &os, const CDirectory &dir );

private:
  baseFile *clone() const override;
  ostream &print( ostream &os ) const override;

  map<const string, copy_ptr<baseFile> > fileList;
};
