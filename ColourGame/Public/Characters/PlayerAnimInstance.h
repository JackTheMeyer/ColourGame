// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LockOnTargetComponent.h"
#include "Animation/AnimInstance.h"
#include "Characters/CharacterTypes.h"
#include "PlayerAnimInstance.generated.h"


/**
 * 
 */

UCLASS()
class COLOURGAME_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category="Movement | Character State")
	ECharacterState CharacterState;

	UPROPERTY(BlueprintReadOnly, Category="Movement | Character State")
	ETargetState TargetState;

	UPROPERTY(BlueprintReadOnly, Category="Movement | Character State")
	EMovementState MovementState;

	UPROPERTY(BlueprintReadOnly, Category="Movement | Character State")
	EDirectionState DirectionState;

	UPROPERTY(BlueprintReadOnly, Category="Movement | Character State")
	EAscendState AscendState;
	
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	class APlayerCharacter* PlayerCharacter;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	class UCharacterMovementComponent* PlayerCharacterMovement;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	ULockOnTargetComponent* LockedTarget;
	
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float GroundSpeed;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool IsFalling;
};
