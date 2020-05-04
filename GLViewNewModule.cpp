#include "GLViewNewModule.h"

#include "WorldList.h" //This is where we place all of our WOs
#include "ManagerOpenGLState.h" //We can change OpenGL State attributes with this
#include "Axes.h" //We can set Axes to on/off with this
#include "PhysicsEngineODE.h"

//Different WO used by this module
#include "WO.h"
#include "WOStatic.h"
#include "WOStaticPlane.h"
#include "WOStaticTrimesh.h"
#include "WOTrimesh.h"
#include "WOHumanCyborg.h"
#include "WOHumanCal3DPaladin.h"
#include "WOWayPointSpherical.h"
#include "WOLight.h"
#include "WOSkyBox.h"
#include "WOCar1970sBeater.h"
#include "Camera.h"
#include "CameraStandard.h"
#include "CameraChaseActorSmooth.h"
#include "CameraChaseActorAbsNormal.h"
#include "CameraChaseActorRelNormal.h"
#include "Model.h"
#include "ModelDataShared.h"
#include "ModelMesh.h"
#include "ModelMeshDataShared.h"
#include "ModelMeshSkin.h"
#include "WONVStaticPlane.h"
#include "WONVPhysX.h"
#include "WONVDynSphere.h"
#include "AftrGLRendererBase.h"
#include "MGLFrustum.h"


//If we want to use way points, we need to include this.
#include "NewModuleWayPoints.h"
#include "NetMessage.h"
#include "NetMessengerClient.h"
#include "io.h"
#include "Airplane.h"
#include <WOFTGLString.h>
#include <MGLFTGLString.h>
#include <WOGUILabel.h>

using namespace Aftr;


const float NEAR1 = 1.0f;
const float FAR1 = 75.0f;
const float HFOV = 75.0f;
const float ASPECT = 16 / 9.0f;
std::string comicSans(ManagerEnvironmentConfiguration::getSMM() + "/fonts/COMIC.ttf");

GLViewNewModule* GLViewNewModule::New( const std::vector< std::string >& args )
{
   GLViewNewModule* glv = new GLViewNewModule( args );
   glv->init( Aftr::GRAVITY, Vector( 0, 0, -1.0f ), "aftr.conf", PHYSICS_ENGINE_TYPE::petODE );
   glv->client = NetMessengerClient::New("127.0.0.1");
   glv->onCreate();
   return glv;
}


GLViewNewModule::GLViewNewModule( const std::vector< std::string >& args ) : GLView( args )
{
   //Initialize any member variables that need to be used inside of LoadMap() here.
   //Note: At this point, the Managers are not yet initialized. The Engine initialization
   //occurs immediately after this method returns (see GLViewNewModule::New() for
   //reference). Then the engine invoke's GLView::loadMap() for this module.
   //After loadMap() returns, GLView::onCreate is finally invoked.

   //The order of execution of a module startup:
   //GLView::New() is invoked:
   //    calls GLView::init()
   //       calls GLView::loadMap() (as well as initializing the engine's Managers)
   //    calls GLView::onCreate()

   //GLViewNewModule::onCreate() is invoked after this module's LoadMap() is completed.
}


void GLViewNewModule::onCreate()
{
   //GLViewNewModule::onCreate() is invoked after this module's LoadMap() is completed.
   //At this point, all the managers are initialized. That is, the engine is fully initialized.

   if( this->pe != NULL )
   {
      //optionally, change gravity direction and magnitude here
      //The user could load these values from the module's aftr.conf
      this->pe->setGravityNormalizedVector( Vector( 0,0,-1.0f ) );
      this->pe->setGravityScalar( Aftr::GRAVITY );
   }
   this->setActorChaseType( STANDARDEZNAV ); //Default is STANDARDEZNAV mode
   //this->setNumPhysicsStepsPerRender( 0 ); //pause physics engine on start up; will remain paused till set to 1

   this->airplane = Airplane::New();

   this->airplaneMessage = WOFTGLString::New(comicSans, 30);//front size should not be confused with world size
   this->airplaneMessage->getModelT<MGLFTGLString>()->setFontColor(aftrColor4f(1.0f, 0.0f, 0.0f, 1.0f));
   this->airplaneMessage->getModelT<MGLFTGLString>()->setSize(30, 10);   
   this->airplaneMessage->setPosition(-25, 0, 15);
   this->airplaneMessage->getModelT<MGLFTGLString>()->setText("Not in Frustrum View");
   this->airplaneMessage->rotateAboutGlobalX(Aftr::PI / 2);
   this->airplaneMessage->rotateAboutGlobalZ(-180 * Aftr::DEGtoRAD);

   worldLst->push_back(this->airplane->getWorldObject());
   worldLst->push_back(this->airplaneMessage);
}


