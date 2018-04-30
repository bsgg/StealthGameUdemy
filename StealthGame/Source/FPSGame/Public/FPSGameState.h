// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FPSGameState.generated.h"

/**
 * 
 */
UCLASS()
class FPSGAME_API AFPSGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	// This will run in all clients
	// NetMulticast = This function is both executed locally on the server and replicated to all clients, regardless of the Actor's NetOwner.
	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnMissionCompleted(APawn* InstigatorPawn, bool bMissionSuccess);
	
};
