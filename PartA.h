//
// Created by ivantha on 09/06/19.
//

#ifndef OGRE_PROJECT1_PARTA_H
#define OGRE_PROJECT1_PARTA_H


#include <Ogre.h>
#include <OgreApplicationContext.h>
#include <iostream>

class PartA : public OgreBites::ApplicationContext, public OgreBites::InputListener
{
private:
    Ogre::SceneManager *scnMgr;

public:
    PartA();

    void setup();

    bool keyPressed(const OgreBites::KeyboardEvent &evt);
};


#endif //OGRE_PROJECT1_PARTA_H
