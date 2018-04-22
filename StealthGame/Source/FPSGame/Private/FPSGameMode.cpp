// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSGameMode.h"
#include "FPSHUD.h"
#include "FPSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

AFPSGameMode::AFPSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/BP_Player"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFPSHUD::StaticClass();
}

void AFPSGameMode::CompletedMission(APawn* InstigatorPawn, bool bMissionSuccess)
{
	if (InstigatorPawn)
	{
		InstigatorPawn->DisableInput(nullptr);		

		if (SpectatingViewpointClass != nullptr)
		{
			// Get actors			
			TArray<AActor*> returnedActors;
			UGameplayStatics::GetAllActorsOfClass(this, SpectatingViewpointClass, returnedActors);
			
			// Change viewtarget if any valid actor found
			if (returnedActors.Num() > 0)
			{
				AActor* newViewTarget = returnedActors[0];
				
				APlayerController *pc = Cast<APlayerController>(InstigatorPawn->GetController());
				if (pc != nullptr)
				{
					pc->SetViewTargetWithBlend(newViewTarget, 0.5f, EViewTargetBlendFunction::VTBlend_Cubic); 
				}
			}		
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("SpectatingViewpointClass is null pointer, please update GameMode class with valid subclass."));
		}
	}

	// Call event
	OnMissionCompleted(InstigatorPawn, bMissionSuccess);

	
}
