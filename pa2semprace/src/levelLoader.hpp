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

class CLevelLoader
{
public:
  CLevelLoader( CWindow &,
                CPhysicsEngine &,
                std::vector<CPhysicsObject *> &,
                std::vector<CText *> &,
                CPainter &,
                std::string );
  void loadLevel( EActionType = EActionType::resetLevel );

private:
  std::string m_currentLevelFileName, m_nextLevelFileName;
  CWindow &m_window;
  CPhysicsEngine &m_engine;
  std::vector<CPhysicsObject *> &m_objects;
  std::vector<CText *> &m_texts;
  CPainter &m_painter;

  void loadScene( const CJsonObject & );
    void loadTitle( const CJsonObject & );
    void loadSceneSize( const CJsonObject & );
    void loadFields( const CJsonArray & );
      void loadField( const CJsonValue & );
    void loadPenAttributes( const CJsonObject & );
    void loadNextFileName( const CJsonObject & );
  void loadItems( const CJsonArray & );
    void loadItem( const CJsonObject & );
      static ETag loadTags( const CJsonObject & );
      static ETag loadTag( const std::string & );
      static TVector<2> loadVector2D( const CJsonArray & );
      void loadCircle( const CJsonObject &, ETag );
      void loadRectangle( const CJsonObject &, ETag );
      void loadText( const CJsonObject &, ETag );
      static double loadDensity( const CJsonObject & );
      static double loadRotation( const CJsonObject & );
};
