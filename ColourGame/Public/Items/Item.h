// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"
class USphereComponent;

UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_Hovering,
	EIS_Equipped
};

UCLASS()
class COLOURGAME_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// protected exposes to event graph
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite,Category="Sine Parameters", meta=(AllowPrivateAccess = "true"))
	float Amplitude = 0.25f;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="Sine Parameters", meta=(AllowPrivateAccess = "true"))
	float TimeConstant = 10.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* ItemMesh;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereVar;
	
	template<typename T>
	T Avg(T First, T Second);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure)
	float TransformedSin();

	UFUNCTION(BlueprintPure)
	float TransformedCos();

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="Sine Parameters", meta=(AllowPrivateAccess = "true"))
	EItemState ItemState = EItemState::EIS_Hovering;
	
private:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	float RunningTime;

};
