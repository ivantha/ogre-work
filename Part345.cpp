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

class MyAssignment : public ApplicationContext, public InputListener, public TrayListener
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
    MyAssignment();

    void setup();

    bool keyPressed(const KeyboardEvent &evt);

    void setupView();

    void setupControls();

    void setupContent();
};

MyAssignment::MyAssignment() : ApplicationContext("OgreTutorialApp"),
                               mTerrainPos(1000, 0, 5000),
                               mTerrainsImported(false)
{
}

bool MyAssignment::keyPressed(const KeyboardEvent &evt)
{
    if (evt.keysym.sym == SDLK_ESCAPE)
    {
        getRoot()->queueEndRendering();
    } else if (evt.keysym.sym == 121) // Y
    {
        Ogre::SceneNode *cameraNode = scnMgr->getSceneNode("cameraNode", true);
        Ogre::Quaternion quat = cameraNode->getOrientation();
        cameraNode->rotate(Ogre::Vector3(1, 0, 0), Ogre::Radian(0.1));
    }else if (evt.keysym.sym == 103) // G
    {
        Ogre::SceneNode *cameraNode = scnMgr->getSceneNode("cameraNode", true);
        Ogre::Quaternion quat = cameraNode->getOrientation();
        cameraNode->rotate(Ogre::Vector3(0, 1, 0), Ogre::Radian(0.1));
    }else if (evt.keysym.sym == 104) // H
    {
        Ogre::SceneNode *cameraNode = scnMgr->getSceneNode("cameraNode", true);
        Ogre::Quaternion quat = cameraNode->getOrientation();
        cameraNode->rotate(Ogre::Vector3(1, 0, 0), Ogre::Radian(-0.1));
    }else if (evt.keysym.sym == 106) // J
    {
        Ogre::SceneNode *cameraNode = scnMgr->getSceneNode("cameraNode", true);
        Ogre::Quaternion quat = cameraNode->getOrientation();
        cameraNode->rotate(Ogre::Vector3(0, 1, 0), Ogre::Radian(-0.1));
    }else
    {
        // not supposed to happen
        std::cout << evt.keysym.sym << std::endl;
    }
    return true;
}

void MyAssignment::setup(void)
{
    ApplicationContext::setup();

    setupView();

    setupContent();
}

void MyAssignment::defineTerrain(long x, long y, bool flat)
{
    // if a file is available, use it
    // if not, generate file from import

    // Usually in a real project you'll know whether the compact terrain data is
    // available or not; I'm doing it this way to save distribution size

    if (flat)
    {
        mTerrainGroup->defineTerrain(x, y, 0.0f);
        return;
    }

    //! [define]
    String filename = mTerrainGroup->generateFilename(x, y);
    if (ResourceGroupManager::getSingleton().resourceExists(mTerrainGroup->getResourceGroup(), filename))
    {
        mTerrainGroup->defineTerrain(x, y);
    } else
    {
        Image img;
        getTerrainImage(x % 2 != 0, y % 2 != 0, img);
        mTerrainGroup->defineTerrain(x, y, &img);
        mTerrainsImported = true;
    }
    //! [define]
}

void MyAssignment::getTerrainImage(bool flipX, bool flipY, Image &img)
{
    //! [heightmap]
    img.load("terrain.png", mTerrainGroup->getResourceGroup());
    if (flipX)
        img.flipAroundY();
    if (flipY)
        img.flipAroundX();
    //! [heightmap]
}

