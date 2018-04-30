// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSGameMode.h"
#include "FPSHUD.h"
#include "FPSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "FPSGameState.h"

AFPSGameMode::AFPSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/BP_Player"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFPSHUD::StaticClass();

	// Specify to use gamestate class otherwise any cast will fail
	GameStateClass = AFPSGameState::StaticClass();
}

void AFPSGameMode::CompletedMission(APawn* InstigatorPawn, bool bMissionSuccess)
{
	if (InstigatorPawn)
	{
		if (SpectatingViewpointClass != nullptr)
		{
			// Get actors			
			TArray<AActor*> returnedActors;
			UGameplayStatics::GetAllActorsOfClass(this, SpectatingViewpointClass, returnedActors);
			 
			// Change viewtarget if any valid actor found
			if (returnedActors.Num() > 0)
			{
				AActor* newViewTarget = returnedActors[0];

				// Check all instances of player  controller
				for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
				{
					APlayerController *PC = It->Get();
					// We want to call all of them client and server so in this case we do not check if locally controller
					if (PC)
					{
						PC->SetViewTargetWithBlend(newViewTarget, 0.5f, EViewTargetBlendFunction::VTBlend_Cubic);
					}
				}				
			}		
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("SpectatingViewpointClass is null pointer, please update GameMode class with valid subclass."));
		}
	}

	// Get current gamestate 
	AFPSGameState* GS = GetGameState<AFPSGameState>();
	if (GS != nullptr)
	{
		GS->MulticastOnMissionCompleted(InstigatorPawn, bMissionSuccess);
	}


	// Call event
	OnMissionCompleted(InstigatorPawn, bMissionSuccess);	
}
