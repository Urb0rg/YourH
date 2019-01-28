// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "YorHighnessTestGameMode.generated.h"

UCLASS(minimalapi)
class AYorHighnessTestGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AYorHighnessTestGameMode();
	virtual void BeginPlay() override;
	void ChangeCursor();
private:
	APlayerController* MyController = nullptr; 

};