void MyAssignment::initBlendMaps(Terrain *terrain)
{
    //! [blendmap]
    TerrainLayerBlendMap *blendMap0 = terrain->getLayerBlendMap(1);
    TerrainLayerBlendMap *blendMap1 = terrain->getLayerBlendMap(2);
    float minHeight0 = 20;
    float fadeDist0 = 15;
    float minHeight1 = 70;
    float fadeDist1 = 15;
    float *pBlend0 = blendMap0->getBlendPointer();
    float *pBlend1 = blendMap1->getBlendPointer();
    for (uint16 y = 0; y < terrain->getLayerBlendMapSize(); ++y)
    {
        for (uint16 x = 0; x < terrain->getLayerBlendMapSize(); ++x)
        {
            Real tx, ty;

            blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
            float height = terrain->getHeightAtTerrainPosition(tx, ty);

            *pBlend0++ = Math::saturate((height - minHeight0) / fadeDist0);
            *pBlend1++ = Math::saturate((height - minHeight1) / fadeDist1);
        }
    }
    blendMap0->dirty();
    blendMap1->dirty();
    blendMap0->update();
    blendMap1->update();
    //! [blendmap]
    // set up a colour map
    /*
      if (!terrain->getGlobalColourMapEnabled())
      {
      terrain->setGlobalColourMapEnabled(true);
      Image colourMap;
      colourMap.load("testcolourmap.jpg", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
      terrain->getGlobalColourMap()->loadImage(colourMap);
      }
    */
}

void MyAssignment::configureTerrainDefaults(Light *l)
{
    //! [configure_lod]
    mTerrainGlobals->setMaxPixelError(8);
    mTerrainGlobals->setCompositeMapDistance(3000);
    //! [configure_lod]

    //mTerrainGlobals->setUseRayBoxDistanceCalculation(true);
    //mTerrainGlobals->getDefaultMaterialGenerator()->setDebugLevel(1);
    //mTerrainGlobals->setLightMapSize(256);

    // Disable the lightmap for OpenGL ES 2.0. The minimum number of samplers allowed is 8(as opposed to 16 on desktop).
    // Otherwise we will run over the limit by just one. The minimum was raised to 16 in GL ES 3.0.
    if (Ogre::Root::getSingletonPtr()->getRenderSystem()->getCapabilities()->getNumTextureUnits() < 9)
    {
        TerrainMaterialGeneratorA::SM2Profile *matProfile =
                static_cast<TerrainMaterialGeneratorA::SM2Profile *>(mTerrainGlobals->getDefaultMaterialGenerator()->getActiveProfile());
        matProfile->setLightmapEnabled(false);
    }

    //! [composite_lighting]
    // Important to set these so that the terrain knows what to use for baked (non-realtime) data
    mTerrainGlobals->setLightMapDirection(l->getDerivedDirection());
    mTerrainGlobals->setCompositeMapAmbient(scnMgr->getAmbientLight());
    mTerrainGlobals->setCompositeMapDiffuse(l->getDiffuseColour());
    //! [composite_lighting]
    //mTerrainGlobals->setCompositeMapAmbient(ColourValue::Red);

    // Configure default import settings for if we use imported image
    //! [import_settings]
    Ogre::Terrain::ImportData &defaultimp = mTerrainGroup->getDefaultImportSettings();
    defaultimp.terrainSize = TERRAIN_SIZE;
    defaultimp.worldSize = TERRAIN_WORLD_SIZE;
    defaultimp.inputScale = 600;
    defaultimp.minBatchSize = 33;
    defaultimp.maxBatchSize = 65;
    //! [import_settings]
    //! [textures]
    defaultimp.layerList.resize(3);
    defaultimp.layerList[0].worldSize = 100;
    defaultimp.layerList[0].textureNames.push_back("grass_green-01_diffusespecular.dds");
    defaultimp.layerList[0].textureNames.push_back("grass_green-01_normalheight.dds");
    defaultimp.layerList[1].worldSize = 100;
    defaultimp.layerList[1].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
    defaultimp.layerList[1].textureNames.push_back("dirt_grayrocky_normalheight.dds");
    defaultimp.layerList[2].worldSize = 200;
    defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_diffusespecular.dds");
    defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_normalheight.dds");
    //! [textures]
}

