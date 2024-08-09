#include "Characters/PlayerCharacter.h"

#include "AsyncTreeDifferences.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "NiagaraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Items/Item.h"
#include "GenericPlatform/GenericPlatformProcess.h"
#include "Components/BoxComponent.h"
#include "Items/Weapon/Weapon.h"

/* Known TODOs
 * TODO: Create a debug build so DrawDebugMessage / UE_LOGs aren't causing perf issues in other builds
 * TODO: Movement logic out of Tick() into CharacterMovementComponent Tick() or shift to delegate / event model. Not sure if I'm keen on Root Motion
 * TODO: Question for Tutor: Does any BP Input Action overwrite C++ InputAction?
 * TODO: EndEquip() is Redundnant? Covered by EndAction()?
 * TODO: Set Weapon Collision to only be activatited during attack, not at all times
 * TODO: What is the diff between constructor and Super::BeingPlay()?
 * TODO: Sheathing and Drawing weapon currently uses 6 functions. That's ridicolous
 * TODO: Learn best debugging practices
 *
 * STATE MACHINE REFACTOR
 * INPUT-ENUMERATOR Global variable or something
 * NextState: Buffer to hold next state
 * Each input function calls state machine and passes in the action it wants to do
 * Each anim has a C++ AnimNotify that sets the state machine to advance to the next state
 * ABP is rewritten with new state machine in mind
 */

/* --------------------------------
 *	CORE - Constuctor, BeginPlay, Tick
 *-------------------------------- */
APlayerCharacter::APlayerCharacter()
{
	// Enables Ticking
	PrimaryActorTick.bCanEverTick = true;

	// TODO: Do these even need to be here
	// Controller rotates character - set to false for some reason
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Rotate the character in alignemnt with movement. If this was false the player mesh and root component rotation could be mis-aligned
	GetCharacterMovement()->bOrientRotationToMovement =true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	// Create a camera boom and attach it to the PlayerCharacter root component
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;

	// Attach a camera to the camera boom
	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);
	
	// Mapping player X and Y axis inputs to track the state the player is trying to move
	DirectionMap.Add(FDirectionKey(1.0f, 0.0f), EDirectionState::EDS_Forward);
	DirectionMap.Add(FDirectionKey(1.0f, 1.0f), EDirectionState::EDS_ForwardRight);
	DirectionMap.Add(FDirectionKey(1.0f, -1.0f), EDirectionState::EDS_ForwardLeft);
	DirectionMap.Add(FDirectionKey(0.0f, 1.0f), EDirectionState::EDS_Right);
	DirectionMap.Add(FDirectionKey(0.0f, -1.0f), EDirectionState::EDS_Left);
	DirectionMap.Add(FDirectionKey(-1.0f, 0.0f), EDirectionState::EDS_Backward);
	DirectionMap.Add(FDirectionKey(-1.0f, 1.0f), EDirectionState::EDS_BackwardRight);
	DirectionMap.Add(FDirectionKey(-1.0f, -1.0f), EDirectionState::EDS_BackwardLeft);

	JetNiagaraRight = CreateDefaultSubobject<UNiagaraComponent>("JetNiagaraRight");
	JetNiagaraRight->SetupAttachment(GetMesh(), TEXT("JetSocketRight"));

	JetNiagaraLeft = CreateDefaultSubobject<UNiagaraComponent>("JetNiagaraLeft");
	JetNiagaraLeft->SetupAttachment(GetMesh(), TEXT("JetSocketLeft"));
	
	// Makes the player character the default controlled character
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	JetNiagaraRight->ResetSystem();
	JetNiagaraLeft->ResetSystem();

	bInputLocked = false;
	ActionStateLocked = false;
}

// A tick is every frame. DeltaTime is the time between frames
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Updates DirectionMap based on player input
	CalculateDirection();
}

/* --------------------------------
 *	PLAYER CONTROL - Move, Turn
 *-------------------------------- */
