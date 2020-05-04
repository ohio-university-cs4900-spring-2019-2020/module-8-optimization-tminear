#pragma once

#include "GLView.h"
#include "NetMessage.h"
#include "NetMessengerClient.h"
#include "Airplane.h"
#include <WOFTGLString.h>
#include <MGLFTGLString.h>
#include <WOGUILabel.h>
#include "MGLFrustum.h"


namespace Aftr
{
   class Camera;

/**
   \class GLViewNewModule
   \author Scott Nykl 
   \brief A child of an abstract GLView. This class is the top-most manager of the module.

   Read \see GLView for important constructor and init information.

   \see GLView

    \{
*/

class GLViewNewModule : public GLView
{
public:
   static GLViewNewModule* New( const std::vector< std::string >& outArgs );
   virtual ~GLViewNewModule();
   virtual void updateWorld(); ///< Called once per frame
   virtual void loadMap(); ///< Called once at startup to build this module's scene
   virtual void createNewModuleWayPoints();
   virtual void onResizeWindow( GLsizei width, GLsizei height );
   virtual void onMouseDown( const SDL_MouseButtonEvent& e );
   virtual void onMouseUp( const SDL_MouseButtonEvent& e );
   virtual void onMouseMove( const SDL_MouseMotionEvent& e );
   virtual void onKeyDown( const SDL_KeyboardEvent& key );
   virtual void onKeyUp( const SDL_KeyboardEvent& key );

   virtual bool isViewable(float hfov, WO* wo);

   Airplane* airplane;

protected:
   GLViewNewModule( const std::vector< std::string >& args );
   virtual void onCreate(); 

   WO* frust;
   NetMessengerClient* client;
   WO* netObj;   
   WOFTGLString* airplaneMessage;
};

} //namespace Aftr