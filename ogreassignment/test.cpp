/*-------------------------------------------------------------------------
//To use Trays, you have to create an TrayManager. This is the class through which you will create and
//manage all your widgets, manipulate the cursor, change the backdrop image, adjust tray properties,
//pop up dialogs, show/hide the loading bar, etc. You can have multiple tray managers in one application.
-------------------------------------------------------------------------*/

//! [fullsource]

#include "Ogre.h"
#include "OgreApplicationContext.h"
#include "OgreInput.h"
#include "OgreRTShaderSystem.h"
#include "OgreTrays.h"
#include <iostream>

using namespace Ogre;
using namespace OgreBites;

class Ball
{
public:
    Ball() 
    {
                
    };

    void CreateBall(SceneManager* scnMgr);
    SceneNode* GetNode();
private:
    SceneNode* ballNode;
};

void Ball::CreateBall(SceneManager* scnMgr)
{
    Ogre::Entity* ballEntity = scnMgr->createEntity(SceneManager::PrefabType::PT_SPHERE);
    ballNode = scnMgr->getRootSceneNode()->createChildSceneNode("ballNode");
    ballNode->setPosition(0, 40, 0);
    ballNode->setScale(0.1f, 0.1f, 0.1f);
    ballNode->attachObject(ballEntity);
}

SceneNode* Ball::GetNode()
{
    return ballNode;
}

class Paddle
{
public:
    Paddle() 
    {
        x = 0.0;
        y = 0.0;
        z = 0.0;
    };
    void CreatePaddle(SceneManager* scnMgr);
    
    SceneNode* GetNode();
    void Translate();
    void SetPosition();
    int xAdd(int x);
private:
    int position;
   
    SceneNode* paddleNode;
    Ogre::Real x;
    Ogre::Real y;
    Ogre::Real z;
};

int Paddle::xAdd(int xx)
{
    x += xx;
    return 0;
}

void Paddle::CreatePaddle(SceneManager* scnMgr)
{
    Entity* paddleEntity = scnMgr->createEntity(SceneManager::PrefabType::PT_PLANE);
    paddleNode = scnMgr->getRootSceneNode()->createChildSceneNode();
    
    paddleNode->setPosition(x, y, z);
    paddleNode->setScale(0.05f, 0.01f, 0.05f);
    paddleNode->attachObject(paddleEntity);
}

SceneNode* Paddle::GetNode()
{
    return paddleNode;
}

void Paddle::Translate()
{
    paddleNode->translate(x, y, z);
}

void Paddle::SetPosition()
{
    paddleNode->setPosition(x, y, z);
}

Ball ball;
Paddle paddle;

OgreBites::Label* mScore;

class ExampleFrameListener : public Ogre::FrameListener
{
public:

    ExampleFrameListener(Ogre::SceneNode* node)
    {
        _node = node;
        Radius = 30.0;
        score = 0;
        m_Angle = 0.0;
    }
    bool frameStarted(const Ogre::FrameEvent& evt);
private:
    Real Radius;
    uint score;
    Ogre::SceneNode* _node;
    Ogre::Real m_Angle;
};

bool ExampleFrameListener::frameStarted(const Ogre::FrameEvent& evt)
{
    if (Radius >= 60.0)//The radius cannot exceed 60
        Radius = 30.0;

    AxisAlignedBox   ballbox = ball.GetNode()->_getWorldAABB();

    AxisAlignedBox  paddlebox = paddle.GetNode()->_getWorldAABB();
    char arrScore[8];
    if (paddlebox.intersects(ballbox))
    {
        score++;//In case of collision, the score is increased by 1
        Radius += 14;//After collision, the radius increases by 14 each time
        m_Angle += 60;//After collision，The angle increases by 60 each time
        snprintf(arrScore, sizeof(arrScore), "%d", score);
        mScore->setCaption(arrScore);
    }

    Ogre::Real x = Radius * Ogre::Math::Cos(m_Angle);
    Ogre::Real y = Radius * Ogre::Math::Sin(m_Angle);
    m_Angle += 0.01;

    _node->setPosition(x, y, 0);

    return true;
}

