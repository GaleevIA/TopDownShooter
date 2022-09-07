
#include "Turret.h"
#include "AIController.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ATurret::ATurret()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* TurretRoot = CreateDefaultSubobject<USceneComponent>(TEXT("TurretRoot"));
	SetRootComponent(TurretRoot);

	TurretBaseComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TurretBase"));
	TurretBaseComponent->SetupAttachment(TurretRoot);

	CurHealth = MaxHealth;
}

void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	switch (CurrentTurretState)
	{
		case ETurretState::Searching:
		{
			SearchingMovement(DeltaTime);
			break;
		}
		case ETurretState::Firing:
		{
			FiringMovement(DeltaTime);
			break;
		}
	}
}

void ATurret::OnCurrentTargetSet()
{
	ETurretState NewState = IsValid(CurrentTarget) ? ETurretState::Firing : ETurretState::Searching;
	SetCurrentTurretState(NewState);
}

bool ATurret::IsDead()
{
	return CurHealth <= 0;
}

FVector ATurret::GetPawnViewLocation() const
{
	return TurretBaseComponent->GetComponentLocation();
}

FRotator ATurret::GetViewRotation() const
{
	return TurretBaseComponent->GetComponentRotation();
}

float ATurret::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	CurHealth -= DamageAmount;

	if (CurHealth <= 0)
	{
		SetActorHiddenInGame(true);
		
		SetActorEnableCollision(false);
		
		SetActorTickEnabled(false);
	}
	
	return DamageAmount;
}

void ATurret::SearchingMovement(float DeltaTime)
{
	FRotator TurretBaseRotation = TurretBaseComponent->GetRelativeRotation();
	TurretBaseRotation.Yaw += DeltaTime * BaseSearchingRotationRate;
	TurretBaseComponent->SetRelativeRotation(TurretBaseRotation);
}

void ATurret::FiringMovement(float DeltaTime)
{
	FVector BaseLookAtDirection = (CurrentTarget->GetActorLocation() - TurretBaseComponent->GetComponentLocation()).GetSafeNormal2D();
	FQuat LookAtQuat = BaseLookAtDirection.ToOrientationQuat(); 
	FQuat TargetQuat = FMath::QInterpTo(TurretBaseComponent->GetComponentQuat(), LookAtQuat, DeltaTime, BaseFiringInterpSpeed);
	TurretBaseComponent->SetWorldRotation(TargetQuat);
}

void ATurret::SetCurrentTurretState(ETurretState NewState)
{
	bool bIsStateChanged = NewState != CurrentTurretState;
	CurrentTurretState = NewState;

	if (!bIsStateChanged)
	{
		return;
	}

	switch (CurrentTurretState)
	{
		case ETurretState::Searching:
		{
			GetWorld()->GetTimerManager().ClearTimer(ShotTimer);
			break;
		}
		case ETurretState::Firing:
		{
			GetWorld()->GetTimerManager().SetTimer(ShotTimer, this, &ATurret::Attack, GetFireInterval(), true, FireDelayTime);
			break;
		}
	}
}

float ATurret::GetFireInterval() const
{
	return 60.0f / RateOfFire;
}

void ATurret::Attack()
{
	if (CurrentTarget->CanBeDamaged())
	{
		DrawDebugLine(GetWorld(), GetActorLocation(), CurrentTarget->GetActorLocation(), FColor::Red, false, 1, 0, 1);
		
		CurrentTarget->TakeDamage(Damage, FDamageEvent(UDamageType::StaticClass()), GetController(), this);
	}
}