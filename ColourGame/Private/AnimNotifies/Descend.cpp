// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/Descend.h"
#include "Characters/PlayerCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

void UDescend::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if(APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(MeshComp->GetOwner()))
	{
		PlayerCharacter->PlayerDescend();
	}
	else
	{
		UWorld* World = MeshComp->GetWorld();
		if(World)
		{
			FText DebugText = FText::FromString(TEXT("Error with Advance Descend"));
			UKismetSystemLibrary::PrintString(World, DebugText.ToString(), true, true, FLinearColor(1, 0, 0), 5.f);
		}
	}
}