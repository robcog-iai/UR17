// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Waldemar Zeitler

#include "UGame.h"
#include "HUD/GameButtonStyle.h"
#include "Modules/ModuleManager.h"

//Custom implementation of the Default Game Module. 
class FSlateGameModule : public FDefaultGameModuleImpl
{
 // Called whenever the module is starting up. In here, we unregister any style sets 
 // (which may have been added by other modules) sharing our 
 // style set's name, then initialize our style set. 
 virtual void StartupModule() override
 {
  //Hot reload hack
  FSlateStyleRegistry::UnRegisterSlateStyle(FGameButtonStyles::GetStyleSetName());
  FGameButtonStyles::Initialize();
 }

 // Called whenever the module is shutting down. Here, we simply tell our MenuStyles to shut down.
 virtual void ShutdownModule() override
 {
  FGameButtonStyles::Shutdown();
 }

};

IMPLEMENT_PRIMARY_GAME_MODULE(FSlateGameModule, UGame, "UGame");