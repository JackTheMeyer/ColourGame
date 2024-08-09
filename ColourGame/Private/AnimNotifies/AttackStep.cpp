// Fill out your copyright notice in the Description page of Project Settings.
#include "AnimNotifies/AttackStep.h"
#include "Characters/PlayerCharacter.h"
#include "Kismet/KismetSystemLibrary.h"



void UAttackStep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if(APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(MeshComp->GetOwner()))
	{
		PlayerCharacter->AttackStep();
	} else
	{
		UWorld* World = MeshComp->GetWorld();
		if(World)
		{
			FText DebugText = FText::FromString(TEXT("Error with Advance Attack Step!!"));
			UKismetSystemLibrary::PrintString(World, DebugText.ToString(), true, true, FLinearColor(1, 0, 0), 5.f);
		}
	}
}
