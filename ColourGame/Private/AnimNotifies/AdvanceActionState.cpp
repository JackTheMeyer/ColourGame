// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotifies/AdvanceActionState.h"
#include "Characters/PlayerCharacter.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

void UAdvanceActionState::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    Super::Notify(MeshComp, Animation);

    if(APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(MeshComp->GetOwner()))
    {
        PlayerCharacter->TransitionActionState(PlayerCharacter->GetNextActionState());
    } else
    {
        UWorld* World = MeshComp->GetWorld();
        if(World)
        {
            FText DebugText = FText::FromString(TEXT("Error with Advance Action State!"));
            UKismetSystemLibrary::PrintString(World, DebugText.ToString(), true, true, FLinearColor(1, 0, 0), 5.f);
        }
    }
}