// Copyright Epic Games, Inc. All Rights Reserved.

#include "TopDownShooterPlayerController.h"

#include "ChaosInterfaceWrapperCore.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "TopDownShooterCharacter.h"
#include "Engine/World.h"

ATopDownShooterPlayerController::ATopDownShooterPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void ATopDownShooterPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	CachedPlayer = Cast<ATopDownShooterCharacter>(InPawn);
}

void ATopDownShooterPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// keep updating the destination every tick while desired
	if (bMoveToMouseCursor)
	{
		MoveToMouseCursor();
	}
}

void ATopDownShooterPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &ATopDownShooterPlayerController::OnSetDestinationPressed);
	//InputComponent->BindAction("PickUp", IE_Pressed, this, &ATopDownShooterPlayerController::OnPickUpPressed);
	InputComponent->BindAction("Attack", IE_Pressed, this, &ATopDownShooterPlayerController::OnAttackPressed);

}

void ATopDownShooterPlayerController::MoveToMouseCursor()
{
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		if (ATopDownShooterCharacter* MyPawn = Cast<ATopDownShooterCharacter>(GetPawn()))
		{
			if (MyPawn->GetCursorToWorld())
			{
				UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, MyPawn->GetCursorToWorld()->GetComponentLocation());
			}
		}
	}
	else
	{
		// Trace to see what is under the mouse cursor
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_Visibility, false, Hit);

		if (Hit.bBlockingHit)
		{
			// We hit something, move there
			SetNewMoveDestination(Hit.ImpactPoint);
		}
	}

	bMoveToMouseCursor = false;
}

void ATopDownShooterPlayerController::SetNewMoveDestination(const FVector DestLocation)
{
	APawn* const MyPawn = GetPawn();
	if (MyPawn)
	{
		float const Distance = FVector::Dist(DestLocation, MyPawn->GetActorLocation());

		// We need to issue move command only if far enough in order for walk animation to play correctly
		if ((Distance > 120.0f))
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DestLocation);
		}
	}
}

void ATopDownShooterPlayerController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
}

void ATopDownShooterPlayerController::OnPickUpPressed()
{
	
}

void ATopDownShooterPlayerController::OnAttackPressed()
{
	if (IsValid(CachedPlayer))
	{
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_Visibility, false, Hit);

		if (Hit.bBlockingHit)
		{
			CachedPlayer->Attack(Hit.ImpactPoint);
		}
	}
}
