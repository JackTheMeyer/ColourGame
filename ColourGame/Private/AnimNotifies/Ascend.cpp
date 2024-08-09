// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/Ascend.h"
#include "Characters/PlayerCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

void UAscend::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if(APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(MeshComp->GetOwner()))
	{
		PlayerCharacter->PlayerAscend();
	}
	else
	{
		UWorld* World = MeshComp->GetWorld();
		if(World)
		{
			FText DebugText = FText::FromString(TEXT("Error with Advance Action Ascend!"));
			UKismetSystemLibrary::PrintString(World, DebugText.ToString(), true, true, FLinearColor(1, 0, 0), 5.f);
		}
	}
}
