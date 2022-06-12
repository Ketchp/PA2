#pragma once

#include "window.hpp"
#include "physicsEngine.hpp"
#include "levelLoader.hpp"
#include "jsonParser.hpp"
#include "painter.hpp"
#include "object.hpp"
#include <cmath>
#include <vector>
#include <memory>
#include <functional>
#include <chrono>

/**
 * Singleton class representing game.
 */
class CGame
{
public:
  /**
   * Initialises game class.
   * @param argcPtr program arguments count pointer
   * @param argv program arguments array pointer
   */
  CGame( int *argcPtr, char *argv[] );
  CGame( const CGame & ) = delete;
  CGame( CGame && ) = delete;
  CGame &operator=( const CGame & ) = delete;
  CGame &operator=( CGame && ) = delete;

  ~CGame();

  /**
   * Infinite game loop.
   * @return Returns after game end.
   */
  void mainLoop();

private:
  /**
   * Key press handler.
   * @param key pressed key
   * @param x, y position of mouse on key-press
   */
  void keyPress( unsigned char key, int x, int y );

  /**
   * Redraws screen.
   */
  void redraw();

  /**
   * Draws health-bar.
   */
  void drawHealthBar();

  /**
   * Performs one game frame.
   */
  void nextFrame();

  /**
   * Mouse click press/release handler.
   * @param button pressed/released button
   * @param state new button state
   * @param x, y button position
   */
  void clickHandler( int button, int state, int x, int y );

  /**
   * Mouse move handler.
   * @param x, y new mouse position
   */
  void moveHandler( int x, int y );

  /**
   * Starts game.
   */
  void start();

  /**
   * Pauses game.
   */
  void pause();

  /**
   * Checks collision for completed checks.
   * @param collisions collisions to check
   * @return true if level has been successfully ended.
   */
  static bool checkCollisions( const std::vector<TManifold> &collisions );

  /**
   * Checks if collision wins level.
   * @param collision collision to check
   * @return true if collisions ends level.
   */
  static bool checkCollision( const TManifold &collision );

  /**
   * @return Time in millisecond.
   */
  static long getMilliseconds();

  /**
   * Starts timer for nextFrame call.
   */
  void setTimer();

  /**
   * @return true if player is killed. False if no player is present.
   */
  [[nodiscard]] bool checkPlayerHealth() const;

  /**
   * @return true if player is on visible screen or if no player is present.
   */
  [[nodiscard]] bool playerOnScreen();

  /**
   * Applies penalty to player for drawing line.
   * @param length line length
   */
  void drawPenalty( double length );

  /**
   * Pause signal.
   */
  bool m_paused = true;

  /**
   * Mouse button pressed.
   */
  bool pressed = false;

  /**
   * Length of single game frame in milliseconds.
   */
  const double frameLength = 40; //ms

  /**
   * Time of last game frame.
   */
  long lastFrame = 0;

  /**
   * Level window (_singleton_ instance)
   */
  CWindow m_window;

  /**
   * Level physics engine.
   */
  CPhysicsEngine m_engine;

  /**
   * All game physics objects.
   */
  std::vector<CPhysicsObject *> m_objects;

  /**
   * Game instruction texts.
   * Not shown after engine starts.
   */
  std::vector<CText *> m_text;

  /**
   * Painter, responsible for drawing complex objects.
   */
  CPainter m_painter;

  /**
   * Loads new/current level.
   */
  CLevelLoader m_levelLoader;
};