void APlayerCharacter::MoveForward(float Value)
{
	// Store the Y direction in conjunction with the X direction in MoveRight()
	// Used to determine the direction the player is trying to move
	X_Direction = Value;
	if (CanMove(Value))
	{
		CalculateDirectionVector(EAxis::X, Value);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	// Store the Y direction in conjunction with the X direction in MoveForward()
	// Used to determine the direction the player is trying to move
	Y_Direction = Value;
	if (CanMove(Value))
	{
		CalculateDirectionVector(EAxis::Y, Value);
	}
}

bool APlayerCharacter::CanMove(float Value)
{
	return (ActionState == EActionState::EAS_Unoccupied) && ((Controller) && Value != 0.f);
}

void APlayerCharacter::CalculateDirectionVector(EAxis::Type Axis, float Value)
{
	if (Value == 0.f) return; // Early return if Value is zero

	const FRotator CurrentYawRotation = FRotator(0.f, GetControlRotation().Yaw, 0.f);
	if (LastYawRotation != CurrentYawRotation) // Check if the rotation has changed
		{
		// Update the cache
		LastYawRotation = CurrentYawRotation;
		CachedDirectionX = FRotationMatrix(LastYawRotation).GetUnitAxis(EAxis::X);
		CachedDirectionY = FRotationMatrix(LastYawRotation).GetUnitAxis(EAxis::Y);
		}

	// Use the cached direction
	const FVector Direction = (Axis == EAxis::X) ? CachedDirectionX : CachedDirectionY;
	AddMovementInput(Direction, Value);
}

void APlayerCharacter::Turn(float Value)
{
	//UE_LOG(LogTemp, Warning, TEXT("TURNING: %f"), Value);
	AddControllerYawInput(Value);
}

void APlayerCharacter::LookUp(float Value)
{
	//UE_LOG(LogTemp, Warning, TEXT("looking up: %f"), Value);
	AddControllerPitchInput(Value);
}

void APlayerCharacter::CalculateDirection()
{
	// Calculate Direction player is inputting from the controller
	FDirectionKey Key(X_Direction, Y_Direction);
	if (DirectionMap.Contains(Key))
	{
		DirectionState = DirectionMap[Key];
	}
	else
	{
		DirectionState = EDirectionState::EDS_Idle;
	}
}



/* --------------------------------
 *	TARGET LOCK - Done primarily in BPs
 *-------------------------------- */
void APlayerCharacter::TargetLock()
{
	// TODO: Question for Tutor: Does any BP InputAction overwrite C++ InputAction?
	return;
}

void APlayerCharacter::CancelTimer()
{
	if (GetWorld())
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(PlayerAscendTimer))
		{
			// Stop the timer if it's active		
			GetWorld()->GetTimerManager().ClearTimer(PlayerAscendTimer);
		}
	}
}

void APlayerCharacter::ResetTimer()
{
	if (GetWorld())
	{
		// Stop the timer, regardless of its state (will also stop an inactive timer, but no harm done)
		GetWorld()->GetTimerManager().ClearTimer(PlayerAscendTimer);

		// Restart the timer after stopping it
		GetWorld()->GetTimerManager().SetTimer(PlayerAscendTimer, this, &APlayerCharacter::PlayerDescend, 3.0f, false);
	}
}

