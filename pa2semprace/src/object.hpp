#pragma once

#include "linearAlgebra.hpp"
#include "window.hpp"
#include "tags.hpp"
#include <GL/freeglut.h>
#include <iostream>


class CObject
{
public:
  explicit CObject( TVector<2> position );

  virtual ~CObject() = default;

  virtual void render( CWindow & ) const = 0;

  void addTag( ETag tag );

  ETag m_tag = ETag::NONE;
  TVector<2> m_position;
};
