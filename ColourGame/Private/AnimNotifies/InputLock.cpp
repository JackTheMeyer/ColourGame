// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/InputLock.h"
#include "Characters/PlayerCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

void UInputLock::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if(APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(MeshComp->GetOwner()))
	{
		PlayerCharacter->InputLock(false);
	}
	else
	{
		UWorld* World = MeshComp->GetWorld();
		if(World)
		{
			FText DebugText = FText::FromString(TEXT("Error with Input Lock"));
			UKismetSystemLibrary::PrintString(World, DebugText.ToString(), true, true, FLinearColor(1, 0, 0), 5.f);
		}
	}
}