GLViewNewModule::~GLViewNewModule()
{
   //Implicitly calls GLView::~GLView()
    NetMessageCreateRawWO x;

    delete client;
}


void GLViewNewModule::updateWorld()
{
   GLView::updateWorld(); //Just call the parent's update world first.
                          //If you want to add additional functionality, do it after
                          //this call.
   
    WO* current = this->airplane->getWorldObject();
    bool inFrustrumView = isViewable(HFOV, current);
    if (inFrustrumView)
    {
        current->isVisible = true;
        this->airplaneMessage->getModelT<MGLFTGLString>()->setText("In Frustrum View");
    }
    else
    {
        current->isVisible = false;
        this->airplaneMessage->getModelT<MGLFTGLString>()->setText("Not in Frustrum View");
    }
   
}


void GLViewNewModule::onResizeWindow( GLsizei width, GLsizei height )
{
   GLView::onResizeWindow( width, height ); //call parent's resize method.
}


void GLViewNewModule::onMouseDown( const SDL_MouseButtonEvent& e )
{
   GLView::onMouseDown( e );
}


void GLViewNewModule::onMouseUp( const SDL_MouseButtonEvent& e )
{
   GLView::onMouseUp( e );
}


void GLViewNewModule::onMouseMove( const SDL_MouseMotionEvent& e )
{
   GLView::onMouseMove( e );
}


void GLViewNewModule::onKeyDown( const SDL_KeyboardEvent& key )
{
   GLView::onKeyDown( key );
   if( key.keysym.sym == SDLK_0 )
      this->setNumPhysicsStepsPerRender( 1 );

   if( key.keysym.sym == SDLK_a )
   {
        NetMessageCreateRawWO msg;

        this->airplane->setPosition(this->airplane->getPosition() + Vector(5, 0, 0));
        this->airplaneMessage->setPosition(this->airplaneMessage->getPosition() + Vector(5, 0, 0));
        this->airplaneMessage->getModelT<MGLFTGLString>()->setText("Airplane Pos: " + this->airplaneMessage->getPosition().toString());

        this->client->sendNetMsgSynchronousTCP(NetMessageCreateRawWO(this->airplane->getPosition()));
   }

   if (key.keysym.sym == SDLK_d)
   {
       NetMessageCreateRawWO msg;

       this->airplane->setPosition(this->airplane->getPosition() + Vector(-5, 0, 0));
       this->airplaneMessage->setPosition(this->airplaneMessage->getPosition() + Vector(-5, 0, 0));
       this->airplaneMessage->getModelT<MGLFTGLString>()->setText("Airplane Pos: " + this->airplaneMessage->getPosition().toString());

       this->client->sendNetMsgSynchronousTCP(NetMessageCreateRawWO(this->airplane->getPosition()));
   }

   if (key.keysym.sym == SDLK_s)
   {
       NetMessageCreateRawWO msg;

       this->airplane->setPosition(this->airplane->getPosition() + Vector(0, 5, 0));
       this->airplaneMessage->setPosition(this->airplaneMessage->getPosition() + Vector(0, 5, 0));
       this->airplaneMessage->getModelT<MGLFTGLString>()->setText("Airplane Pos: " + this->airplaneMessage->getPosition().toString());

       this->client->sendNetMsgSynchronousTCP(NetMessageCreateRawWO(this->airplane->getPosition()));
   }

   if (key.keysym.sym == SDLK_w)
   {
       NetMessageCreateRawWO msg;

       this->airplane->setPosition(this->airplane->getPosition() + Vector(0, -5, 0));
       this->airplaneMessage->setPosition(this->airplaneMessage->getPosition() + Vector(0, -5, 0));
       this->airplaneMessage->getModelT<MGLFTGLString>()->setText("Airplane Pos: " + this->airplaneMessage->getPosition().toString());

       this->client->sendNetMsgSynchronousTCP(NetMessageCreateRawWO(this->airplane->getPosition()));
   }

   if (key.keysym.sym == SDLK_5)
   {
       NetMessageCreateRawWO msg;

       this->airplane->setPosition(this->airplane->getPosition() + Vector(0, 0, 5));
       this->airplaneMessage->setPosition(this->airplaneMessage->getPosition() + Vector(0, 0, 5));
       this->airplaneMessage->getModelT<MGLFTGLString>()->setText("Airplane Pos: " + this->airplaneMessage->getPosition().toString());

       this->client->sendNetMsgSynchronousTCP(NetMessageCreateRawWO(this->airplane->getPosition()));
   }

   if (key.keysym.sym == SDLK_6)
   {
       NetMessageCreateRawWO msg;

       this->airplane->setPosition(this->airplane->getPosition() + Vector(0, 0, -5));
       this->airplaneMessage->setPosition(this->airplaneMessage->getPosition() + Vector(0, 0, -5));
       this->airplaneMessage->getModelT<MGLFTGLString>()->setText("Airplane Pos: " + this->airplaneMessage->getPosition().toString());

       this->client->sendNetMsgSynchronousTCP(NetMessageCreateRawWO(this->airplane->getPosition()));
   }
}


