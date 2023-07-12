#pragma once

UENUM(BlueprintType)
enum class EMovementState : uint8
{
	EMS_NoMovement UMETA(DisplayName = "No Movement"),
	EMS_RotateForward UMETA(DisplayName = "Rotate Forward"),
	EMS_RotateBackward UMETA(DisplayName = "Rotate Backward"),
	EMS_MoveDown UMETA(DisplayName = "Move Down"),
	EMS_MoveUp UMETA(DisplayName = "Move Up"),
	EMS_MoveForward UMETA(DisplayName = "Move Forward"),
	EMS_MoveBackward UMETA(DisplayName = "Move Backward"),

	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};