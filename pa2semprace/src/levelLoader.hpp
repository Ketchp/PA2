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

enum class ECheckType
{
  collision,
  keyPress
};

enum class EActionType
{
  nextLevel,
  resetLevel
};

struct TCheck
{
  ECheckType type;
  EActionType action;
  union subType
  {
    unsigned char button;
    struct collisionType
    {
      enum collisionCondition{ TagTag, TagId, IdId } collisionCond;
      union specifier { ETag tag;
                        int id;
      } first, second;
    } collisionType;
  } spec = { '\0' };

  TCheck( ECheckType, EActionType, unsigned char );
  TCheck( ECheckType, EActionType, ETag, ETag );
  TCheck( ECheckType, EActionType, ETag, int );
  TCheck( ECheckType, EActionType, int, int );
};

class CLevelLoader
{
public:
  explicit CLevelLoader( std::string levelFileName = "assets/level_1.json" );
  void loadLevel( CWindow &,
                  CPhysicsEngine &,
                  std::vector<CObject *> &,
                  std::vector<TCheck> &,
                  CPainter &,
                  EActionType = EActionType::resetLevel );
private:
  std::string m_currentLevelFileName, m_nextLevelFileName;
  CWindow *m_levelWindow = nullptr;
  CPhysicsEngine *m_levelEngine = nullptr;
  std::vector<CObject *> *m_objects = nullptr;
  std::vector<TCheck> *m_checks = nullptr;
  CPainter *m_painter = nullptr;
  int idPool = INT32_MAX;

  void loadScene( const CJsonObject & );
    void loadTitle( const CJsonObject & );
    void loadSceneSize( const CJsonObject & );
    void loadFields( const CJsonArray & );
      void loadField( const CJsonValue & );
    void loadPenAttributes( const CJsonObject & );
    void loadNextFileName( const CJsonObject & );
  void loadItems( const CJsonArray & );
    void loadItem( const CJsonObject & );
      int loadItemId( const CJsonObject & );
      static ETag loadTags( const CJsonObject & );
      static ETag loadTag( const std::string & );
      static TVector<2> loadVector2D( const CJsonArray & );
      void loadCircle( const CJsonObject &, int, ETag );
      void loadRectangle( const CJsonObject &, int, ETag );
      void loadText( const CJsonObject &, int, ETag );
      static double loadDensity( const CJsonObject & );
      static double loadRotation( const CJsonObject & );
  void loadChecks( const CJsonArray & );
    void loadCheck( const CJsonObject & );
    static EActionType loadCheckAction( const CJsonObject & );
      void loadCheckPress( const CJsonObject & );
      void loadCheckCollision( const CJsonObject & );
        static TCheck::subType::collisionType::collisionCondition
            loadCheckCollisionType( const CJsonObject & );
        void loadCheckTagTag( const CJsonObject & );
        void loadCheckTagId( const CJsonObject & );
          static int loadCheckId( const CJsonObject & );
        void loadCheckIdId( const CJsonObject & );
};
