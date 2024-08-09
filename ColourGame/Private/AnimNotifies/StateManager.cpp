// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/StateManager.h"
#include "Characters/PlayerCharacter.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Kismet/KismetSystemLibrary.h"
 
 void UStateManager::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float Duration)
 {
 	Super::NotifyBegin(MeshComp, Animation, Duration);
 
 	if(APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(MeshComp->GetOwner()))
 	{
 		PlayerCharacter->NextActionState = EActionState::EAS_Waiting;
 		PlayerCharacter->bInputLocked = false;
 		
 		UE_LOG(LogTemp, Log, TEXT("Player character's state has been updated to EAS_Waiting From StateManager AnimNotifyState."));
 	} else {
 		UE_LOG(LogTemp, Warning, TEXT("Could not cast the component owner to APlayerCharacter."));
 	}
 }

void UStateManager::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float duration)
{
	Super::NotifyTick(MeshComp, Animation, duration);
	
	if(APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(MeshComp->GetOwner()))
	{
		// Only transition the action state if the next action state is not "waiting".
		if (PlayerCharacter->NextActionState != EActionState::EAS_Waiting && PlayerCharacter->NextActionState !=EActionState::EAS_Unoccupied)
		{
			PlayerCharacter->TransitionActionState(PlayerCharacter->NextActionState);
			UE_LOG(LogTemp, Warning, TEXT("Transitioned action state to: %s"), *UEnum::GetValueAsString(PlayerCharacter->NextActionState));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Next action state is Waiting or Unoccupied during State Manager Tick()."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter cast failed."));
	}
}

void UStateManager::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);
	if(APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(MeshComp->GetOwner()))
	{
		//PlayerCharacter->ClearCombo();
		//Logging
		UE_LOG(LogTemp, Warning, TEXT("NotifyEnd has been triggered from State Manager."));
	}
}
