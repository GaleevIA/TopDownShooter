// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Turret.generated.h"

UENUM(BlueprintType)
enum class ETurretState : uint8
{
	Searching,
	Firing
};

class UWeaponBarellComponent;
UCLASS()
class ATurret : public APawn
{
	GENERATED_BODY()

public:
	ATurret();
	
	virtual void Tick(float DeltaTime) override;

	void OnCurrentTargetSet();

	bool IsDead();
	
	AActor* CurrentTarget = nullptr;

	virtual FVector GetPawnViewLocation() const override;

	virtual FRotator GetViewRotation() const override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* TurretBaseComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret parameters", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float BaseSearchingRotationRate = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret parameters", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float BaseFiringInterpSpeed = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret parameters", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float PitchRotationRate = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret parameters | Fire", meta = (ClampMin = 1.0f, UIMin = 1.0f))
	float RateOfFire = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret parameters | Fire", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float FireDelayTime = 1.0f;	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret parameters | Fire", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float Damage = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret parameters | Health", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MaxHealth = 100.0f;
	
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
private:
	void SearchingMovement(float DeltaTime);
	void FiringMovement(float DeltaTime);

	void SetCurrentTurretState(ETurretState NewState);
	ETurretState CurrentTurretState = ETurretState::Searching;
	
	float GetFireInterval() const;
	void Attack();

	FTimerHandle ShotTimer;

	float CurHealth = 100.0f;
};
