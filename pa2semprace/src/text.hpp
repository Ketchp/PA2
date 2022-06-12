#pragma once

#include "object.hpp"


class CText : public CObject
{
public:
  CText( TVector<2> position, std::string text );

  void render( CWindow & ) const override;

private:
  std::string text;
};

