#pragma once
#include "object.hpp"


class CText: public CObject
{
public:
  CText( int id, TVector<2> position, std::string text );
  void render( CWindow & ) const override;
  CObject &rotate( double ) override;

private:
  std::string text;
};

