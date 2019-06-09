/*
  -----------------------------------------------------------------------------
  This source file is part of OGRE
  (Object-oriented Graphics Rendering Engine)
  For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2014 Torus Knot Software Ltd
Also see acknowledgements in Readme.html

  You may use this sample code for anything you like, it is not covered by the
  same license as the rest of the engine.
  -----------------------------------------------------------------------------
*/
#ifndef __Terrain_H__
#define __Terrain_H__

#include "SdkSample.h"
#include "OgrePageManager.h"
#include "OgreTerrain.h"
#include "OgreTerrainGroup.h"
#include "OgreTerrainQuadTreeNode.h"
#include "OgreTerrainMaterialGeneratorA.h"
#include "OgreTerrainPaging.h"

class _OgreSampleClassExport Sample_Terrain : public SdkSample
{
 public:

 Sample_Terrain()
     : mTerrainGlobals(0)
        , mTerrainGroup(0)
        , mTerrainPaging(0)
        , mPageManager(0)
        , mFly(false)
        , mFallVelocity(0)
        , mMode(MODE_NORMAL)
        , mLayerEdit(1)
        , mBrushSizeTerrainSpace(0.02)
        , mHeightUpdateCountDown(0)
        , mTerrainPos(1000,0,5000)
        , mTerrainsImported(false)
        , mKeyPressed(0)

    {
        mInfo["Title"] = "Terrain";
        mInfo["Description"] = "Demonstrates use of the terrain rendering plugin.";
        mInfo["Thumbnail"] = "thumb_terrain.png";
        mInfo["Category"] = "Environment";
        mInfo["Help"] = "Left click and drag anywhere in the scene to look around. Let go again to show "
            "cursor and access widgets. Use WASD keys to move. Use +/- keys when in edit mode to change content.";

        // Update terrain at max 20fps
        mHeightUpdateRate = 1.0 / 20.0;
    }

