// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/WeaponWielder.h"
#include "InputAction.h"
#include "PlayerCharacter.generated.h"

enum class EStateOfViews :uint8;
enum class EMovementState :uint8;
enum class EActionState :uint8;

class UWeaponComponent;
class UStatComponent;

UCLASS()
class FEATUREWAREHOUSE_API APlayerCharacter : public AWeaponWielder
{
	GENERATED_BODY()
public:
	// Sets default values for this character's properties
	APlayerCharacter();

	void PlayMontage(UAnimMontage* Montage);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetTPP();

	void SetFPP();

	void SetTDP();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;

	void OnEquipEnded() override;

	void OnUnequipEnded() override;

	UFUNCTION()
	void OnDodgeEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION(BlueprintCallable)
	AActor* FindInteractableActor(const FVector Start, const FVector End);

	void EquipFirstWeapon() override;

	void EquipSecondWeapon() override;

	UFUNCTION(BlueprintCallable)
	void Attack() override;

	UFUNCTION(BlueprintCallable)
	void StopAttack() override;

	UFUNCTION()
	void ZoomIn();

	UFUNCTION()
	void ZoomOut();

	UFUNCTION(BlueprintCallable)
	void StartZoom();

	UFUNCTION(BlueprintCallable)
	void StopZoom();

	UFUNCTION()
	void Aiming();

	void HeavyAttack() override;

	void Dodge();

	FVector DrawCameraLineTrace();

#pragma region LockOn
	UFUNCTION()
	void LockTarget();

	void TryLockTarget();

	void FindNearbyLockTarget(float DeltaYaw = 0.0f, float DeltaPitch = 0.0f);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LockOn")
	bool bIsTargetLocked;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LockOn")
	AActor* LockOnTarget;

	FTimerHandle LockOnTimer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LockOn")
	float LockChangeElapsedTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LockOn")
	float LockChangeDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LockOn")
	bool bIsLockChanged;

#pragma endregion
private:
	// Variables
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GamePlay, meta = (AllowPrivateAccess = "true"))
	class AFW_PlayerController* PlayerController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "true"))
	bool bIsZoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "true"))
	float LookYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "true"))
	float LookPitch;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoom", meta = (AllowPrivateAccess = "true"))
	float MaxArmLength;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoom", meta = (AllowPrivateAccess = "true"))
	float MinArmLength;

	FTimerHandle ZoomTimerHandle;

	FTimerHandle AimingTimerHandle;

#pragma region InputAction
protected:
	void MoveForward(const FInputActionValue& Value);

	void MoveRight(const FInputActionValue& Value);

	void Turn(const FInputActionValue& Value);

	void LookUp(const FInputActionValue& Value);

	void JumpTriggered(const FInputActionValue& Value);

	void CrouchTriggered(const FInputActionValue& Value);

	void Run(const FInputActionValue& Value);

	void Interact(const FInputActionValue& Value);

	void FirstWeapon(const FInputActionValue& Value);

	void SecondWeapon(const FInputActionValue& Value);

	void AttackTriggered(const FInputActionValue& Value);

	void Zoom(const FInputActionValue& Value);

	void DodgePressed(const FInputActionValue& Value);

	void LockOn(const FInputActionValue& Value);

	void SetMovementStateIdle();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = InputAction, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveForwardAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = InputAction, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveRightAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = InputAction, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = InputAction, meta = (AllowPrivateAccess = "true"))
	UInputAction* TurnAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = InputAction, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookUpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = InputAction, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwitchViewAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = InputAction, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwitchRunAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = InputAction, meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = InputAction, meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = InputAction, meta = (AllowPrivateAccess = "true"))
	UInputAction* FirstWeaponAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = InputAction, meta = (AllowPrivateAccess = "true"))
	UInputAction* SecondWeaponAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = InputAction, meta = (AllowPrivateAccess = "true"))
	UInputAction* AttackAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = InputAction, meta = (AllowPrivateAccess = "true"))
	UInputAction* ZoomAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = InputAction, meta = (AllowPrivateAccess = "true"))
	UInputAction* DodgeAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = InputAction, meta = (AllowPrivateAccess = "true"))
	UInputAction* LockOnAction;
#pragma endregion
};
