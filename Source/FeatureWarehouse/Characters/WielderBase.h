// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "WielderBase.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnKilledDelegate, AActor*);

enum class EMovementState :uint8;
enum class EActionState :uint8;
enum class EDirection :uint8;
enum class EFactionType :uint8;

class AWeapon;
class UWeaponComponent;
class UStatComponent;

/*
* WeaponComponent를 사용할 수 있는 캐릭터 클래스
*
* 기존에 구현했던 플레이어만 사용하던 WeaponComponent를 AI도 함께 사용하고자 하여 PlayerCharacter로부터 분리하였음.
*/
UCLASS()
class FEATUREWAREHOUSE_API AWielderBase : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	AWielderBase();

	FOnKilledDelegate OnKilled;

	void PlayMontage(UAnimMontage* Montage);

	void TransferReactionMontages(TMap<FString, UAnimMontage*> AnimMontages);

	virtual void OnEquipEnded();

	virtual void OnUnequipEnded();

	virtual void Die();

	/* 데미지를 받을 수 있는 지 체크 */
	virtual bool CheckDamagable();

protected:
	virtual void PostInitializeComponents() override;
	
	virtual void BeginPlay() override;

	// Functions
	virtual void Attack();

	/* Receive Point Damage Function */
	UFUNCTION() virtual void OnReceivePointDamageEvent(AActor* DamagedActor, float Damage, AController* InstigatedBy, 
		FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName,
		FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser);

	UFUNCTION()
	virtual void OnHitEnded();

	UFUNCTION(BlueprintCallable)
	virtual void EquipFirstWeapon();

	UFUNCTION(BlueprintCallable)
	virtual void EquipSecondWeapon();

	UFUNCTION(BlueprintCallable)
	virtual void Unequip();

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;

	/* 액션을 취할 수 있는지 체크하는 함수. */
	bool CheckTakeAction(EActionState SpecificAction, bool bCanTakeContinuously);

	void NotifyToGameMode();

	// Variables
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Component")
	UWeaponComponent* WeaponComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Component")
	UStatComponent* StatComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Component")
	class UNiagaraComponent* AfterImageComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	AWeapon* EquipWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool bHasWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|State")
	EMovementState MovementState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|State")
	EActionState ActionState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "State|Death")
	bool bIsDead;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "State|Death")
	AWielderBase* KilledByWielder;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Team")
	EFactionType Faction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team")
	FGenericTeamId TeamId;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Montage|Dodge")
	TMap<EDirection, UAnimMontage*> DodgeMontages;

#pragma region Attack
public:
	UFUNCTION(BlueprintCallable)
	bool IsCriticallyHittable();

protected:
	virtual void StopAttack();

	virtual void HeavyAttack();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Component")
	class UBoxComponent* CriticalTriggerVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack|State")
	TArray<AWielderBase*> OverlappingWieldersInCritiacalVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack|State")
	bool bCanCriticalHit;
	
	UFUNCTION()
	virtual void CriticalTriggerVolumeBeginOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void CriticalTriggerVolumeEndOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

#pragma endregion Attack

#pragma region GetterSetter
public:
	// Equip Weapon
	FORCEINLINE AWeapon* CurWeapon() { return EquipWeapon; }
	FORCEINLINE void SetWeapon(AWeapon* Weapon) { EquipWeapon = Weapon; }

	// Components
	FORCEINLINE UWeaponComponent* GetWeaponComponent() { return WeaponComponent; }
	FORCEINLINE UStatComponent* GetStatComponent() { return StatComponent; }

	// States
	FORCEINLINE EMovementState CurMovementState() { return MovementState; }
	FORCEINLINE void SetMovementState(EMovementState State) { MovementState = State; }
	FORCEINLINE EActionState CurActionState() { return ActionState; }
	FORCEINLINE void SetActionState(EActionState State) { ActionState = State; }

	FORCEINLINE bool IsDead() { return bIsDead; }
	FORCEINLINE void SetIsDead(bool IsDead) { bIsDead = IsDead; }

	FGenericTeamId GetGenericTeamId() const override;
	FORCEINLINE EFactionType GetFaction() { return Faction; }

	FORCEINLINE AWielderBase* GetKilledByWielder() { return KilledByWielder; }
#pragma endregion
};
