#pragma once
#include <GL/freeglut.h>
#include "helpers.hpp"

class object
{
public:
  object( int id );
  unsigned long m_ID = 0;
  virtual void render() const = 0;
  virtual ~object() = default;
};

class solidObject : public object
{
public:
  explicit solidObject( int, alphaColor = { 1.f,1.f,1.f,1.f } );
  ~solidObject() override = default;
  alphaColor m_color = { 1.0f, 1.0f, 1.0f, 1.0f };
protected:
  void applyColorFill() const;
};

class rectangle : public solidObject
{
public:
  rectangle( int, vector2D, GLfloat, GLfloat, alphaColor = { 1.f,1.f,1.f,1.f } );
  rectangle( int, GLfloat, GLfloat, GLfloat, GLfloat, alphaColor = { 1.f,1.f,1.f,1.f } );
  void render() const override;
  vector2D m_position;
  GLfloat m_width, m_height;
};