void APlayerCharacter::TransitionActionState(EActionState nextState)
{
	if (nextState == EActionState::EAS_Waiting)
	{;
		ActionState = EActionState::EAS_Waiting;
		bInputLocked = false;
		UE_LOG(LogTemp, Warning, TEXT("NextActionState was EAS_Waiting - ActionState no EAS_WAiting input unlocked"));
		return;
	}

	if (!ActionStateLocked)
	{
		//ActionStateLocked = true;
		switch (nextState)
		{
		case EActionState::EAS_LightAttacking:
			UE_LOG(LogTemp, Warning, TEXT("Calling Light Attack from Transition Action State"));
			LightAttack();
			break;
		case EActionState::EAS_GroundLightAttacking1:
		case EActionState::EAS_GroundLightAttacking2:
		case EActionState::EAS_GroundLightAttacking3:
		case EActionState::EAS_GroundLightAttacking4:
			InputLock(true);
			GroundLightAttack(nextState);
			break;
		case EActionState::EAS_AirLightAttacking1:
		case EActionState::EAS_AirLightAttacking2:
		case EActionState::EAS_AirLightAttacking3:
		case EActionState::EAS_AirLightAttacking4:
			InputLock(true);
			AirLightAttack(nextState);
			break;
		case EActionState::EAS_GroundHeavyAttacking1:
		case EActionState::EAS_GroundHeavyAttacking2:
		case EActionState::EAS_GroundHeavyAttacking3:
		case EActionState::EAS_GroundHeavyAttacking4:
			InputLock(true);
			GroundHeavyAttack(nextState);
		case EActionState::EAS_AirHeavyAttacking1:
		case EActionState::EAS_AirHeavyAttacking2:
		case EActionState::EAS_AirHeavyAttacking3:
		case EActionState::EAS_AirHeavyAttacking4:
			InputLock(true);
			AirHeavyAttack(nextState);
			break;
		case EActionState::EAS_GroundSpecialAttacking:
			InputLock(true); 
			GroundSpecialAttack();
		case EActionState::EAS_AirSpecialAttacking:
			InputLock(true);
			AirSpecialAttack();
			break;
		case EActionState::EAS_Parry:
		case EActionState::EAS_Blocking:
			InputLock(true);
			Block(nextState);
			break;
		case EActionState::EAS_Jumping:
			PlayerJump();
			break;
		case EActionState::EAS_DoubleJumping:
			PlayerDoubleJump();
			break;
		case EActionState::EAS_Rolling:
		case EActionState::EAS_RollingFinal:
			Roll(nextState);
			break;
		case EActionState::EAS_Dashing:
		case EActionState::EAS_DashingFinal:
			Dash(nextState);
			break;
		case EActionState::EAS_Interacting:
			Interact();
		default:
			Unoccupied();
			break;
		}
		//NextActionState = EActionState::EAS_Unoccupied;
		//ActionStateLocked  = false;
	}
}

void APlayerCharacter::Unoccupied()
{
	ActionState = EActionState::EAS_Unoccupied;
}


/* --------------------------------
 *	PLAYER ACTION - Interact, Roll, Equip, Attack
 *-------------------------------- */
void APlayerCharacter::PlayerAscend()
{
	AscendHeight = GetActorLocation().Z;
	bIsAscended = true;
	AscendState = EAscendState::EASCS_Ascended;
	UCharacterMovementComponent* CharMovement = GetCharacterMovement();
	CharMovement->GravityScale = 0.0f;
	CharMovement->Velocity.Set(0.0f, 0.0f, 0.0f);
	ActionState = EActionState::EAS_Unoccupied;

	// New timer code
	// Create your timer here and tie it to your function
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(PlayerAscendTimer, this, &APlayerCharacter::PlayerDescend, 3.0f, false);
	}

}

void APlayerCharacter::ClearCombo()
{
	ComboState = EComboState::ECS_ComboZero;
	ActionState = EActionState::EAS_Unoccupied;
	NextActionState = EActionState::EAS_Unoccupied;
	bInputLocked = false;
}

void APlayerCharacter::PlayerDescend()
{
	bIsAscended = false;
	UCharacterMovementComponent* CharMovement = GetCharacterMovement();
	CharMovement->GravityScale = 1.0f;
	CharMovement->Velocity.Set(0.0f, 0.0f, -1250.0f);
	AscendState = EAscendState::EASCS_Descended;
	PlayAnimationMontage(DoubleJumpMontage, "DoubleJumpLand");
}


void APlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	
	if (bHasLanded == false)
	{
		GetCharacterMovement()->GravityScale = 1.0f;
		// Set the ActionState back to Unoccupied when the character lands
		ActionState = EActionState::EAS_Unoccupied;

		PlayAnimationMontage(DoubleJumpMontage, "DoubleJumpLand");
		// Set the landing state to true
		bHasLanded = true;
		AscendState = EAscendState::EASCS_Descended;
	}
}

