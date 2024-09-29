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

protected:	
	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	void NotifyToGameMode();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Component")
	UWeaponComponent* WeaponComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Component")
	UStatComponent* StatComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Component")
	class UNiagaraComponent* AfterImageComponent;

#pragma region Montage
public:
	void PlayMontage(UAnimMontage* Montage);

	void TransferReactionMontages(TMap<FString, UAnimMontage*> AnimMontages);

	virtual void OnEquipEnded();

	virtual void OnUnequipEnded();

protected:
	UFUNCTION()
	virtual void OnHitEnded();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Montage|Dodge")
	TMap<EDirection, UAnimMontage*> DodgeMontages;

#pragma endregion

#pragma region Weapon
public:

protected:
	UFUNCTION(BlueprintCallable)
	virtual void EquipFirstWeapon();

	UFUNCTION(BlueprintCallable)
	virtual void EquipSecondWeapon();

	UFUNCTION(BlueprintCallable)
	virtual void Unequip();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	AWeapon* EquipWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool bHasWeapon;
#pragma endregion

#pragma region State
public:
	/* 캐릭터가 죽었음을 알리는 델리게이트 */
	FOnKilledDelegate OnKilled;

	virtual void Die();

	/* 데미지를 받을 수 있는 지 체크 */
	virtual bool CheckDamageable();

	EActionState GetActionState()
	{
		return ActionState;
	}

	void HandleWielderState(EActionState State);

protected:
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;

	/* Receive Point Damage Function */
	UFUNCTION() virtual void OnReceivePointDamageEvent(AActor* DamagedActor, float Damage, AController* InstigatedBy,
		FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName,
		FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser);

	/* 액션을 취할 수 있는지 체크하는 함수. */
	bool CheckTakeAction(EActionState SpecificAction, bool bCanTakeContinuously);

	void SweepWieldersInCriticalVolume();

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

#pragma endregion State


#pragma region Attack
public:
	UFUNCTION(BlueprintCallable)
	bool IsCriticallyHittable();

	UFUNCTION(BlueprintSetter)
	void SetCanCriticalHit(bool CanCriticalHit);

	void ExecuteCriticalHitOnTarget();

	void GetCriticalHit();

protected:
	virtual void Attack();

	virtual void StopAttack();

	virtual void HeavyAttack();
	
	UFUNCTION()
	virtual void CriticalTriggerVolumeBeginOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void CriticalTriggerVolumeEndOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/* 치명타 트리거 볼륨 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Component")
	class UBoxComponent* CriticalTriggerVolume;

	/* 치명타 볼륨 내 Overlap된 Wielders */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack|State")
	TArray<AWielderBase*> OverlappingWieldersInCritiacalVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack|State")
	bool bCanCriticalHit;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack|State")
	AWielderBase* CriticalHitTarget;

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

	FORCEINLINE bool IsDead() { return bIsDead; }
	FORCEINLINE void SetIsDead(bool IsDead) { bIsDead = IsDead; }

	FGenericTeamId GetGenericTeamId() const override;
	FORCEINLINE EFactionType GetFaction() { return Faction; }

	FORCEINLINE AWielderBase* GetKilledByWielder() { return KilledByWielder; }
#pragma endregion
};
