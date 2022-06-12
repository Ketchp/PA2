#pragma once

#include "linearAlgebra.hpp"
#include "window.hpp"
#include "tags.hpp"
#include <GL/freeglut.h>
#include <iostream>

/**
 * @class Base class for all drawable items.
 */
class CObject
{
public:
  /**
   * Constructor.
   * @param position object centre position
   */
  explicit CObject( TVector<2> position );

  virtual ~CObject() = default;

  /**
   * Virtual method for rendering.
   * @param window Target window.
   */
  virtual void render( CWindow &window ) const = 0;

  /**
   * Adds tag to object.
   * @param newTag
   */
  void addTag( ETag newTag );

  /**
   * Object tags in form of flags.
   */
  ETag m_tag = ETag::NONE;

  /**
   * Object position, what position relates to is up to derived classes to implement.
   */
  TVector<2> m_position;
};
