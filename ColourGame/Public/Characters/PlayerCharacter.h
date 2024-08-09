// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterTypes.h"
#include "InputAction.h"
#include "LockOnTargetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerCharacter.generated.h"

class UInputMappingContext;
class UNiagaraComponent;
class USpringArmComponent;
class UCameraComponent;
class AItem;
class UProceduralMeshComponent;
class UAnimMontage;
class ULockOnTargetComponent;
class AWeapon;

UCLASS()
class COLOURGAME_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void AttackStep();
	void InputLock(bool locked);
	void PlayerAscend();
	void PlayerDescend();
	void ClearCombo();
	void Unoccupied();
	
	/* --------------------------------
	 *	MEMBER FUNCTIONS
	*-------------------------------- */
	void TransitionActionState(EActionState nextState);
	
	static void DrawDebugMessage(int32 Key, const FString& Message);
	
protected:
	UFUNCTION(BlueprintCallable)
	void Arm();
	
	UFUNCTION(BlueprintCallable)
	void Disarm();
	
private:
	/* * * * * * * * * * * * * *
	 * --- INPUT BUFFER FUNCTIONS ---
	 *  * * * * * * * * * * * * */
	void BufferPlayerJumpInput();
	void BufferPlayerDoubleJumpInput();
	void BufferInteractInput();
	void BufferRollInput();
	void BufferDashInput();
	void BufferLightAttackInput();
	void BufferHeavyAttackInput();
	void BufferSpecialAttackInput();
	void BufferBlockInput();

	/* * * * * * * * * * * * * *
	 * --- INPUT FUNCTIONS ---
	 *  * * * * * * * * * * * * */
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	void Walk(const FInputActionValue& Value);
	
	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	
	/* * * * * * * * * * * * * *
	 * --- OTHER FUNCTIONS ---
	 *  * * * * * * * * * * * * */
	
	virtual void BeginPlay() override;
	void MoveForward(float Value);
	void MoveRight(float Value);
	bool CanMove(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void CalculateDirectionVector(EAxis::Type axis, float Value);
	void CalculateDirection();

	void TargetLock();

	void CancelTimer();
	void ResetTimer();

	void Interact();
	void PlayerJump();
	void PlayerDoubleJump();
	virtual void Landed(const FHitResult& Hit) override;
	void Roll(EActionState nextState);
	void RollLaunch(FVector launchValue, FName SectionName);
	void Dash(EActionState nextState);
	void AttackLaunch();
	void LightAttack();
	void GroundLightAttack(EActionState nextState);
	void GroundHeavyAttack(EActionState nextState);
	void AirLightAttack(EActionState nextState);
	void AirHeavyAttack(EActionState nextState);
	void GroundSpecialAttack();
	void AirSpecialAttack();
	void Block(EActionState nextState);
	void Sheathe();
	bool CanArm();
	bool CanDisarm();

	void PlayAnimationMontage(UAnimMontage* AnimMontage, FName SectionName);
	

	/* --------------------------------
	*	MEMBER VARIABLES
	*-------------------------------- */
public:
	UPROPERTY(BlueprintReadWrite, meta = (AllowsPrivateAccess = "true"))
	EActionState NextActionState = EActionState::EAS_Unoccupied;
	
	UPROPERTY(BlueprintReadWrite, meta = (AllowsPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;
	
	UPROPERTY(BlueprintReadOnly)
	bool bInputLocked;

	bool ActionStateLocked;

	bool bIsAscended;
	float AscendHeight;

protected:
	UPROPERTY(BlueprintReadWrite, meta = (AllowsPrivateAccess = "true"))
	ULockOnTargetComponent* LockedTarget;
	
private:
	FTimerHandle PlayerAscendTimer;
	
	//  Mapped Direction for Player Input
	TMap<FDirectionKey, EDirectionState> DirectionMap;
	
	FRotator OriginalCameraRotation;
	
	FVector StartLocation;
	FVector EndLocation;

	FVector CachedDirectionX;
	FVector CachedDirectionY;
	FRotator LastYawRotation;
	
	float MovementDuration;
	float CurrentLerpTime;

	float Y_Direction;
	float X_Direction;
	
	float LastFrameDotProduct;

	float OriginalZVelocity;

	bool bHasLanded;
	/* --------------------------------
	 *	INPUT PROPERTIES 
	*-------------------------------- */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effects", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LightAttackAction;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* HeavyAttackAction;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SpecialAttackAction;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* BlockParryAction;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractAction;
	
	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RollAction;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DashAction;
	
	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;
	
	/** Jump-Press Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpPressAction;

	/** Jump-Hold Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpHoldAction;

	/** Jump-Hold Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DoubleJumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* WalkAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;
	
	/* --------------------------------
	 *	MEMBER VARIABLES EXPOSED AS PROPERTIES 
	*-------------------------------- */
	UPROPERTY(VisibleAnywhere);
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere);
	UCameraComponent* ViewCamera;

	UPROPERTY(VisibleAnywhere);
	AItem* OverlappingItem;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	AWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* JetNiagara;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effects", meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* JetNiagaraRight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effects", meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* JetNiagaraLeft;

	

	/* --------------------------------
	 *	Anim Montages
	*-------------------------------- */
	UPROPERTY(EditAnywhere, Category = Montages);
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, Category = Montages);
	UAnimMontage* AirLightAttackMontage;

	UPROPERTY(EditAnywhere, Category = Montages);
	UAnimMontage* GroundLightAttackMontage;

	UPROPERTY(EditAnywhere, Category = Montages);
	UAnimMontage* EquipMontage;
	
	UPROPERTY(EditAnywhere, Category = Montages);
	UAnimMontage* RollMontage;

	UPROPERTY(EditAnywhere, Category = Montages);
	UAnimMontage* DoubleJumpMontage;

	


protected:
	/* --------------------------------
	 *	ENUMERATED STATES 
	*-------------------------------- */
	UPROPERTY(BlueprintReadWrite, meta = (AllowsPrivateAccess = "true"))
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(BlueprintReadWrite, meta = (AllowsPrivateAccess = "true"))
	EAttackState AttackState = EAttackState::EATS_LightAttack;

	UPROPERTY(BlueprintReadWrite, meta = (AllowsPrivateAccess = "true"))
	EMovementState MovementState = EMovementState::EMS_Idle;
	
	UPROPERTY(BlueprintReadWrite, meta = (AllowsPrivateAccess = "true"))
	ERollState RollState = ERollState::ERS_RollImobile;

	UPROPERTY(BlueprintReadWrite, meta = (AllowsPrivateAccess = "true"))
	ETurnState TurnState = ETurnState::ETS_NotTurning;

	UPROPERTY(BlueprintReadWrite, meta = (AllowsPrivateAccess = "true"))
	EStepState StepState = EStepState::ESS_NotStepping;
	
	UPROPERTY(BlueprintReadWrite, meta = (AllowsPrivateAccess = "true"))
	ETargetState TargetState = ETargetState::ETS_NoTarget;

	UPROPERTY(BlueprintReadWrite, meta = (AllowsPrivateAccess = "true"))
	EDirectionState DirectionState = EDirectionState::EDS_Idle;

	UPROPERTY(BlueprintReadWrite, meta = (AllowsPrivateAccess = "true"))
	EAirborneState AirborneState = EAirborneState::EAS_AirborneDown;

	UPROPERTY(BlueprintReadWrite, meta = (AllowsPrivateAccess = "true"))
	ESwingState SwingState = ESwingState::ESS_FirstSwing;

	UPROPERTY(BlueprintReadWrite, meta = (AllowsPrivateAccess = "true"))
	EAscendState AscendState = EAscendState::EASCS_Descended;
	
	UPROPERTY(BlueprintReadWrite, meta = (AllowsPrivateAccess = "true"))
	EComboState ComboState = EComboState::ECS_ComboZero;

public:
	/* --------------------------------
	 *	INLINE GETTERS AND SETTERS
	*-------------------------------- */
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }

	FORCEINLINE ULockOnTargetComponent* GetLockOnTargetComponent() const  { return LockedTarget; }

	FORCEINLINE EActionState GetNextActionState() const { return NextActionState; }
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE ETargetState GetTargetState() const { return TargetState; }
	FORCEINLINE ETurnState GetTurnState() const { return TurnState; }
	FORCEINLINE EMovementState GetMovementState() const { return MovementState; }
	FORCEINLINE EDirectionState GetDirectionState() const { return DirectionState; }
	FORCEINLINE EAscendState GetAscendedState() const { return AscendState; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);
	
	UFUNCTION(BlueprintCallable, Category = "PlayerCharacter")
	FORCEINLINE void SetGroundFrictionToDefault() 
	{
		if (UCharacterMovementComponent* CharMovement = GetCharacterMovement())
		{
			CharMovement->GroundFriction = 8.0f;
		}
	}

	
};
