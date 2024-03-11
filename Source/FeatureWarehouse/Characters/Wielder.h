// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponWielder.h"
#include "Wielder.generated.h"

enum class EStateOfEnemy :uint8;
enum class EBattleState :uint8;

UENUM(BlueprintType)
enum class ETypeOfWielder : uint8
{
	ETW_NONE = 0 UMETA(DisplayName = "NONE"),
	ETW_Samurai = 1 UMETA(DisplayName = "Samurai"),			// 사무라이
	ETW_Halberdier = 2 UMETA(DisplayName = "Halberdier")	// 할버드 병
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

	void CheckEquipWeapon();

	/* 현재 타겟으로 지정된 액터를 가져오는 함수 */
	UFUNCTION() AActor* GetSeeingPawn();

	/* 타겟을 지정하는 함수 */
	UFUNCTION() void DesignateEnemy(AActor* Enemy);

	/* 플레이어가 AI에게 접근했는지 */
	UFUNCTION(BlueprintCallable) bool IsPlayerApproached();

	/* 무언가를 인식했는지 */
	UFUNCTION(BlueprintCallable) bool IsRecognizedSomething();

	/* AI가 Combat에 들어가기 시작했을 때 실행하는 함수 */
	virtual void EngagingInCombat(AActor* AdversaryActor);

	/* AI의 스피드를 설정하는 함수 */
	UFUNCTION(BlueprintCallable) void SetMovementSpeed(float Speed);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State|Speed")
	float WalkSpeed;	// 걸을 때 속도

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State|Speed")
	float SprintSpeed;	// 뛸 때 속도

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State|Speed")
	float CreepSpeed;	// 천천히 걸을 때 속도

protected:
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	UFUNCTION(BlueprintCallable)
	virtual void Attack() override;

	UFUNCTION()
	void RetreatFromEnemy();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "State")
	EStateOfEnemy CurState;			// 현재 AI의 상태

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "State")
	EBattleState BattleState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State")
	ETypeOfWielder WielderType;		// Wielder 타입

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
	float RecognizeRange;			// 인식 범위

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Range")
	float DetectionRange;			// 탐지 범위

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Range")
	float AttackRange;				// 공격 시작 범위

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Range")
	bool bIsRecognizedSomething;	// 무언가를 인식했는지

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Range")
	bool bIsPlayerApproached;		// 플레이어가 접근했는지

	// Components
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Range")
	class USphereComponent* RecognizeRangeComponent;	// 무언가를 인식한 단계 (플레이어인지, 다른 무엇인지 모르는 단계)

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Range")
	class USphereComponent* DetectionRangeComponent;	// 이미 플레이어를 인식하고 Combat에 들어가는 단계

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Range")
	class USphereComponent* AttackRangeComponent;		//  Combat 상태에 들어갔을 때 공격 시작 범위

public:
	// Getter and Setter
	FORCEINLINE EStateOfEnemy GetCurState() { return CurState; }
	FORCEINLINE void SetCurState(EStateOfEnemy State) { CurState = State; }

	FORCEINLINE EBattleState CurBattleState() { return BattleState; }
	FORCEINLINE void SetBattleState(EBattleState State) { BattleState = State; }

	FORCEINLINE ETypeOfWielder GetWielderType() { return WielderType; }

};
