#pragma once
#include <map>

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),
	ECS_EquippedOneHandedWeapon UMETA(DisplayName = "Equipped One-Handed Weapon"),
	ECS_EquippedTwoHandedWeapon UMETA(DisplayName = "Equipped Two-Handed Weapon")
};

UENUM(BlueprintType)
enum class EMovementState : uint8
{
	EMS_Idle UMETA(DisplayName = "Idle"),
	EMS_WalkingForward UMETA(DisplayName = "WalkingForward"),
	EMS_WalkingLeft UMETA(DisplayName = "WalkingLeft"),
	EMS_WalkingRight UMETA(DisplayName = "WalkingRight"),
	EMS_RunningForward UMETA(DisplayName = "RunningForward"),
	EMS_RunningLeft UMETA(DisplayName = "RunningLeft"),
	EMS_RunningRight UMETA(DisplayName = "RunningRight"),
	EMS_SprintingForward UMETA(DisplayName = "SprintingForward"),
	EMS_SprintingLeft UMETA(DisplayName = "SprintingLeft"),
	EMS_SprintingRight UMETA(DisplayName = "SprintingRight"),
	EMS_StrafeLeft UMETA(DisplayName = "StrafeLeft"),
	EMS_StrafeRight UMETA(DisplayName = "StrafeRight"),
	EMS_WalkBackwards UMETA(DisplayName = "WalkBackwards"),
	EMS_RunBackwards UMETA(DisplayName = "RunBackwards"),
	EMS_InAction UMETA(DisplayName = "InAction"),
};

UENUM(BlueprintType)
enum class EDirectionState : uint8
{
	EDS_Idle UMETA(DisplayName = "Idle"),
	EDS_Forward UMETA(DisplayName = "Forward"),
	EDS_ForwardLeft UMETA(DisplayName = "ForwardLeft"),
	EDS_ForwardRight UMETA(DisplayName = "ForwardRight"),
	EDS_Left UMETA(DisplayName = "Left"),
	EDS_Right UMETA(DisplayName = "Right"),
	EDS_Backward UMETA(DisplayName = "Backward"),
	EDS_BackwardLeft UMETA(DisplayName = "BackwardLeft"),
	EDS_BackwardRight UMETA(DisplayName = "BackwardRight"),
};

UENUM(BlueprintType)
enum class EComboState : uint8
{
	ECS_ComboZero UMETA(DisplayName = "ComboZero"),
	ECS_ComboOne UMETA(DisplayName = "ComboOne"),
	ECS_ComboTwo UMETA(DisplayName = "ComboTwo"),
	ECS_ComboThree UMETA(DisplayName = "ComboThree"),
	ECS_ComboFour UMETA(DisplayName = "ComboFour"),
	ECS_ComboFive UMETA(DisplayName = "ComboFive"),
};

struct ComboInfo
{
	FName SectionName;
	EComboState NextState;
};

inline std::map<EComboState, ComboInfo> AscendComboStateMap = {
	{EComboState::ECS_ComboZero, ComboInfo{"AirLightAttacking1", EComboState::ECS_ComboOne}},
	{EComboState::ECS_ComboOne,  ComboInfo{"AirLightAttacking2", EComboState::ECS_ComboTwo}},
	{EComboState::ECS_ComboTwo,  ComboInfo{"AirLightAttacking3", EComboState::ECS_ComboThree}}
};

