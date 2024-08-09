// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/WeaponCollision.h"

#include "Characters/PlayerCharacter.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Kismet/KismetSystemLibrary.h"

void UWeaponCollision::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float Duration)
{
	Super::NotifyBegin(MeshComp, Animation, Duration);

	if(APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(MeshComp->GetOwner()))
	{
		PlayerCharacter->SetWeaponCollisionEnabled(ECollisionEnabled::QueryOnly);
	} 
}

void UWeaponCollision::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);
	if(APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(MeshComp->GetOwner()))
	{
		PlayerCharacter->SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}