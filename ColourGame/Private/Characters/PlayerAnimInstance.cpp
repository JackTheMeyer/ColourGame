// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PlayerAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Characters/PlayerCharacter.h"
#include "Engine/Engine.h"
#include "Kismet/KismetMathLibrary.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	PlayerCharacter = Cast<APlayerCharacter>(TryGetPawnOwner());
	if (PlayerCharacter)
	{
		PlayerCharacterMovement = PlayerCharacter->GetCharacterMovement();
	}
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (PlayerCharacterMovement)
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(PlayerCharacterMovement->Velocity);
		IsFalling = PlayerCharacterMovement->IsFalling();
		CharacterState = PlayerCharacter->GetCharacterState();
		TargetState = PlayerCharacter->GetTargetState();
		MovementState = PlayerCharacter->GetMovementState();
		DirectionState = PlayerCharacter->GetDirectionState();
		LockedTarget = PlayerCharacter->GetLockOnTargetComponent();
		AscendState = PlayerCharacter->GetAscendedState();
	}
}

