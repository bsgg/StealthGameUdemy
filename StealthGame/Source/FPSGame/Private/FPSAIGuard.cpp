// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSAIGuard.h"
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "FPSGameMode.h"
#include "AI/Navigation/NavigationSystem.h"



// Sets default values
AFPSAIGuard::AFPSAIGuard()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// No need to attach this to anything because is not a scene component
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	PawnSensingComp->OnSeePawn.AddDynamic(this, &AFPSAIGuard::OnPawnSeen);
	PawnSensingComp->OnHearNoise.AddDynamic(this, &AFPSAIGuard::OnNoiseHeard);

	GuardState = EAIState::Idle;
}

// Called when the game starts or when spawned
void AFPSAIGuard::BeginPlay()
{
	Super::BeginPlay();

	OriginalRotation = GetActorRotation();

	if (bPatrol)
	{
		MoveToNextPatrolPoint();
	}
	
}

void AFPSAIGuard::SetGuardState(EAIState NewState)
{
	if (GuardState == NewState) return;
	GuardState = NewState;

	OnStateChanged(GuardState);

}

// Called every frame
void AFPSAIGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentPatrolPoint)
	{
		FVector Delta = GetActorLocation() - CurrentPatrolPoint->GetActorLocation();
		float DistanceTarget = Delta.Size();

		if (DistanceTarget < 50.0f)
		{
			MoveToNextPatrolPoint();
		}
	}

}

void AFPSAIGuard::OnPawnSeen(APawn* SeenPawn)
{
	//UE_LOG(LogTemp, Warning, TEXT("AFPSAIGuard::OnPawnSeen"));

	if (SeenPawn == nullptr) return;

	//UE_LOG(LogTemp, Warning, TEXT("AFPSAIGuard::OnPawnSeen %s"), *SeenPawn->.GetName());
	DrawDebugSphere(GetWorld(), SeenPawn->GetActorLocation(), 32.0f, 12, FColor::Blue, false, 10.0f);
	
	AFPSGameMode* gm = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
	if (gm)
	{
		gm->CompletedMission(SeenPawn, false);
	}	

	SetGuardState(EAIState::Alerted);

	// When we see a pawn we don't patrol anymore
	AController* Controller = GetController();
	if (Controller != nullptr)
	{
		Controller->StopMovement();
	}

}

void AFPSAIGuard::OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume)
{
	if (GuardState == EAIState::Alerted) return;
	//UE_LOG(LogTemp, Warning, TEXT("AFPSAIGuard::OnNoiseHeard"));
	
	//DrawDebugSphere(GetWorld(), Location, 32.0f, 12, FColor::Green, false, 10.0f);

	// Get normalized direction between actor and where the noise was spawned
	FVector direction = Location - GetActorLocation();
	direction.Normalize();

	// Get rotation from that vector, take only Yall to rotate the guard
	FRotator NewLookAt = FRotationMatrix::MakeFromX(direction).Rotator();
	NewLookAt.Pitch = 0.0f;
	NewLookAt.Roll = 0.0f;

	SetActorRotation(NewLookAt);

	// Set timer to rotate the guard to its original rotation
	// Celar the previous timer
	GetWorldTimerManager().ClearTimer(TimerHandle_ResetOrientation);
	GetWorldTimerManager().SetTimer(TimerHandle_ResetOrientation, this, &AFPSAIGuard::ResetOrientation, 3.0f, false);
	
	SetGuardState(EAIState::Suspicious);	

	// When we see a pawn we don't patrol anymore
	AController* Controller = GetController();
	if (Controller != nullptr)
	{
		Controller->StopMovement();
	}
}

void AFPSAIGuard::ResetOrientation()
{
	if (GuardState == EAIState::Alerted) return;

	// Reset to original rotation
	SetActorRotation(OriginalRotation);

	SetGuardState(EAIState::Idle);

	if (bPatrol)
	{
		MoveToNextPatrolPoint();
	}
}

void AFPSAIGuard::MoveToNextPatrolPoint()
{
	if (CurrentPatrolPoint == nullptr || CurrentPatrolPoint == SecondPatrolPoint)
	{
		CurrentPatrolPoint = FirstPatrolPoint;
	}
	else
	{
		CurrentPatrolPoint = SecondPatrolPoint;
	}

	UNavigationSystem::SimpleMoveToActor(GetController(), CurrentPatrolPoint);
}

