// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputAction.h"
#include "PlayerCharacter.generated.h"

enum class EStateOfViews :uint8;
class UWeaponComponent;
class UStatComponent;

UENUM(BlueprintType)
enum class EMovementState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Walking UMETA(DisplayName = "Walking"),
	Running UMETA(DisplayName = "Running"),
	Jumping UMETA(DisplayName = "Jumping"),
	Crouching UMETA(DisplayName = "Crouching")
};

UCLASS()
class FEATUREWAREHOUSE_API APlayerCharacter : public ACharacter
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

	void GetDamaged(float Damage);

	UFUNCTION(BlueprintCallable)
	void UpdateHealth();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable)
	AActor* FindInteractableActor(const FVector Start, const FVector End);

	UFUNCTION(BlueprintCallable)
	void EquipFirstWeapon();

	UFUNCTION(BlueprintCallable)
	void EquipSecondWeapon();

	UFUNCTION(BlueprintCallable)
	void Attack();

	UFUNCTION(BlueprintCallable)
	void StopAttack();

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

	FVector DrawCameraLineTrace();
	
private:
	// Variables
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	UWeaponComponent* WeaponComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	UStatComponent* StatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	bool HasWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	class AWeapon* MainWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GamePlay, meta = (AllowPrivateAccess = "true"))
	class AFW_PlayerController* PlayerController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "true"))
	bool bIsZoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "true"))
	float LookYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "true"))
	float LookPitch;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoom|InOut", meta = (AllowPrivateAccess = "true"))
	float MaxArmLength;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zoom|InOut", meta = (AllowPrivateAccess = "true"))
	float MinArmLength;

	FTimerHandle ZoomTimerHandle;

	FTimerHandle AimingTimerHandle;

#pragma region Movement_State
protected:
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Movement", meta = (AllowPrivateAccess = "true"))
	EMovementState MovementState;
#pragma endregion

#pragma region GetterSetter
public:
	FORCEINLINE UWeaponComponent* GetWeaponComponent() { return WeaponComponent; }
	FORCEINLINE UStatComponent* GetStatComponent() { return StatComponent; }

	FORCEINLINE class AWeapon* GetMainWeapon() { return MainWeapon; }
	FORCEINLINE EMovementState GetMovementState() { return MovementState; }
	FORCEINLINE void SetMovementState(EMovementState State) { MovementState = State; }

	FORCEINLINE void SetMainWeapon(class AWeapon* Weapon) { MainWeapon = Weapon; }
#pragma endregion

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
#pragma endregion
};
