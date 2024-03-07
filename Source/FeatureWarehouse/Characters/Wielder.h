// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponWielder.h"
#include "Wielder.generated.h"

enum class EStateOfEnemy :uint8;

UENUM(BlueprintType)
enum class ETypeOfWielder : uint8
{
	ETW_NONE = 0 UMETA(DisplayName = "NONE"),
	ETW_Samurai = 1 UMETA(DisplayName = "Samurai"),			// �繫����
	ETW_Halberdier = 2 UMETA(DisplayName = "Halberdier")	// �ҹ��� ��
};

/**
 * 
 */
UCLASS()
class FEATUREWAREHOUSE_API AWielder : public AWeaponWielder
{
	GENERATED_BODY()
public:
	AWielder();

	/* �÷��̾ AI���� �����ߴ��� */
	UFUNCTION(BlueprintCallable) bool IsPlayerApproached();

	/* AI�� Combat�� ���� �������� �� �����ϴ� �Լ� */
	virtual void EngagingInCombat(AActor* AdversaryActor);

	/* AI�� ���ǵ带 �����ϴ� �Լ� */
	UFUNCTION(BlueprintCallable) void SetMovementSpeed(float Speed);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State|Speed")
	float WalkSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State|Speed")
	float SprintSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State|Speed")
	float CreepSpeed;

protected:
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "State")
	EStateOfEnemy CurState;			// ���� AI�� ����

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State")
	ETypeOfWielder WielderType;		// Wielder Ÿ��

	////////////////////////////////////////////////////////////////////    Montage    ////////////////////////////////////////////////////////////////////
	
	virtual void EquipEnded() override;
	
	virtual void UnequipEnded() override;

	////////////////////////////////////////////////////////////////////    Weapon    ////////////////////////////////////////////////////////////////////

	virtual void EquipFirstWeapon() override;

	virtual void Unequip() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<class AWeapon> WeaponBlueprint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool bIsEquipWeapon;

	////////////////////////////////////////////////////////////////////    Range    ////////////////////////////////////////////////////////////////////
	// Functions
	UFUNCTION()
	void OnRecognizeRangeBeginOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnRecognizeRangeEndOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnDetectionRangeBeginOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnDetectionRangeEndOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnAttackRangeBeginOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnAttackRangeEndOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Variables
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Range")
	float RecognizeRange;			// �ν� ����

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Range")
	float DetectionRange;			// Ž�� ����

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Range")
	float AttackRange;				// ���� ���� ����

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Range")
	bool bIsRecognizedSomething;	// ���𰡸� �ν��ߴ���

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Range")
	bool bIsPlayerApproached;		// �÷��̾ �����ߴ���

	// Components
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Range")
	class USphereComponent* RecognizeRangeComponent;	// ���𰡸� �ν��� �ܰ� (�÷��̾�����, �ٸ� �������� �𸣴� �ܰ�)

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Range")
	class USphereComponent* DetectionRangeComponent;	// �̹� �÷��̾ �ν��ϰ� Combat�� ���� �ܰ�

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Range")
	class USphereComponent* AttackRangeComponent;		//  Combat ���¿� ���� �� ���� ���� ����

public:
	// Getter and Setter
	FORCEINLINE EStateOfEnemy GetCurState() { return CurState; }
	FORCEINLINE void SetCurState(EStateOfEnemy State) { CurState = State; }

	FORCEINLINE ETypeOfWielder GetWielderType() { return WielderType; }

};
