#include "MyGameModeBase.h"

AMyGameModeBase::AMyGameModeBase() : Super()
{
	PrimaryActorTick.bCanEverTick = false;

	ConstructorHelpers::FClassFinder<APawn> MainCharacterPhantom(TEXT("/Game/Characters/Main/BP_MainCharacter"));

	if (MainCharacterPhantom.Class != NULL)
	{
		DefaultPawnClass = MainCharacterPhantom.Class;
	}

}