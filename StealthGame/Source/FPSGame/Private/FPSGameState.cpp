// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSGameState.h"
#include "FPSPlayerController.h"

// This Multicast event runs in each client
void AFPSGameState::MulticastOnMissionCompleted_Implementation(APawn* InstigatorPawn, bool bMissionSuccess)
{
	// Iterate on the pawns that are available in this level
	/*for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; It++)
	{
		APawn* Pawn = It->Get();

		if (Pawn && Pawn->IsLocallyControlled())
		{
			Pawn->DisableInput(nullptr); 
		}
	}*/

	// Get all players controllers availables in this level
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		AFPSPlayerController* PC = Cast<AFPSPlayerController>(It->Get());
		if (PC && PC->IsLocalController())
		{
			PC->OnMissionCompleted(InstigatorPawn, bMissionSuccess);

			// Get this pawn to disable input
			APawn* MyPawn = PC->GetPawn();
			if (MyPawn)
			{
				MyPawn->DisableInput(PC);
			}
		}

	}
}

