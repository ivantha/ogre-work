//
// Created by ivantha on 09/06/19.
//

#ifndef OGRE_PROJECT1_PARTB_H
#define OGRE_PROJECT1_PARTB_H

#define TERRAIN_PAGE_MIN_X 0
#define TERRAIN_PAGE_MIN_Y 0
#define TERRAIN_PAGE_MAX_X 0
#define TERRAIN_PAGE_MAX_Y 0

#include <Ogre.h>
#include <OgreApplicationContext.h>
#include <iostream>
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>
#include <Terrain/OgreTerrainMaterialGeneratorA.h>
#include <OgreTrays.h>

#define TERRAIN_FILE_PREFIX String("testTerrain")
#define TERRAIN_FILE_SUFFIX String("dat")
#define TERRAIN_WORLD_SIZE 12000.0f
#define TERRAIN_SIZE 513

using namespace Ogre;
using namespace OgreBites;

class PartB : public ApplicationContext, public InputListener, public TrayListener
{
private:
    Ogre::SceneManager *scnMgr;

    Camera *camera;
    SceneNode *cameraNode;

    TerrainGlobalOptions *mTerrainGlobals;
    TerrainGroup *mTerrainGroup;
    Vector3 mTerrainPos;
    bool mTerrainsImported;

    TrayManager* mTrayMgr;

protected:
    void defineTerrain(long x, long y, bool flat = false);

    void getTerrainImage(bool flipX, bool flipY, Image &img);

    void initBlendMaps(Terrain *terrain);

    void configureTerrainDefaults(Light *l);

public:
    PartB();

    void setup();

    bool keyPressed(const KeyboardEvent &evt);

    void setupView();

    void setupControls();

    void setupContent();
};


#endif //OGRE_PROJECT1_PARTB_H
