#include <Ogre.h>
#include <OgreApplicationContext.h>
#include <iostream>

class MyAssignment : public OgreBites::ApplicationContext, public OgreBites::InputListener
{
private:
    Ogre::SceneManager *scnMgr;

public:
    MyAssignment();

    void setup();

    bool keyPressed(const OgreBites::KeyboardEvent &evt);
};

MyAssignment::MyAssignment() : OgreBites::ApplicationContext("OgreTutorialApp")
{
}

bool MyAssignment::keyPressed(const OgreBites::KeyboardEvent &evt)
{
    if (evt.keysym.sym == OgreBites::SDLK_ESCAPE)
    {
        getRoot()->queueEndRendering();
    } else if (evt.keysym.sym == 119) // W
    {
        Ogre::SceneNode *nodeC = scnMgr->getSceneNode("ogrC", true);
        Ogre::Vector3 scale = nodeC->getScale();
        nodeC->setScale(scale.x + 0.1, scale.y + 0.1, scale.z + 0.1);
    } else if (evt.keysym.sym == 115) // S
    {
        Ogre::SceneNode *nodeC = scnMgr->getSceneNode("ogrC", true);
        Ogre::Vector3 scale = nodeC->getScale();
        nodeC->setScale(scale.x - 0.1, scale.y - 0.1, scale.z - 0.1);
    } else if (evt.keysym.sym == 113) // Q
    {
        Ogre::SceneNode *nodeC = scnMgr->getSceneNode("ogrC", true);
        Ogre::Quaternion quat = nodeC->getOrientation();
        nodeC->rotate(Ogre::Vector3(0, 0, 1), Ogre::Radian(0.1));
    } else if (evt.keysym.sym == 101) // E
    {
        Ogre::SceneNode *nodeC = scnMgr->getSceneNode("ogrC", true);
        Ogre::Quaternion quat = nodeC->getOrientation();
        nodeC->rotate(Ogre::Vector3(0, 0, 1), Ogre::Radian(-0.1));
    } else if (evt.keysym.sym == 1073741906) // UP
    {
        Ogre::SceneNode *nodeC = scnMgr->getSceneNode("ogrC", true);
        Ogre::Vector3 position = nodeC->getPosition();
        nodeC->setPosition(Ogre::Vector3(position.x, position.y + 0.1, position.z));
    } else if (evt.keysym.sym == 1073741903) // RIGHT
    {
        Ogre::SceneNode *nodeC = scnMgr->getSceneNode("ogrC", true);
        Ogre::Vector3 position = nodeC->getPosition();
        nodeC->setPosition(Ogre::Vector3(position.x + 0.1, position.y, position.z));
    } else if (evt.keysym.sym == 1073741905) // DOWN
    {
        Ogre::SceneNode *nodeC = scnMgr->getSceneNode("ogrC", true);
        Ogre::Vector3 position = nodeC->getPosition();
        nodeC->setPosition(Ogre::Vector3(position.x, position.y - 0.1, position.z));
    } else if (evt.keysym.sym == 1073741904) // LEFT
    {
        Ogre::SceneNode *nodeC = scnMgr->getSceneNode("ogrC", true);
        Ogre::Vector3 position = nodeC->getPosition();
        nodeC->setPosition(Ogre::Vector3(position.x - 0.1, position.y, position.z));
    } else
    {
        // not supposed to happen
        std::cout << evt.keysym.sym << std::endl;
    }
    return true;
}

void MyAssignment::setup(void)
{
    OgreBites::ApplicationContext::setup();

    // register for input events
    addInputListener(this);

    // get a pointer to the already created root
    Ogre::Root *root = getRoot();
    scnMgr = root->createSceneManager();

    // register the scene with the RTSS
    Ogre::RTShader::ShaderGenerator *shadergen = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
    shadergen->addSceneManager(scnMgr);

    // set lights
    Ogre::Light *light = scnMgr->createLight("MainLight");
    Ogre::SceneNode *lightNode = scnMgr->getRootSceneNode()->createChildSceneNode();
    lightNode->setPosition(0, 0, 600);
    lightNode->attachObject(light);

    // set position
    Ogre::SceneNode *camNode = scnMgr->getRootSceneNode()->createChildSceneNode();
    camNode->setPosition(0, 0, 600);
    camNode->lookAt(Ogre::Vector3(0, 0, -1), Ogre::Node::TS_PARENT);

    // create the camera
    Ogre::Camera *cam = scnMgr->createCamera("myCam");
    cam->setNearClipDistance(5); // specific to this sample
    cam->setAutoAspectRatio(true);
    camNode->attachObject(cam);

    // render into the main window
    getRenderWindow()->addViewport(cam);

    // render
    Ogre::Entity *entA = scnMgr->createEntity("ogrehead.mesh");
    Ogre::SceneNode *nodeA = scnMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(0, -70, 0));
    nodeA->attachObject(entA);

    Ogre::Entity *entB = scnMgr->createEntity("ogrehead.mesh");
    Ogre::SceneNode *nodeB = scnMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(70, 0, 0));
    nodeB->attachObject(entB);

    Ogre::Entity *entC = scnMgr->createEntity("ogrehead.mesh");
    Ogre::SceneNode *nodeC = scnMgr->getRootSceneNode()->createChildSceneNode("ogrC", Ogre::Vector3(0, 70, 0));
    nodeC->scale(2, 1.2, 1);
    nodeC->attachObject(entC);

    Ogre::Entity *entD = scnMgr->createEntity("ogrehead.mesh");
    Ogre::SceneNode *nodeD = scnMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(-70, 0, 0));
    nodeD->rotate(Ogre::Vector3(0, 0, 1), Ogre::Radian(90 / 180.0 * atan(1) * 4)); // atan(1) * 4 is PI
    nodeD->attachObject(entD);
}

int main(int argc, char *argv[])
{
    MyAssignment app;
    app.initApp();
    app.getRoot()->startRendering();
    app.closeApp();
    return 0;
}
