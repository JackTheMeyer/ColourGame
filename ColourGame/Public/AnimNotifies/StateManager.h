// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "StateManager.generated.h"

/**
 * 
 */
UCLASS()
class COLOURGAME_API UStateManager : public UAnimNotifyState
{
	GENERATED_BODY()

	void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float Duration);
	void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float duration);
	void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation);
};