inline std::map<EComboState, ComboInfo> DescendComboStateMap = {
	{EComboState::ECS_ComboZero, ComboInfo{"GroundLightAttacking1", EComboState::ECS_ComboOne}},
	{EComboState::ECS_ComboOne,  ComboInfo{"GroundLightAttacking2", EComboState::ECS_ComboTwo}},
	{EComboState::ECS_ComboTwo,  ComboInfo{"GroundLightAttacking3", EComboState::ECS_ComboThree}}
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"),

	EAS_Waiting UMETA(DisplayName = "Waiting"),

	EAS_LightAttacking UMETA(DisplayName = "LightAttacking"),
	EAS_HeavyAttacking UMETA(DisplayName = "HeavyAttacking"),
	EAS_SpecialAttacking UMETA(DisplayName = "SpecialAttacking"),
	EAS_GroundLightAttacking1 UMETA(DisplayName = "GroundLightAttacking1"),
	EAS_GroundLightAttacking2 UMETA(DisplayName = "GroundLightAttacking2"),
	EAS_GroundLightAttacking3 UMETA(DisplayName = "GroundLightAttacking3"),
	EAS_GroundLightAttacking4 UMETA(DisplayName = "GroundLightAttacking4"),

	EAS_GroundHeavyAttacking1 UMETA(DisplayName = "GroundHeavyAttacking1"),
	EAS_GroundHeavyAttacking2 UMETA(DisplayName = "GroundHeavyAttacking2"),
	EAS_GroundHeavyAttacking3 UMETA(DisplayName = "GroundHeavyAttacking3"),
	EAS_GroundHeavyAttacking4 UMETA(DisplayName = "GroundHeavyAttacking4"),

	EAS_AirLightAttacking1 UMETA(DisplayName = "AirLightAttacking1"),
	EAS_AirLightAttacking2 UMETA(DisplayName = "AirLightAttacking2"),
	EAS_AirLightAttacking3 UMETA(DisplayName = "AirLightAttacking3"),
	EAS_AirLightAttacking4 UMETA(DisplayName = "AirLightAttacking4"),

	EAS_AirHeavyAttacking1 UMETA(DisplayName = "AirHeavyAttacking1"),
	EAS_AirHeavyAttacking2 UMETA(DisplayName = "AirHeavyAttacking2"),
	EAS_AirHeavyAttacking3 UMETA(DisplayName = "AirHeavyAttacking3"),
	EAS_AirHeavyAttacking4 UMETA(DisplayName = "AirHeavyAttacking4"),
	
	EAS_GroundSpecialAttacking UMETA(DisplayName = "GroundSpecialAttacking"),
	EAS_AirSpecialAttacking UMETA(DisplayName = "AirSpecialAttacking"),

	EAS_Parry UMETA(DisplayName = "Parry"),
	EAS_Blocking UMETA(DisplayName = "Blocking"),

	EAS_Falling UMETA(DisplayName = "Falling"),
	EAS_Jumping UMETA(DisplayName = "Jumping"),

	EAS_Rolling UMETA(DisplayName = "Rolling"),
	EAS_RollingFinal UMETA(DisplayName = "RollingFinal"),
	EAS_Dashing UMETA(DisplayName = "Dashing"),
	EAS_DashingFinal UMETA(DisplayName = "DashingFinal"),

	EAS_DoubleJumping UMETA(DisplayName = "DoubleJumping"),
	EAS_Ascended UMETA(DisplayName = "Ascended"),
	EAS_Descending UMETA(DisplayName = "Descending"),

	EAS_HitReaction UMETA(DisplayName = "HitReaction"),
	
	EAS_EquippingWeapon UMETA(DisplayName = "Equipping Weapon"),

	EAS_Interacting UMETA(DisplayName = "Interacting"),
	
	EAS_ComboActionAvailable UMETA(DisplayName = "ComboActionAvailable"),
	EAS_AirborneComboActionAvailable UMETA(DisplayName = "AirborneComboActionAvailable"),
	
	
	EAS_AirAttacking UMETA(DisplayName = "AirAttacking"),
	EAS_AirUnoccupied UMETA(DisplayName = "AirUnoccupied"),
	EAS_Attacking UMETA(DisplayName = "Attacking"),
	
	EAS_Dead UMETA(DisplayName = "Dead")
};

inline std::map<EActionState, FName> ActionStateFName = {
	{EActionState::EAS_Unoccupied, FName("Unoccupied")},
	{EActionState::EAS_GroundLightAttacking1, FName("GroundLightAttacking1")},
	{EActionState::EAS_GroundLightAttacking2, FName("GroundLightAttacking2")},
	{EActionState::EAS_GroundLightAttacking3, FName("GroundLightAttacking3")},
	{EActionState::EAS_GroundLightAttacking4, FName("GroundLightAttacking4")},
	{EActionState::EAS_GroundHeavyAttacking1, FName("GroundHeavyAttacking1")},
	{EActionState::EAS_GroundHeavyAttacking2, FName("GroundHeavyAttacking2")},
	{EActionState::EAS_GroundHeavyAttacking3, FName("GroundHeavyAttacking3")},
	{EActionState::EAS_GroundHeavyAttacking4, FName("GroundHeavyAttacking4")},
	{EActionState::EAS_AirLightAttacking1, FName("AirLightAttacking1")},
	{EActionState::EAS_AirLightAttacking2, FName("AirLightAttacking2")},
	{EActionState::EAS_AirLightAttacking3, FName("AirLightAttacking3")},
	{EActionState::EAS_AirLightAttacking4, FName("AirLightAttacking4")},
	{EActionState::EAS_AirHeavyAttacking1, FName("AirHeavyAttacking1")},
	{EActionState::EAS_AirHeavyAttacking2, FName("AirHeavyAttacking2")},
	{EActionState::EAS_AirHeavyAttacking3, FName("AirHeavyAttacking3")},
	{EActionState::EAS_AirHeavyAttacking4, FName("AirHeavyAttacking4")},
	{EActionState::EAS_GroundSpecialAttacking, FName("GroundSpecialAttacking")},
	{EActionState::EAS_AirSpecialAttacking, FName("AirSpecialAttacking")},
	{EActionState::EAS_Parry, FName("Parry")},
	{EActionState::EAS_Blocking, FName("Blocking")},
	{EActionState::EAS_Falling, FName("Falling")},
	{EActionState::EAS_Jumping, FName("Jumping")},
	{EActionState::EAS_Rolling, FName("Rolling")},
	{EActionState::EAS_RollingFinal, FName("RollingFinal")},
	{EActionState::EAS_Dashing, FName("Dashing")},
	{EActionState::EAS_DashingFinal, FName("DashingFinal")},
	{EActionState::EAS_DoubleJumping, FName("DoubleJumping")},
	{EActionState::EAS_Ascended, FName("Ascended")},
	{EActionState::EAS_Descending, FName("Descending")},
	{EActionState::EAS_HitReaction, FName("HitReaction")},
	{EActionState::EAS_EquippingWeapon, FName("Equipping Weapon")},
	{EActionState::EAS_Interacting, FName("Interacting")},
	{EActionState::EAS_ComboActionAvailable, FName("ComboActionAvailable")},
	{EActionState::EAS_AirborneComboActionAvailable, FName("AirborneComboActionAvailable")},
	{EActionState::EAS_AirAttacking, FName("AirAttacking")},
	{EActionState::EAS_AirUnoccupied, FName("AirUnoccupied")},
	{EActionState::EAS_Attacking, FName("Attacking")},
	{EActionState::EAS_Dead, FName("Dead")}
	// Add other states as needed
};