void GLViewNewModule::onKeyUp( const SDL_KeyboardEvent& key )
{
   GLView::onKeyUp( key );
}


void Aftr::GLViewNewModule::loadMap()
{

   this->worldLst = new WorldList(); //WorldList is a 'smart' vector that is used to store WO*'s
   this->actorLst = new WorldList();
   this->netLst = new WorldList();

   ManagerOpenGLState::GL_CLIPPING_PLANE = 1000.0;
   ManagerOpenGLState::GL_NEAR_PLANE = 0.1f;
   ManagerOpenGLState::enableFrustumCulling = false;
   Axes::isVisible = true;
   this->glRenderer->isUsingShadowMapping( false ); //set to TRUE to enable shadow mapping, must be using GL 3.2+

   this->cam->setPosition(Vector(50, 50, 5));
   this->cam->rotateAboutGlobalZ(-90 * Aftr::DEGtoRAD);

   std::string shinyRedPlasticCube( ManagerEnvironmentConfiguration::getSMM() + "/models/cube4x4x4redShinyPlastic_pp.wrl" );
   std::string wheeledCar( ManagerEnvironmentConfiguration::getSMM() + "/models/rcx_treads.wrl" );
   std::string grass( ManagerEnvironmentConfiguration::getSMM() + "/models/grassFloor400x400_pp.wrl" );
   std::string human( ManagerEnvironmentConfiguration::getSMM() + "/models/human_chest.wrl" );
   std::string peg(ManagerEnvironmentConfiguration::getSMM() + "/models/WOTeapot/cylinder/cylinder.3ds");
   
   
   //SkyBox Textures readily available
   std::vector< std::string > skyBoxImageNames;
   skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_Hubble_Nebula+6.jpg" );



   float ga = 0.1f; //Global Ambient Light level for this module
   ManagerLight::setGlobalAmbientLight( aftrColor4f( ga, ga, ga, 1.0f ) );
   WOLight* light = WOLight::New();
   light->isDirectionalLight( true );
   light->setPosition( Vector( 0, 0, 100 ) );
   light->getModel()->setDisplayMatrix( Mat4::rotateIdentityMat( { 0, 1, 0 }, 90.0f * Aftr::DEGtoRAD ) );
   light->setLabel( "Light" );
   worldLst->push_back( light );

   //Create the SkyBox
   WO* wo = WOSkyBox::New( skyBoxImageNames.at( 0 ), this->getCameraPtrPtr() );
   wo->setPosition( Vector( 0,0,0 ) );
   wo->setLabel( "Sky Box" );
   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   worldLst->push_back( wo );

   std::string sans(ManagerEnvironmentConfiguration::getSMM() + "/models/Sans.jpg");

   ////Create the infinite grass plane (the floor)
   wo = WO::New( grass, Vector( 1, 1, 1 ), MESH_SHADING_TYPE::mstFLAT );
   wo->setPosition( Vector( 0, 0, 0 ) );
   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   ModelMeshSkin& grassSkin = wo->getModel()->getModelDataShared()->getModelMeshes().at( 0 )->getSkins().at( 0 );
   grassSkin.getMultiTextureSet().at( 0 )->setTextureRepeats( 5.0f );
   grassSkin.setAmbient( aftrColor4f( 0.4f, 0.4f, 0.4f, 1.0f ) ); //Color of object when it is not in any light
   grassSkin.setDiffuse( aftrColor4f( 1.0f, 1.0f, 1.0f, 1.0f ) ); //Diffuse color components (ie, matte shading color of this object)
   grassSkin.setSpecular( aftrColor4f( 0.4f, 0.4f, 0.4f, 1.0f ) ); //Specular color component (ie, how "shiney" it is)
   grassSkin.setSpecularCoefficient( 10 ); // How "sharp" are the specular highlights (bigger is sharper, 1000 is very sharp, 10 is very dull)
   wo->setLabel( "Grass" );
   worldLst->push_back( wo );


   Aftr::Mat4 ort = this->cam->getDisplayMatrix();

   frust = WO::New();
   //frust->rotateAboutGlobalZ(-90 * Aftr::DEGtoRAD);
   frust->setModel(MGLFrustum::New(frust, NEAR1, FAR1, HFOV, ASPECT));
   frust->setPosition(Vector(50, 50, 5));
   frust->getModel()->setDisplayMatrix(ort);
   worldLst->push_back(frust);

   createNewModuleWayPoints();
}

