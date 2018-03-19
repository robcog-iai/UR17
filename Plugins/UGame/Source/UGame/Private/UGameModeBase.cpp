
#include "UGame.h"
#include "UGameModeBase.h"
#include "HUD/GameHUD.h"

AUGameModeBase::AUGameModeBase()
{
	HUDClass = AGameHUD::StaticClass();
}