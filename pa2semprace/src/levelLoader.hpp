#pragma once
#include "physicsAttributes.hpp"
#include "window.hpp"
#include <vector>

enum class ECheckType
{
  collision
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
};


class CJsonObject;
class CJsonArray;
class CLevelLoader
{
public:
  explicit CLevelLoader( std::string levelFileName = "assets/level_1.json" );
  void loadLevel( CWindow &,
                  CPhysicsEngine &,
                  std::vector<CObject *> &,
                  std::vector<TCheck> & );
private:
  std::string m_levelDescriptionFile;
  CWindow *m_levelWindow = nullptr;
  CPhysicsEngine *m_levelEngine = nullptr;
  std::vector<CObject *> *m_objects = nullptr;
  std::vector<TCheck> *m_checks = nullptr;
  int idPool = INT32_MAX;

  void loadScene( const CJsonObject & );
    void loadSceneSize( const CJsonObject & );
  void loadItems( const CJsonArray & );
    void loadItem( const CJsonObject & );
      static TVector<2> loadVector2D( const CJsonArray & );
      void loadLine( const CJsonObject &, int, double );
      void loadCircle( const CJsonObject &, int, double );
      void loadRectangle( const CJsonObject &, int, double );
      double loadDensity( const CJsonObject & );
  void loadControls( const CJsonArray & );
  void loadChecks( const CJsonArray & );

};
