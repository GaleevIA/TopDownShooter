
#include "TurretController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Sight.h"

void ATurretController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	if (IsValid(InPawn))
	{
		CachedTurret = StaticCast<ATurret*>(InPawn);
	}
	else
	{
		CachedTurret = nullptr;
	}
}

void ATurretController::ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	Super::ActorsPerceptionUpdated(UpdatedActors);
	if (!CachedTurret.IsValid() || CachedTurret->IsDead())
	{
		return;
	}

	AActor* ClosestActor = GetClosestSensedActor(UAISense_Damage::StaticClass());

	if (!IsValid(ClosestActor))
	{
		ClosestActor = GetClosestSensedActor(UAISense_Sight::StaticClass());
	}
	
	CachedTurret->CurrentTarget = ClosestActor;
	CachedTurret->OnCurrentTargetSet();
}

ATurretController::ATurretController()
{
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
}

AActor* ATurretController::GetClosestSensedActor(TSubclassOf<UAISense> SenseClass) const
{
	if (!IsValid(GetPawn()))
	{
		return nullptr;
	}

	TArray<AActor*> SensedActors;
	PerceptionComponent->GetCurrentlyPerceivedActors(SenseClass, SensedActors);

	AActor* ClosestActor = nullptr;
	float MinSquaredDistance = FLT_MAX;
	FVector PawnLocation = GetPawn()->GetActorLocation();

	for (AActor* SeenActor : SensedActors)
	{
		float CurrentSquaredDistance = (PawnLocation - SeenActor->GetActorLocation()).SizeSquared();
		if (CurrentSquaredDistance < MinSquaredDistance)
		{
			MinSquaredDistance = CurrentSquaredDistance;
			ClosestActor = SeenActor;
		}
	}
	return ClosestActor;
}

