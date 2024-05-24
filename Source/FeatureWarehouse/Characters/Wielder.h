// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WielderBase.h"
#include "Wielder.generated.h"

enum class EStateOfEnemy :uint8;
enum class EBattleState :uint8;

class UStatBarComponent;
class ULockOnComponent;

UENUM(BlueprintType)
enum class EPerceptionState : uint8
{
	EPS_NONE = 0 UMETA(DisplayName = "NONE"),
	EPS_InRecognizedRange = 1 UMETA(DisplayName = "InRecognizedRange"),
	EPS_InDetectedRange = 2 UMETA(DisplayName = "InDetectedRange"),
	EPS_InAttackRange = 3 UMETA(DisplayName = "InAttackRange")
};

UENUM(BlueprintType)
enum class ETypeOfWielder : uint8
{
	ETW_NONE = 0 UMETA(DisplayName = "NONE"),
	ETW_Samurai = 1 UMETA(DisplayName = "Samurai"),			// 사무라이
	ETW_Halberdier = 2 UMETA(DisplayName = "Halberdier"),	// 할버드병
	ETW_Warrior = 3 UMETA(DisplayName = "Warrior")			// 워리어 
};

/**
 * 
 */
UCLASS()
class FEATUREWAREHOUSE_API AWielder : public AWielderBase
{
	GENERATED_BODY()
public:
	AWielder();

	void CheckEquipWeapon();

	/* 현재 타겟으로 지정된 액터를 가져오는 함수 */
	UFUNCTION(BlueprintCallable) AActor* GetSeeingPawn();

	/* 타겟을 지정하는 함수 */
	UFUNCTION() void DesignateEnemy(AActor* Enemy);

	/* 플레이어가 AI에게 접근했는지 */
	UFUNCTION(BlueprintCallable) bool IsEnemyApproached();

	/* 무언가를 인식했는지 */
	UFUNCTION(BlueprintCallable) bool IsRecognizedSomething();

	/* AI의 스피드를 설정하는 함수 */
	UFUNCTION(BlueprintCallable) void SetMovementSpeed(float Speed);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State|Speed")
	float WalkSpeed;	// 걸을 때 속도

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State|Speed")
	float SprintSpeed;	// 뛸 때 속도

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State|Speed")
	float CreepSpeed;	// 천천히 걸을 때 속도

	UFUNCTION(BlueprintCallable)
	void ShowStatBar();

	UFUNCTION(BlueprintCallable)
	void HideStatBar();

	void SetVisibleLockOnImage(bool IsVisisble);

	void AddDetectedWielder(AWielderBase* DetectedWielder);

	void RemoveDetectedWielder(AWielderBase* DetectedWielder);

	bool CheckEnemyWielder(AWielderBase* DetectedWielder);

protected:
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	void Die() override;

	UFUNCTION(BlueprintCallable)
	virtual void Attack() override;

	void OnReceivePointDamageEvent(AActor* DamagedActor, float Damage, AController* InstigatedBy,
		FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName,
		FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser) override;

	void OnEquipEnded();

	void OnUnequipEnded();

	UFUNCTION()
	void OnGetDamaged(bool IsRetreat);
	
	void OnHitEnded() override;

	UFUNCTION()
	void ChangeToRetreatMode();

	/* 적으로부터 거리를 벌릴 때 실행되는 함수 */
	UFUNCTION(BlueprintCallable)
	void RetreatFromEnemy();

#pragma region Monitoring
public:
	/* 모니터링을 시작할 때 실행되는 함수 */
	UFUNCTION(BlueprintCallable) void Monitoring();

protected:
	/* 모니터링 하는 시간을 구하는 함수 */
	UFUNCTION() void CalculateMonitoringTime();

	/* 모니터링(경계) 상태일 때 적의 주변을 돌기위해 위치를 구하는 함수 */
	UFUNCTION(BlueprintCallable)
	FVector CirclingAroundTheEnemy();

	/* 모니터링 상태에서 사용되는 변수들을 초기화하는 함수 */
	void ClearMonitoring();

	/** 경계(모니터링) 거리 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monitor")
	float MonitoringDistance;

	/** 경과 시간 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monitor")
	float ElapsedTime;

	/** 모니터링 타이머 시간 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monitor")
	float MonitoringTimerDuration;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monitor")
	bool bIsClockWiseRotation;

	/** 모니터링을 시작했는지 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monitor")
	bool bIsStartMonitoring;

	FTimerHandle MonitoringTimer;
#pragma endregion

	/** 현재 AI의 상태 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "State")
	EStateOfEnemy CurState;

	/** 전투 상태로 들어갔을 때 상태 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "State")
	EBattleState BattleState;

	/** Wielder 타입 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State")
	ETypeOfWielder WielderType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Component")
	UStatBarComponent* StatBarComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Component")
	ULockOnComponent* LockOnComponent;

	////////////////////////////////////////////////////////////////////    Weapon    ////////////////////////////////////////////////////////////////////

	virtual void EquipFirstWeapon() override;

	virtual void Unequip() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<class AWeapon> WeaponBlueprint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool bIsEquipWeapon;

#pragma region Range
protected:
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
	/** Wielder의 인지 상태 **/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Range")
	EPerceptionState PerceptionState;

	/** 인지 범위 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Range")
	float RecognizeRange;

	/** 탐지 범위 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Range")
	float DetectionRange;

	/** 공격 시작 범위 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Range")
	float AttackRange;

	/** 최소 회피 거리 */
	UPROPERTY(EditDefaultsOnly, BLueprintReadOnly, Category = "Range")
	float RetreatDistanceMin;

	/** 무언가를 인지했는지 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Range")
	bool bIsRecognizedSomething;

	/** 적이 접근했는지 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Range")
	bool bIsEnemyApproached;

	/** 감지된 Wielder들 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Range")
	TArray<AWielderBase*> DetectedWielders;

	// Initial Settings
	/** 시작과 동시에 패트롤을 진행할 것인지 여부 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initial Setting")
	bool bStartWithPatrol;

	// Components
	/** 무언가를 인식할 단계 (어떤 액터인지 모르는 단계) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Range")
	class USphereComponent* RecognizeRangeComponent;

	/** 이미 적을 인지하고 전투(Battle)에 들어가는 상태*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Range")
	class USphereComponent* DetectionRangeComponent;

	/** 전투에 들어갔을 때 공격 시작 범위 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Range")
	class USphereComponent* AttackRangeComponent;

public:
	UFUNCTION(BlueprintGetter, Category = "Range|Perception")
	EPerceptionState GetPerceptionState() { return PerceptionState; }

	UFUNCTION(BlueprintSetter, Category = "Range|Perception")
	void SetPerceptionState(EPerceptionState NewPerceptionState) 
	{
		PerceptionState = NewPerceptionState;
	}

#pragma endregion

public:
	// Getter and Setter
	FORCEINLINE EStateOfEnemy GetCurState() { return CurState; }
	FORCEINLINE void SetCurState(EStateOfEnemy State) { CurState = State; }

	FORCEINLINE EBattleState CurBattleState() { return BattleState; }
	FORCEINLINE void SetBattleState(EBattleState State) { BattleState = State; }

	FORCEINLINE ETypeOfWielder GetWielderType() { return WielderType; }
	FORCEINLINE float GetRecognizeRange() { return RecognizeRange; }
	FORCEINLINE float GetAttackRange() { return AttackRange; }

	FORCEINLINE TArray<AWielderBase*> GetDetectedWielders() { return DetectedWielders; }
};