    void testCapabilities(const RenderSystemCapabilities* caps)
    {
        if (!caps->hasCapability(RSC_VERTEX_PROGRAM) || !caps->hasCapability(RSC_FRAGMENT_PROGRAM))
        {
            OGRE_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "Your graphics card does not support vertex or fragment shaders, "
                        "so you cannot run this sample. Sorry!", "Sample_Terrain::testCapabilities");
        }
    }

    StringVector getRequiredPlugins()
    {
        StringVector names;
        if (!GpuProgramManager::getSingleton().isSyntaxSupported("glsles") &&
            !GpuProgramManager::getSingleton().isSyntaxSupported("glsl") &&
            !GpuProgramManager::getSingleton().isSyntaxSupported("hlsl"))
            names.push_back("Cg Program Manager");
        return names;
    }

    void doTerrainModify(Terrain* terrain, const Vector3& centrepos, Real timeElapsed)
    {
        Vector3 tsPos;
        terrain->getTerrainPosition(centrepos, &tsPos);
    }
    bool frameRenderingQueued(const FrameEvent& evt)
    {
        if (mMode != MODE_NORMAL)
        {
            // fire ray
            Ray ray;
            //ray = mCamera->getCameraToViewportRay(0.5, 0.5);
            ray = mTrayMgr->getCursorRay(mCamera);

            TerrainGroup::RayResult rayResult = mTerrainGroup->rayIntersects(ray);
            if (rayResult.hit)
            {
                mEditMarker->setVisible(true);
                mEditNode->setPosition(rayResult.position);

                // figure out which terrains this affects
                TerrainGroup::TerrainList terrainList;
                Real brushSizeWorldSpace = TERRAIN_WORLD_SIZE * mBrushSizeTerrainSpace;
                Sphere sphere(rayResult.position, brushSizeWorldSpace);
                mTerrainGroup->sphereIntersects(sphere, &terrainList);

                for (TerrainGroup::TerrainList::iterator ti = terrainList.begin();
                     ti != terrainList.end(); ++ti)
                    doTerrainModify(*ti, rayResult.position, evt.timeSinceLastFrame);
            }
            else
            {
                mEditMarker->setVisible(false);
            }
        }

        if (!mFly)
        {
            // clamp to terrain
            Vector3 camPos = mCameraNode->getPosition();
            Ray ray;
            ray.setOrigin(Vector3(camPos.x, mTerrainPos.y + 10000, camPos.z));
            ray.setDirection(Vector3::NEGATIVE_UNIT_Y);

            TerrainGroup::RayResult rayResult = mTerrainGroup->rayIntersects(ray);
            Real distanceAboveTerrain = 50;
            Real fallSpeed = 300;
            Real newy = camPos.y;
            if (rayResult.hit)
            {
                if (camPos.y > rayResult.position.y + distanceAboveTerrain)
                {
                    mFallVelocity += evt.timeSinceLastFrame * 20;
                    mFallVelocity = std::min(mFallVelocity, fallSpeed);
                    newy = camPos.y - mFallVelocity * evt.timeSinceLastFrame;

                }
                newy = std::max(rayResult.position.y + distanceAboveTerrain, newy);
                mCameraNode->setPosition(camPos.x, newy, camPos.z);

            }

        }

        if (mHeightUpdateCountDown > 0)
        {
            mHeightUpdateCountDown -= evt.timeSinceLastFrame;
            if (mHeightUpdateCountDown <= 0)
            {
                mTerrainGroup->update();
                mHeightUpdateCountDown = 0;

            }
        }

        //! [loading_label]
        if (mTerrainGroup->isDerivedDataUpdateInProgress())
        {
            mTrayMgr->moveWidgetToTray(mInfoLabel, TL_TOP, 0);
            mInfoLabel->show();
            if (mTerrainsImported)
            {
                mInfoLabel->setCaption("Building terrain, please wait...");
            }
            else
            {
                mInfoLabel->setCaption("Updating textures, patience...");
            }
        }
        else
        {
            mTrayMgr->removeWidgetFromTray(mInfoLabel);
            mInfoLabel->hide();
            if (mTerrainsImported)
            {
                saveTerrains(true);
                mTerrainsImported = false;
            }
        }
        //! [loading_label]

        return SdkSample::frameRenderingQueued(evt);  // don't forget the parent updates!
    }

    void saveTerrains(bool onlyIfModified)
    {
        mTerrainGroup->saveAllTerrains(onlyIfModified);
    }

    bool keyReleased(const KeyboardEvent& evt)
    {
        mKeyPressed = 0;
        return SdkSample::keyReleased(evt);
    }

    bool keyPressed (const KeyboardEvent &e)
    {
        return true;
    }

    void itemSelected(SelectMenu* menu)
    {
        if (menu == mEditMenu)
        {
            mMode = (Mode)mEditMenu->getSelectionIndex();
        }
        else if (menu == mShadowsMenu)
        {
            mShadowMode = (ShadowMode)mShadowsMenu->getSelectionIndex();
            changeShadows();
        }
    }

    void checkBoxToggled(CheckBox* box)
    {
        if (box == mFlyBox)
        {
            mFly = mFlyBox->isChecked();
        }
    }

 protected:

    TerrainGlobalOptions* mTerrainGlobals;
    TerrainGroup* mTerrainGroup;
    bool mPaging;
    TerrainPaging* mTerrainPaging;
    PageManager* mPageManager;
    bool mFly;
    Real mFallVelocity;
    enum Mode
    {
        MODE_NORMAL = 0,
        MODE_EDIT_HEIGHT = 1,
        MODE_EDIT_BLEND = 2,
        MODE_COUNT = 3
    };
    enum ShadowMode
    {
        SHADOWS_NONE = 0,
        SHADOWS_COLOUR = 1,
        SHADOWS_DEPTH = 2,
        SHADOWS_COUNT = 3
    };
    Mode mMode;
    ShadowMode mShadowMode;
    Ogre::uint8 mLayerEdit;
    Real mBrushSizeTerrainSpace;
    SceneNode* mEditNode;
    Entity* mEditMarker;
    Real mHeightUpdateCountDown;
    Real mHeightUpdateRate;
    Vector3 mTerrainPos;
    SelectMenu* mEditMenu;
    SelectMenu* mShadowsMenu;
    CheckBox* mFlyBox;
    //! [infolabel]
    OgreBites::Label* mInfoLabel = nullptr;
    //! [infolabel]
    bool mTerrainsImported;
    ShadowCameraSetupPtr mPSSMSetup;

    typedef std::list<Entity*> EntityList;
    EntityList mHouseList;

    Keycode mKeyPressed;

    void addTextureShadowDebugOverlay(TrayLocation loc, size_t num)
    {
        for (size_t i = 0; i < num; ++i)
        {
            TexturePtr shadowTex = mSceneMgr->getShadowTexture(i);
            addTextureDebugOverlay(loc, shadowTex, i);
        }
    }

    void changeShadows()
    {
        configureShadows(mShadowMode != SHADOWS_NONE, mShadowMode == SHADOWS_DEPTH);
    }

    void configureShadows(bool enabled, bool depthShadows)
    {
        TerrainMaterialGeneratorA::SM2Profile* matProfile =
            static_cast<TerrainMaterialGeneratorA::SM2Profile*>(mTerrainGlobals->getDefaultMaterialGenerator()->getActiveProfile());
        matProfile->setReceiveDynamicShadowsEnabled(enabled);
#ifdef SHADOWS_IN_LOW_LOD_MATERIAL
        matProfile->setReceiveDynamicShadowsLowLod(true);
#else
        matProfile->setReceiveDynamicShadowsLowLod(false);
#endif

        RTShader::RenderState* schemRenderState = mShaderGenerator->getRenderState(RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

        for (auto srs : schemRenderState->getTemplateSubRenderStateList())
        {
            // This is the pssm3 sub render state -> remove it.
            if (dynamic_cast<RTShader::IntegratedPSSM3*>(srs))
            {
                schemRenderState->removeTemplateSubRenderState(srs);
                break;
            }
        }

        if (enabled)
        {
            // General scene setup
            mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
            mSceneMgr->setShadowFarDistance(3000);

            // 3 textures per directional light (PSSM)
            mSceneMgr->setShadowTextureCountPerLightType(Ogre::Light::LT_DIRECTIONAL, 3);

            if (!mPSSMSetup)
            {
                // shadow camera setup
                PSSMShadowCameraSetup* pssmSetup = new PSSMShadowCameraSetup();
                pssmSetup->setSplitPadding(mCamera->getNearClipDistance()*2);
                pssmSetup->calculateSplitPoints(3, mCamera->getNearClipDistance(), mSceneMgr->getShadowFarDistance());
                pssmSetup->setOptimalAdjustFactor(0, 2);
                pssmSetup->setOptimalAdjustFactor(1, 1);
                pssmSetup->setOptimalAdjustFactor(2, 0.5);

                mPSSMSetup.reset(pssmSetup);
            }
            mSceneMgr->setShadowCameraSetup(mPSSMSetup);

            if (depthShadows)
            {
                mSceneMgr->setShadowTextureCount(3);
                mSceneMgr->setShadowTextureConfig(0, 2048, 2048, PF_FLOAT32_R);
                mSceneMgr->setShadowTextureConfig(1, 1024, 1024, PF_FLOAT32_R);
                mSceneMgr->setShadowTextureConfig(2, 1024, 1024, PF_FLOAT32_R);
                mSceneMgr->setShadowTextureSelfShadow(true);
                mSceneMgr->setShadowCasterRenderBackFaces(true);

                auto subRenderState = mShaderGenerator->createSubRenderState<RTShader::IntegratedPSSM3>();
                subRenderState->setSplitPoints(static_cast<PSSMShadowCameraSetup*>(mPSSMSetup.get())->getSplitPoints());
                schemRenderState->addTemplateSubRenderState(subRenderState);

                mSceneMgr->setShadowTextureCasterMaterial(MaterialManager::getSingleton().getByName("PSSM/shadow_caster"));
            }
            else
            {
                mSceneMgr->setShadowTextureCount(3);
                mSceneMgr->setShadowTextureConfig(0, 2048, 2048, PF_X8B8G8R8);
                mSceneMgr->setShadowTextureConfig(1, 1024, 1024, PF_X8B8G8R8);
                mSceneMgr->setShadowTextureConfig(2, 1024, 1024, PF_X8B8G8R8);
                mSceneMgr->setShadowTextureSelfShadow(false);
                mSceneMgr->setShadowCasterRenderBackFaces(false);
                mSceneMgr->setShadowTextureCasterMaterial(MaterialPtr());
            }

            matProfile->setReceiveDynamicShadowsDepth(depthShadows);
            matProfile->setReceiveDynamicShadowsPSSM(static_cast<PSSMShadowCameraSetup*>(mPSSMSetup.get()));

            //addTextureShadowDebugOverlay(TL_RIGHT, 3);
        }
        else
        {
            mSceneMgr->setShadowTechnique(SHADOWTYPE_NONE);
        }

        mShaderGenerator->invalidateScheme(RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
    }

    void setupControls()
    {
        mTrayMgr->showCursor();

        // make room for the controls
        mTrayMgr->showLogo(TL_TOPRIGHT);
        mTrayMgr->showFrameStats(TL_TOPRIGHT);
        mTrayMgr->toggleAdvancedFrameStats();

        //! [infolabel_create]
        mInfoLabel = mTrayMgr->createLabel(TL_TOP, "TInfo", "", 350);
        //! [infolabel_create]

        mEditMenu = mTrayMgr->createLongSelectMenu(TL_BOTTOM, "EditMode", "Edit Mode", 370, 250, 3);
        mEditMenu->addItem("None");
        mEditMenu->addItem("Elevation");
        mEditMenu->addItem("Blend");
        mEditMenu->selectItem(0);  // no edit mode

        mFlyBox = mTrayMgr->createCheckBox(TL_BOTTOM, "Fly", "Fly");
        mFlyBox->setChecked(false, false);

        mShadowsMenu = mTrayMgr->createLongSelectMenu(TL_BOTTOM, "Shadows", "Shadows", 370, 250, 3);
        mShadowsMenu->addItem("None");
        mShadowsMenu->addItem("Colour Shadows");
        mShadowsMenu->addItem("Depth Shadows");
        mShadowsMenu->selectItem(0);  // no edit mode

        // a friendly reminder
        StringVector names;
        names.push_back("Help");
        mTrayMgr->createParamsPanel(TL_TOPLEFT, "Help", 100, names)->setParamValue(0, "H/F1");
    }

    void setupContent()
    {
//        //! [global_opts]
//        mTerrainGlobals = new Ogre::TerrainGlobalOptions();
//        //! [global_opts]

//        // Bugfix for D3D11 Render System because of pixel format incompatibility when using
//        // vertex compression
//        if (Ogre::Root::getSingleton().getRenderSystem()->getName() == "Direct3D11 Rendering Subsystem")
//            mTerrainGlobals->setUseVertexCompressionWhenAvailable(false);

        mEditMarker = mSceneMgr->createEntity("editMarker", "sphere.mesh");
        mEditNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
        mEditNode->attachObject(mEditMarker);
        mEditNode->setScale(0.05, 0.05, 0.05);

//        setupControls();

        mCameraMan->setTopSpeed(50);

        setDragLook(true);

//        MaterialManager::getSingleton().setDefaultTextureFiltering(TFO_ANISOTROPIC);
//        MaterialManager::getSingleton().setDefaultAnisotropy(7);
//
//        mSceneMgr->setFog(FOG_LINEAR, ColourValue(0.7, 0.7, 0.8), 0, 10000, 25000);
//
//        LogManager::getSingleton().setLogDetail(LL_BOREME);

//        //! [light]
//        Vector3 lightdir(0.55, -0.3, 0.75);
//        lightdir.normalise();
//
//        Ogre::Light* l = mSceneMgr->createLight("tstLight");
//        l->setType(Light::LT_DIRECTIONAL);
//        l->setDirection(lightdir);
//        l->setDiffuseColour(ColourValue::White);
//        l->setSpecularColour(ColourValue(0.4, 0.4, 0.4));
//        //! [light]
//        mSceneMgr->setAmbientLight(ColourValue(0.2, 0.2, 0.2));

//        //! [terrain_create]
//        mTerrainGroup = new Ogre::TerrainGroup(mSceneMgr, Ogre::Terrain::ALIGN_X_Z, TERRAIN_SIZE, TERRAIN_WORLD_SIZE);
//        mTerrainGroup->setFilenameConvention(TERRAIN_FILE_PREFIX, TERRAIN_FILE_SUFFIX);
//        mTerrainGroup->setOrigin(mTerrainPos);
//        //! [terrain_create]
        
//        configureTerrainDefaults(l);

//        //! [define_loop]
//        for (long x = TERRAIN_PAGE_MIN_X; x <= TERRAIN_PAGE_MAX_X; ++x)
//            for (long y = TERRAIN_PAGE_MIN_Y; y <= TERRAIN_PAGE_MAX_Y; ++y)
//                defineTerrain(x, y);
//        // sync load since we want everything in place when we start
//        mTerrainGroup->loadAllTerrains(true);
//        //! [define_loop]

//        //! [init_blend]
//        if (mTerrainsImported)
//        {
//            TerrainGroup::TerrainIterator ti = mTerrainGroup->getTerrainIterator();
//            while(ti.hasMoreElements())
//            {
//                Terrain* t = ti.getNext()->instance;
//                initBlendMaps(t);
//            }
//        }
//
//        mTerrainGroup->freeTemporaryResources();
//        //! [init_blend]

        // create a few entities on the terrain
        Entity* e = mSceneMgr->createEntity("tudorhouse.mesh");
        Vector3 entPos(mTerrainPos.x + 2043, 0, mTerrainPos.z + 1715);
        Quaternion rot;
        entPos.y = mTerrainGroup->getHeightAtWorldPosition(entPos) + 65.5 + mTerrainPos.y;
        rot.FromAngleAxis(Degree(Math::RangeRandom(-180, 180)), Vector3::UNIT_Y);
        SceneNode* sn = mSceneMgr->getRootSceneNode()->createChildSceneNode(entPos, rot);
        sn->setScale(Vector3(0.12, 0.12, 0.12));
        sn->attachObject(e);
        mHouseList.push_back(e);

        e = mSceneMgr->createEntity("tudorhouse.mesh");
        entPos = Vector3(mTerrainPos.x + 1850, 0, mTerrainPos.z + 1478);
        entPos.y = mTerrainGroup->getHeightAtWorldPosition(entPos) + 65.5 + mTerrainPos.y;
        rot.FromAngleAxis(Degree(Math::RangeRandom(-180, 180)), Vector3::UNIT_Y);
        sn = mSceneMgr->getRootSceneNode()->createChildSceneNode(entPos, rot);
        sn->setScale(Vector3(0.12, 0.12, 0.12));
        sn->attachObject(e);
        mHouseList.push_back(e);

        e = mSceneMgr->createEntity("tudorhouse.mesh");
        entPos = Vector3(mTerrainPos.x + 1970, 0, mTerrainPos.z + 2180);
        entPos.y = mTerrainGroup->getHeightAtWorldPosition(entPos) + 65.5 + mTerrainPos.y;
        rot.FromAngleAxis(Degree(Math::RangeRandom(-180, 180)), Vector3::UNIT_Y);
        sn = mSceneMgr->getRootSceneNode()->createChildSceneNode(entPos, rot);
        sn->setScale(Vector3(0.12, 0.12, 0.12));
        sn->attachObject(e);
        mHouseList.push_back(e);

        mSceneMgr->setSkyBox(true, "Examples/CloudyNoonSkyBox");
    }

};

#endif
