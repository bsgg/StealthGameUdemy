// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSAIGuard.h"
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "FPSGameMode.h"


// Sets default values
AFPSAIGuard::AFPSAIGuard()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// No need to attach this to anything because is not a scene component
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	PawnSensingComp->OnSeePawn.AddDynamic(this, &AFPSAIGuard::OnPawnSeen);
	PawnSensingComp->OnHearNoise.AddDynamic(this, &AFPSAIGuard::OnNoiseHeard);
}

// Called when the game starts or when spawned
void AFPSAIGuard::BeginPlay()
{
	Super::BeginPlay();

	OriginalRotation = GetActorRotation();
	
}

// Called every frame
void AFPSAIGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

}

void AFPSAIGuard::OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume)
{
	//UE_LOG(LogTemp, Warning, TEXT("AFPSAIGuard::OnNoiseHeard"));
	
	DrawDebugSphere(GetWorld(), Location, 32.0f, 12, FColor::Green, false, 10.0f);

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
	
}

void AFPSAIGuard::ResetOrientation()
{
	// Reset to original rotation
	SetActorRotation(OriginalRotation);
}