void APlayerCharacter::RollLaunch(FVector DirectionVector, FName SectionName)
{
	ActionState = EActionState::EAS_Rolling;
	PlayAnimationMontage(RollMontage, SectionName);

	UCharacterMovementComponent* CharMovement = GetCharacterMovement();
	if (CharMovement)
	{
		CharMovement->GroundFriction = 2.0f;
	}

	// Use the provided DirectionVector for the launch
	LaunchCharacter(FVector(DirectionVector.X, DirectionVector.Y, 0.0f), false, false);
}

void APlayerCharacter::AttackLaunch()
{
	UCharacterMovementComponent* CharMovement = GetCharacterMovement();
	CharMovement->Velocity = FVector(0, 0, 0);
	// Get the character forward vector and multiply it by 250
	FVector ForwardLaunch = GetActorForwardVector() * 250.f;
	// Combine this with the launch on the z axis
	FVector LaunchVelocity = FVector(ForwardLaunch.X, ForwardLaunch.Y, 1230.0f);
	LaunchCharacter(LaunchVelocity, false, false);
}



void APlayerCharacter::AttackStep()
{
	if(bIsAscended){
		FVector NewLocation = GetActorLocation();
		NewLocation.Z = AscendHeight;
		SetActorLocation(NewLocation);
	}
	// Get the location of the target
	else if (LockedTarget && LockedTarget->GetTargetActor())
	{
		auto TargetLocation = LockedTarget->GetTargetActor()->GetActorLocation();
		
		// Get the location of the player
		auto PlayerLocation = GetActorLocation();

		// Calculate the direction toward the target from the player
		auto Direction = (TargetLocation - PlayerLocation).GetSafeNormal();

		// determine if distance to target is greater than 1000cm
		float DistanceToTarget = FVector::Dist(PlayerLocation, TargetLocation);
		if(DistanceToTarget > 200.0f)
		{
			// Define the force to be exerted
			const float LaunchForce = 1000.0f;  // Adjust this value as needed.

			// Launch the character forward towards the target
			LaunchCharacter(Direction * LaunchForce, true, true);
		}
	}
}

void APlayerCharacter::InputLock(bool locked)
{
	bInputLocked = locked;
}

void APlayerCharacter::GroundLightAttack(EActionState nextState)
{
	ActionState = nextState;
	PlayAnimationMontage(GroundLightAttackMontage, ActionStateFName[nextState]);
	//AttackStep();
	//AttackLaunch();
}

void APlayerCharacter::GroundHeavyAttack(EActionState nextState)
{
	ActionState = nextState;
	PlayAnimationMontage(AttackMontage, ActionStateFName[nextState]);
	AttackLaunch();
}

void APlayerCharacter::AirLightAttack(EActionState nextState)
{
	// TODO: Set Weapon Collision to only be activated during attack, not at all times
	ActionState = nextState;
	PlayAnimationMontage(AirLightAttackMontage, ActionStateFName[nextState]);
	//AttackLaunch();
}

void APlayerCharacter::AirHeavyAttack(EActionState nextState)
{
	ActionState = nextState;
	PlayAnimationMontage(AttackMontage, ActionStateFName[nextState]);
	AttackLaunch();
}

void APlayerCharacter::GroundSpecialAttack()
{
}

void APlayerCharacter::AirSpecialAttack()
{
}

void APlayerCharacter::Sheathe()
{
	if (CanDisarm())
	{
		DrawDebugMessage(2, TEXT("Disarm Montage"));
		ActionState = EActionState::EAS_EquippingWeapon;
		PlayAnimationMontage(EquipMontage, "Sheathe");
		//Disarm();
			
	}
	else if (CanArm())
	{
		DrawDebugMessage(2, TEXT("Arm Montage"));
		ActionState = EActionState::EAS_EquippingWeapon;
		PlayAnimationMontage(EquipMontage, "DrawWeapon");
	}
}

