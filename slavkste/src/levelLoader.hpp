#pragma once

#include "physicsAttributes.hpp"
#include "physicsEngine.hpp"
#include "window.hpp"
#include "jsonParser.hpp"
#include "painter.hpp"
#include "circle.hpp"
#include "rectangle.hpp"
#include "text.hpp"
#include "complexObject.hpp"
#include <vector>


enum class EActionType
{
  nextLevel,
  resetLevel
};


/**
 * Class for loading and parsing files describing levels.
 */
class CLevelLoader
{
public:
  /**
   * Creates level loader with first file levelFileName.
   * Managing: window, engine, objects, texts, painter.
   * @param window
   * @param engine
   * @param objects
   * @param texts
   * @param painter
   * @param levelFileName
   */
  CLevelLoader( CWindow &window,
                CPhysicsEngine &engine,
                std::vector<CPhysicsObject *> &objects,
                std::vector<CText *> &texts,
                CPainter &painter,
                std::string levelFileName );

  /**
   * Loads current or new level.
   * @param action decides level to load.
   */
  void loadLevel( EActionType action = EActionType::resetLevel );

  /**
   * Show if health-bar should be displayed.
   */
  bool healthBar = true;

private:
  /**
   * Name of current level file.
   */
  std::string m_currentLevelFileName;

  /**
   * Name of next level file.
   */
  std::string m_nextLevelFileName;

  /**
   * Managed window.
   */
  CWindow &m_window;

  /**
   * Managed engine.
   */
  CPhysicsEngine &m_engine;

  /**
   * Managed objects.
   */
  std::vector<CPhysicsObject *> &m_objects;

  /**
   * Managed texts.
   */
  std::vector<CText *> &m_texts;

  /**
   * Managed painter.
   */
  CPainter &m_painter;

  /**
   * Shows if level has player.
   */
  bool hasPlayer = false;

  /**
   * Loads general information about scene.
   * @param sceneDescription
   */
  void loadScene( const CJsonObject &sceneDescription );

  /**
   * Loads scene title.
   * @param sceneDescription
   */
  void loadTitle( const CJsonObject &sceneDescription );

  /**
   * Loads scene size.
   * @param sceneDescription
   */
  void loadSceneSize( const CJsonObject &sceneDescription );

  /**
   * Loads all scene force fields.
   * @param fieldsDescription
   */
  void loadFields( const CJsonArray &fieldsDescription );

  /**
   * Loads single force field.
   * @param fieldDescription
   */
  void loadField( const CJsonValue &fieldDescription );

  /**
   * Loads pen attributes.
   * @param sceneDescription
   */
  void loadPenAttributes( const CJsonObject &sceneDescription );

  /**
   * Loads next level filename.
   * @param sceneDescription
   */
  void loadNextFileName( const CJsonObject &sceneDescription );

  /**
   * Loads items.
   * @param itemsDescription
   */
  void loadItems( const CJsonArray &itemsDescription );

  /**
   * Loads single item.
   * @param itemDescription
   */
  void loadItem( const CJsonObject &itemDescription );

  /**
   * Loads item tags.
   * @param itemDescription
   * @return Item tag.
   */
  ETag loadTags( const CJsonObject &itemDescription );

  /**
   * Loads tag from string representation.
   * @param tag string representation of tag.
   * @return Tag.
   */
  ETag loadTag( const std::string &tag );

  /**
   * Loads two dimensional vector.
   * @param jsonArray
   * @return TVector<2>
   */
  static TVector<2> loadVector2D( const CJsonArray &jsonArray );

  /**
   * Loads circle
   * @param circleDescription
   * @param tags Circle tags.
   */
  void loadCircle( const CJsonObject &circleDescription, ETag tags );

  /**
   * Loads rectangle.
   * @param rectDescription
   * @param tags Rectangle tags.
   */
  void loadRectangle( const CJsonObject &rectDescription, ETag tags );

  /**
   * Loads text.
   * @param textDescription
   * @param tags Text tags.
   */
  void loadText( const CJsonObject &textDescription, ETag tags );

  /**
   * Loads item density.
   * @param itemDescription
   * @return Item density.
   */
  static double loadDensity( const CJsonObject &itemDescription );

  /**
   * Loads item rotation
   * @param itemDescription
   * @return Rotation.
   */
  static double loadRotation( const CJsonObject &itemDescription );
};
