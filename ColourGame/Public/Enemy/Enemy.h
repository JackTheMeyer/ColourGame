// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Enemy.generated.h"

class UAnimMontage;

UCLASS()
class COLOURGAME_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void DirectionHitReact(const FVector& ImpactPoint);
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	

private:
	UPROPERTY(EditAnywhere, Category = Montages);
	UAnimMontage* GetHitMontage;

	UPROPERTY(EditAnywhere, Category=Sounds)
	USoundBase* HitSound;

	void PlayAnimationMontage(UAnimMontage* AnimMontage, FName SectionName);

	UPROPERTY(EditAnywhere, Category=VisualEffects)
	UParticleSystem* HitParticles;
	
};