UENUM(BlueprintType)
enum class EAscendState : uint8
{
	EASCS_Ascended UMETA(DisplayName = "Ascended"),
	EASCS_Descended UMETA(DisplayName = "Descended"),
};

UENUM(BlueprintType)
enum class ESwingState : uint8
{
	ESS_FirstSwing UMETA(DisplayName = "FirstSwing"),
	ESS_SecondSwing UMETA(DisplayName = "SecondSwing"),
	ESS_ThirdSwing UMETA(DisplayName = "ThirdSwing"),
	ESS_FourthSwing UMETA(DisplayName = "FourthSwing"),
	ESS_FinalSwing UMETA(DisplayName = "FinalSwing"),

};

UENUM(BlueprintType)
enum class EAttackState : uint8
{
	EATS_LightAttack UMETA(DisplayName = "LightAttack"),
	EATS_HeavyAttack UMETA(DisplayName = "HeavyAttack"),
	EATS_SpecialAttack UMETA(DisplayName = "SpecialAttack"),
	EATS_JumpAttack UMETA(DisplayName = "JumpAttack"),
};

UENUM(BlueprintType)
enum EDeathPose
{
	EDP_Death1 UMETA(DisplayName = "Death1"),
	EDP_Death2 UMETA(DisplayName = "Death2"),
	EDP_Death3 UMETA(DisplayName = "Death3"),
	EDP_Death4 UMETA(DisplayName = "Death4"),
	EDP_Death5 UMETA(DisplayName = "Death5"),
	EDP_Death6 UMETA(DisplayName = "Death6"),
	EDP_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class ERollState : uint8
{
	ERS_RollMobile UMETA(DisplayName = "RollMobile"),
	ERS_RollImobile UMETA(DisplayName = "RollImoble"),
};

UENUM(BlueprintType)
enum class EAirborneState : uint8
{
	EAS_AirborneUp UMETA(DisplayName = "AirborneUp"),
	EAS_AirborneHover UMETA(DisplayName = "AirborneHover"),
	EAS_AirborneDown UMETA(DisplayName = "AirborneDown"),
};

UENUM(BlueprintType)
enum class ETurnState : uint8
{
	ETS_LeftToRight UMETA(DisplayName = "LeftToRight"),
	ETS_RightToLeft UMETA(DisplayName = "RightToLeft"),
	ETS_FrontToBack UMETA(DisplayName = "FrontToBack"),
	ETS_BackToFront UMETA(DisplayName = "BackToFront"),
	ETS_NotTurning UMETA(DisplayName = "NotTurning"),
};

UENUM(BlueprintType)
enum class EStepState : uint8
{
	ESS_SteppingForward UMETA(DisplayName = "SteppingForward"),
	ESS_SteppingBackward UMETA(DisplayName = "SteppingBackward"),
	ESS_NotStepping UMETA(DisplayName = "NotStepping"),
};

UENUM(BlueprintType)
enum class ETargetState : uint8
{
	ETS_TargetLocked UMETA(DisplayName = "TargetLocked"),
	ETS_NoTarget UMETA(DisplayName = "NoTarget"),
};

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_NoState UMETA(DisplayName = "NoState"),
	EES_Dead UMETA(DisplayName = "Dead"),
	EES_Patrolling UMETA(DisplayName = "Patrolling"),
	EES_Chasing UMETA(DisplayName = "Chasing"),
	EES_Attacking UMETA(DisplayName = "Attacking"),
	EES_Engaged UMETA(DisplayName = "Engaged")
};

struct FDirectionKey
{
	float X_Direction;
	float Y_Direction;

	FDirectionKey(float x, float y) : X_Direction(x), Y_Direction(y) {}

	bool operator==(const FDirectionKey& Other) const
	{
		return X_Direction == Other.X_Direction && Y_Direction == Other.Y_Direction;
	}
};

// Hash function for the FDirectionKey struct
FORCEINLINE uint32 GetTypeHash(const FDirectionKey& Key)
{
	// Hash the float values directly
	return HashCombine(::GetTypeHash(Key.X_Direction), ::GetTypeHash(Key.Y_Direction));
}