#pragma once

#include "object.hpp"

/**
 * Text object.
 */
class CText : public CObject
{
public:
  /**
   * Initialises text at position.
   * @param position
   * @param text
   */
  CText( TVector<2> position, std::string text );

  /**
   * Renders text to window.
   * @param window
   */
  void render( CWindow &window ) const override;

private:
  /**
   * Text.
   */
  std::string text;
};