class Game : public ApplicationContext, public InputListener
{
public:
    Game();
    int Run();
    void setup();
    bool keyPressed(const KeyboardEvent& evt);
    void createFrameListener();
   
};

Game::Game() : ApplicationContext("pong-like paddle game")
{
}

int Game::Run()
{
    initApp();
    getRoot()->startRendering();
    closeApp();
    return 0;
}
void Game::setup()
{
    // do not forget to call the base first
    ApplicationContext::setup();
    addInputListener(this);

    // get a pointer to the already created root
    Root* root = getRoot();
    SceneManager* scnMgr = root->createSceneManager();

    // register our scene with the RTSS
    RTShader::ShaderGenerator* shadergen = RTShader::ShaderGenerator::getSingletonPtr();
    shadergen->addSceneManager(scnMgr);

    // -- tutorial section start --
    //! [turnlights]
    scnMgr->setAmbientLight(ColourValue(0.5, 0.5, 0.5));
    //! [turnlights]

    //! [newlight]
    Light* light = scnMgr->createLight("MainLight");
    SceneNode* lightNode = scnMgr->getRootSceneNode()->createChildSceneNode();
    lightNode->attachObject(light);
    //! [newlight]

    //! [camera]
    SceneNode* camNode = scnMgr->getRootSceneNode()->createChildSceneNode();

    // create the camera

    Camera* cam = scnMgr->createCamera("myCam");
    cam->setNearClipDistance(5); // specific to this sample
    cam->setAutoAspectRatio(true);
    camNode->attachObject(cam);
    camNode->setPosition(0, 0, 140);

    // and tell it to render into the main window
    getRenderWindow()->addViewport(cam);

    //! [camera]


    //! [cameramove]
    camNode->setPosition(0, 47, 222);
    //! [cameramove]

    OgreBites::Label* mScoreLabel;
    

    OgreBites::Label* mLivesLabel;
    OgreBites::Label* mLives;

    OgreBites::TrayManager* mTrayMgr = new OgreBites::TrayManager("InterfaceName", getRenderWindow());

    //you must add this in order to add a tray
    scnMgr->addRenderQueueListener(mOverlaySystem);
    //Once you have your tray manager, make sure you relay input events to it.
    addInputListener(mTrayMgr);

    mTrayMgr->showFrameStats(TL_TOPRIGHT);
    mScoreLabel = mTrayMgr->createLabel(TL_TOPLEFT, "Score", "Score:", 150);
    mScore = mTrayMgr->createLabel(TL_TOPLEFT, "score", "0", 150);
    
    mLivesLabel = mTrayMgr->createLabel(TL_TOPLEFT, "Lives", "Lives:", 150);
    mLives = mTrayMgr->createLabel(TL_TOPLEFT, "lives", "5", 150);
    
    ball.CreateBall(scnMgr);
    paddle.CreatePaddle(scnMgr);

    scnMgr->showBoundingBoxes(true);
    createFrameListener();

}

bool Game::keyPressed(const KeyboardEvent& evt)
{
    switch (evt.keysym.sym)
    {
    case SDLK_ESCAPE:
        getRoot()->queueEndRendering();
        break;
    case SDLK_LEFT:
        
        paddle.xAdd(-10);
        paddle.Translate();        
        break;
    case SDLK_RIGHT:
        paddle.xAdd(10);
        
        paddle.Translate();
        break;
   
    default:
        break;
    }

    paddle.SetPosition();
    return true;
}

void Game::createFrameListener()
{
    Ogre::FrameListener* FrameListener = new ExampleFrameListener(ball.GetNode());
    mRoot->addFrameListener(FrameListener);
}

int main(int argc, char** argv)
{
    try
    {
        Game game;
        game.Run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error occurred during execution: " << e.what() << '\n';
        return 1;
    }

    return 0;
}

//! [fullsource]