bool APlayerCharacter::CanArm()
{
	DrawDebugMessage(1, TEXT("Can Arm"));
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState == ECharacterState::ECS_Unequipped && 
		EquippedWeapon;
}

bool APlayerCharacter::CanDisarm()
{
	DrawDebugMessage(1, TEXT("Can DisArm"));
	return ActionState == EActionState::EAS_Unoccupied && 
		CharacterState != ECharacterState::ECS_Unequipped;
}

void APlayerCharacter::Arm()
{
	if (EquippedWeapon)
	{
		DrawDebugMessage(2, TEXT("Arming"));
		CharacterState = ECharacterState::ECS_EquippedTwoHandedWeapon;
		ActionState = EActionState::EAS_Unoccupied;
		//PlayEquipMontage("Equip");
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
	
}

void APlayerCharacter::Disarm()
{
	DrawDebugMessage(2, TEXT("Disarm Entered"));
	if (EquippedWeapon)
	{
		DrawDebugMessage(2, TEXT("Disarming"));
		CharacterState = ECharacterState::ECS_Unequipped;
		ActionState = EActionState::EAS_Unoccupied;
		//PlayEquipMontage("Unequip");
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}



/* --------------------------------
 *	PLAYER ACTION MOVEMENTS- Roll, Step, Airborne
 *-------------------------------- */
void APlayerCharacter::BufferPlayerJumpInput()
{
	// Player can only jump if they are unoccupied, or proceed to double jump if they are already double jumping
	// Player can double jump during grounded attack combo but not airborne attack combo

	if (ActionState == EActionState::EAS_Unoccupied)
	{
		TransitionActionState(EActionState::EAS_Jumping);
	}
}

void APlayerCharacter::PlayerJump() {
	ActionState = EActionState::EAS_Jumping;
	Super::Jump();
	bHasLanded = false;
	PlayAnimationMontage(DoubleJumpMontage, "JumpStart");
}

void APlayerCharacter::BufferPlayerDoubleJumpInput()
{
	// Player can only jump if they are unoccupied, or proceed to double jump if they are already double jumping
	// Player can double jump during grounded attack combo but not airborne attack combo

	if (ActionState == EActionState::EAS_Unoccupied)
	{
		TransitionActionState(EActionState::EAS_DoubleJumping);
	}
	else if (ActionState == EActionState::EAS_DoubleJumping)
	{
		return;
	}
	else
	{
		NextActionState = EActionState::EAS_DoubleJumping;
	}
}

void APlayerCharacter::PlayerDoubleJump()
{
	ActionState = EActionState::EAS_DoubleJumping;
	NextActionState = EActionState::EAS_Unoccupied;
	bHasLanded = false;
	PlayAnimationMontage(DoubleJumpMontage, "DoubleJumpStart");
		
	// Get the forward vector of the root component
	FVector ForwardVector = GetRootComponent()->GetForwardVector();
        
	// Multiply the forward vector by 1750
	FVector LaunchVelocity = ForwardVector;
	GetCharacterMovement()->GravityScale = 3.0f;
	
	// Launch the character using the X and Y components of the LaunchVelocity
	LaunchCharacter(FVector(LaunchVelocity.X, LaunchVelocity.Y, 2000.0f), false, false);
}

void APlayerCharacter::BufferInteractInput()
{
	bInputLocked = false;
	NextActionState = EActionState::EAS_Interacting;
	TransitionActionState(NextActionState);
	
}

void APlayerCharacter::Interact()
{
	// Checks to see if the player is overlapping an Item, casts it to weapon, and caches it
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon)
	{
		DrawDebugMessage(2, TEXT("Overlapping a weapon"));
		OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocket"));
		CharacterState = ECharacterState::ECS_EquippedTwoHandedWeapon;
		OverlappingItem = nullptr;
		EquippedWeapon = OverlappingWeapon;
		NextActionState = EActionState::EAS_Unoccupied;
		TransitionActionState(NextActionState);
	}
}

void APlayerCharacter::BufferRollInput()
{
	NextActionState = EActionState::EAS_Rolling;
}

void APlayerCharacter::Roll(EActionState nextState)
{
	FVector DirectionVector;
	FName SectionName;

	// Get the character's forward and right vectors
	FVector ForwardVector = GetActorForwardVector();
	FVector RightVector = GetActorRightVector();

	switch (DirectionState)
	{
	case EDirectionState::EDS_Backward:
		DirectionVector = -ForwardVector * 2000.0f;
		SectionName = "Roll_B";
		break;
	case EDirectionState::EDS_Left:
		DirectionVector = -RightVector * 2000.0f;
		SectionName = "Roll_L";
		break;
	case EDirectionState::EDS_Right:
		DirectionVector = RightVector * 2000.0f;
		SectionName = "Roll_R";
		break;
	default: // Forward or any other state
		DirectionVector = ForwardVector * 2000.0f;
		SectionName = "Roll";
		break;
	}

	RollLaunch(DirectionVector, SectionName);
}

void APlayerCharacter::BufferDashInput()
{
	NextActionState = EActionState::EAS_Dashing;
}

void APlayerCharacter::Dash(EActionState nextState)
{
	
}

void APlayerCharacter::BufferLightAttackInput()
{
	if (!bInputLocked)
	{
		//Logging
		UE_LOG(LogTemp, Warning, TEXT("Input Locked when at start of attack buffer"));
		bInputLocked = true;
		if (ActionState == EActionState::EAS_Unoccupied)
		{
			UE_LOG(LogTemp, Warning, TEXT("TransitionedActionState to EAS_LightAttacking during Light Attack Buffer"));
			TransitionActionState(EActionState::EAS_LightAttacking);
			
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Next Action State set to EAS_LightAttacking"));
			NextActionState = EActionState::EAS_LightAttacking;
		}
	}
	else
	{
		
		//Logging
		UE_LOG(LogTemp, Warning, TEXT("INPUT LOCKED WHEN CHECKING LIGHT ATTACK BUFFER ! !"));
	}
}

void APlayerCharacter::LightAttack()
{
	ActionState = EActionState::EAS_LightAttacking;
	UE_LOG(LogTemp, Warning, TEXT("Next Action State set to EAS_LightAttacking"));
	if (AscendState == EAscendState::EASCS_Ascended)
	{
		auto& comboInfo = AscendComboStateMap[ComboState];
		ComboState = comboInfo.NextState;
		PlayAnimationMontage(AirLightAttackMontage, comboInfo.SectionName);
	}
	else if (AscendState == EAscendState::EASCS_Descended)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not Ascended - Iterating Combo and playing attack montage"));
		auto& comboInfo = DescendComboStateMap[ComboState];
		ComboState = comboInfo.NextState;
		PlayAnimationMontage(GroundLightAttackMontage, comboInfo.SectionName);
	}
}

