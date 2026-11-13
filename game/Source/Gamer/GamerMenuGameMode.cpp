#include "GamerMenuGameMode.h"

#include "GamerHUD.h"
#include "GamerPlayerController.h"

AGamerMenuGameMode::AGamerMenuGameMode()
{
	DefaultPawnClass = nullptr;
	PlayerControllerClass = AGamerPlayerController::StaticClass();
	HUDClass = AGamerHUD::StaticClass();
}