void MyAssignment::setupView(){
    // get a pointer to the already created root
    Root *root = getRoot();
    scnMgr = root->createSceneManager();

    // set position
    cameraNode = scnMgr->getRootSceneNode()->createChildSceneNode("cameraNode");
    cameraNode->setPosition(mTerrainPos + Vector3(1683, 100, 2116));

    // look at
    cameraNode->lookAt(Vector3(2000, 50, 1600), Node::TS_WORLD);

    //! [camera_setup]
    camera = scnMgr->createCamera("myCam");
    camera->setNearClipDistance(40); // tight near plane important for shadows
    camera->setFarClipDistance(50000);
    cameraNode->attachObject(camera);
    //! [camera_setup]

    //! [camera_inf]
    if (mRoot->getRenderSystem()->getCapabilities()->hasCapability(RSC_INFINITE_FAR_PLANE))
    {
        camera->setFarClipDistance(0);   // enable infinite far clip distance if we can
    }
    //! [camera_inf]

    // render into the main window
    getRenderWindow()->addViewport(camera);

    // register the scene with the RTSS
//    RTShader::ShaderGenerator *shadergen = RTShader::ShaderGenerator::getSingletonPtr();
//    shadergen->addSceneManager(scnMgr);

    // Make this viewport work with shader generator scheme.
    getRenderWindow()->getViewport(0)->setMaterialScheme(RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
}

void MyAssignment::setupControls(){
    // register for input events
    addInputListener(this);
}

void MyAssignment::setupContent() {
    //! [global_opts]
    mTerrainGlobals = new Ogre::TerrainGlobalOptions();
    //! [global_opts]

    // Bugfix for D3D11 Render System because of pixel format incompatibility when using
    // vertex compression
    if (Ogre::Root::getSingleton().getRenderSystem()->getName() == "Direct3D11 Rendering Subsystem")
        mTerrainGlobals->setUseVertexCompressionWhenAvailable(false);

    setupControls();

    MaterialManager::getSingleton().setDefaultTextureFiltering(TFO_ANISOTROPIC);
    MaterialManager::getSingleton().setDefaultAnisotropy(7);

    Ogre::ColourValue fadeColour(0.7, 0.7, 0.7);
    getRenderWindow()->getViewport(0)->setBackgroundColour(fadeColour);
    scnMgr->setFog(Ogre::FOG_EXP, fadeColour, 0.0015);

    LogManager::getSingleton().setLogDetail(LL_BOREME);

    //! [light]
    Vector3 lightdir(0.55, -0.3, 0.75);
    lightdir.normalise();

    Ogre::Light *light = scnMgr->createLight("MainLights");
    light->setType(Light::LT_DIRECTIONAL);
    light->setDirection(lightdir);
    light->setDiffuseColour(ColourValue::White);
    light->setSpecularColour(ColourValue(0.4, 0.4, 0.4));
    //! [light]
    scnMgr->setAmbientLight(ColourValue(0.2, 0.2, 0.2));

    //! [terrain_create]
    mTerrainGroup = new Ogre::TerrainGroup(scnMgr, Ogre::Terrain::ALIGN_X_Z, TERRAIN_SIZE, TERRAIN_WORLD_SIZE);
    mTerrainGroup->setFilenameConvention(TERRAIN_FILE_PREFIX, TERRAIN_FILE_SUFFIX);
    mTerrainGroup->setOrigin(mTerrainPos);
    //! [terrain_create]

    configureTerrainDefaults(light);

    //! [define_loop]
    for (long x = TERRAIN_PAGE_MIN_X; x <= TERRAIN_PAGE_MAX_X; ++x)
        for (long y = TERRAIN_PAGE_MIN_Y; y <= TERRAIN_PAGE_MAX_Y; ++y)
            defineTerrain(x, y);
    // sync load since we want everything in place when we start
    mTerrainGroup->loadAllTerrains(true);
    //! [define_loop]

    //! [init_blend]
    if (mTerrainsImported)
    {
        TerrainGroup::TerrainIterator ti = mTerrainGroup->getTerrainIterator();
        while(ti.hasMoreElements())
        {
            Terrain* t = ti.getNext()->instance;
            initBlendMaps(t);
        }
    }

    mTerrainGroup->freeTemporaryResources();
    //! [init_blend]

    scnMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);
}

int main(int argc, char *argv[])
{
    MyAssignment app;
    app.initApp();
    app.getRoot()->startRendering();
    app.closeApp();
    return 0;
}