void APlayerCharacter::BufferHeavyAttackInput()
{
	switch(ActionState)
	{
	case EActionState::EAS_AirHeavyAttacking1:
		NextActionState = EActionState::EAS_AirHeavyAttacking2;
		break;
	case EActionState::EAS_AirHeavyAttacking2:
		NextActionState = EActionState::EAS_AirHeavyAttacking3;
		break;
	case EActionState::EAS_AirHeavyAttacking3:
		NextActionState = EActionState::EAS_AirHeavyAttacking4;
		break;
	case EActionState::EAS_GroundHeavyAttacking1:
		NextActionState = EActionState::EAS_GroundHeavyAttacking2;
		break;
	case EActionState::EAS_GroundHeavyAttacking2:
		NextActionState = EActionState::EAS_GroundHeavyAttacking3;
		break;
	case EActionState::EAS_GroundHeavyAttacking3:
		NextActionState = EActionState::EAS_GroundHeavyAttacking4;
		break;
	}
}

void APlayerCharacter::BufferSpecialAttackInput()
{
	NextActionState = EActionState::EAS_GroundSpecialAttacking;
}

void APlayerCharacter::BufferBlockInput()
{
	NextActionState = EActionState::EAS_Blocking;
}

void APlayerCharacter::Block(EActionState nextState)
{
	
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;

	static FVector2D PreviousMovementVector(0.0, 0.0);
	
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		bool bIsDirectionChanged180 =
			(PreviousMovementVector.Y >= 0.0f && MovementVector.Y < 0.0f) ||   // Moved from forward to backward
			(PreviousMovementVector.Y < 0.0f && MovementVector.Y >= 0.0f);     // Moved from backward to forward

		if (bIsDirectionChanged180)
		{
			MovementVector.Y *= -1;  // Reverse forward/backward direction
			// Apply a 180 degree rotation
			AddControllerYawInput(180.0f); 
		}

		Y_Direction = MovementVector.X;
		X_Direction = MovementVector.Y;
		AddMovementInput(ForwardDirection, MovementVector.Y * 0.66f);
		AddMovementInput(RightDirection, MovementVector.X * 0.66f);

		PreviousMovementVector = MovementVector;
	}
}

