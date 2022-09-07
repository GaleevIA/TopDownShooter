
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AISense.h"
#include "TopDownShooter/Turret.h"
#include "TurretController.generated.h"

/**
 * 
 */
UCLASS()
class ATurretController : public AAIController
{
	GENERATED_BODY()

public:
	ATurretController();
	
	virtual void SetPawn(APawn* InPawn) override;

	virtual void ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors) override;
	
protected:
	AActor* GetClosestSensedActor(TSubclassOf<UAISense> SenseClass) const;
	
private:
	TWeakObjectPtr<ATurret> CachedTurret;
};
