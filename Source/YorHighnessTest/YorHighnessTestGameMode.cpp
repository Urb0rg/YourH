// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "YorHighnessTestGameMode.h"
#include "YorHighnessTestCharacter.h"
#include "Classes/GameFramework/Actor.h"
#include "UObject/ConstructorHelpers.h"

AYorHighnessTestGameMode::AYorHighnessTestGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AYorHighnessTestGameMode::BeginPlay()
{
	Super::BeginPlay();

	MyController = GetWorld()->GetFirstPlayerController();
	MyController->bShowMouseCursor = true;
	MyController->bEnableClickEvents = true;
	MyController->bEnableMouseOverEvents = true;

}