void APlayerCharacter::Walk(const FInputActionValue& Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;
	
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement
		Y_Direction = MovementVector.X;
		X_Direction = MovementVector.Y;
		AddMovementInput(ForwardDirection, MovementVector.Y * 0.33f);
		AddMovementInput(RightDirection, MovementVector.X * 0.33f);
	}
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X * 0.5f);
		AddControllerPitchInput(LookAxisVector.Y * 0.5f);
	}
}

/* --------------------------------
 *	UTILITY - PlayAnimationMontage, Weapon Collision
 *-------------------------------- */
void APlayerCharacter::PlayAnimationMontage(UAnimMontage* AnimMontage, FName SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AnimMontage)
	{
		AnimInstance->Montage_Play(AnimMontage);
		AnimInstance->Montage_JumpToSection(SectionName, AnimMontage);
	}
}

void APlayerCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();
	}
}




/* --------------------------------
 *	PLAYER INPUT
 *-------------------------------- */
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	FDateTime CurrentTime = FDateTime::Now();
	UE_LOG(LogTemp, Warning, TEXT("[%s] Setting up action bindings"), *CurrentTime.ToString());
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpPressAction, ETriggerEvent::Triggered, this, &APlayerCharacter::BufferPlayerJumpInput);
		EnhancedInputComponent->BindAction(DoubleJumpAction, ETriggerEvent::Completed, this, &APlayerCharacter::BufferPlayerDoubleJumpInput);
		
		EnhancedInputComponent->BindAction(LightAttackAction, ETriggerEvent::Triggered, this, &APlayerCharacter::BufferLightAttackInput);
		EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Triggered, this, &APlayerCharacter::BufferHeavyAttackInput);
		EnhancedInputComponent->BindAction(SpecialAttackAction, ETriggerEvent::Triggered, this, &APlayerCharacter::BufferSpecialAttackInput);
		EnhancedInputComponent->BindAction(BlockParryAction, ETriggerEvent::Triggered, this, &APlayerCharacter::BufferBlockInput);

		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &APlayerCharacter::BufferInteractInput);
		EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Triggered, this, &APlayerCharacter::BufferRollInput);
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &APlayerCharacter::BufferDashInput);
		
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		EnhancedInputComponent->BindAction(WalkAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Walk);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to set up Enhanced Input Component"));
	}
}




/* --------------------------------
 *	DEBUG
 *-------------------------------- */
void APlayerCharacter::DrawDebugMessage(int32 Key, const FString& Message)
{
	// TODO: Create debug build so this crap doesn't get included in test and prod builds
	if (GEngine)
	{
		float TimeToDisplay = 5.0f;
		FColor TextColor = FColor::Green; 

		GEngine->AddOnScreenDebugMessage(Key, TimeToDisplay, TextColor, Message);
	}
}