bool Aftr::GLViewNewModule::isViewable(float hfov, WO* wo)
{
	AftrGeometryFrustum* frustum = new AftrGeometryFrustum(ASPECT, (hfov * Aftr::PI / 180) * Aftr::DEGtoRAD, NEAR1, FAR1, this->frust->getLookDirection(), this->frust->getNormalDirection(), this->frust->getPosition());

    Mat4 mat = wo->getDisplayMatrix();
	Vector box = wo->getModel()->getBoundingBox().getlxlylz() / 2.0;
	Vector pos = wo->getPosition();

    for (unsigned int plane = 0; plane < 6; ++plane) {
        Vector normal = frustum->getPlaneNormal(plane);
        float coeff = frustum->getPlaneCoef(plane);

        // check if each vertex of the bbox is outside this frustum plane
        bool outside = true;
        for (int x = -1; x <= 1; x += 2) {
            for (int y = -1; y <= 1; y += 2) {
                for (int z = -1; z <= 1; z += 2) {
                    Vector v = mat * (Vector(x, y, z) * box) + pos;
                    if (normal.dotProduct(v) < coeff) {
                        outside = false;
                    }
                }
            }
        }

        if (outside) {
            return false;
        }
    }

    return true;

}



void GLViewNewModule::createNewModuleWayPoints()
{
   // Create a waypoint with a radius of 3, a frequency of 5 seconds, activated by GLView's camera, and is visible.
   WayPointParametersBase params(this);
   params.frequency = 5000;
   params.useCamera = true;
   params.visible = true;
   WOWayPointSpherical* wayPt = WOWP1::New( params, 3 );
   wayPt->setPosition( Vector( 50, 50, 5 ) );
   worldLst->push_back( wayPt );
}
