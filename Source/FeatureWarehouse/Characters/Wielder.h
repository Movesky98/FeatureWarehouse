// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponWielder.h"
#include "Wielder.generated.h"

enum class EStateOfEnemy :uint8;
enum class EBattleState :uint8;

class UStatBarComponent;
class ULockOnComponent;

UENUM(BlueprintType)
enum class ETypeOfWielder : uint8
{
	ETW_NONE = 0 UMETA(DisplayName = "NONE"),
	ETW_Samurai = 1 UMETA(DisplayName = "Samurai"),			// �繫����
	ETW_Halberdier = 2 UMETA(DisplayName = "Halberdier"),	// �ҹ��� ��
	ETW_Warrior = 3 UMETA(DisplayName = "Warrior")			// ������
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

	/* ���� Ÿ������ ������ ���͸� �������� �Լ� */
	UFUNCTION(BlueprintCallable) AActor* GetSeeingPawn();

	/* Ÿ���� �����ϴ� �Լ� */
	UFUNCTION() void DesignateEnemy(AActor* Enemy);

	/* �÷��̾ AI���� �����ߴ��� */
	UFUNCTION(BlueprintCallable) bool IsEnemyApproached();

	/* ���𰡸� �ν��ߴ��� */
	UFUNCTION(BlueprintCallable) bool IsRecognizedSomething();

	/* AI�� ���ǵ带 �����ϴ� �Լ� */
	UFUNCTION(BlueprintCallable) void SetMovementSpeed(float Speed);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State|Speed")
	float WalkSpeed;	// ���� �� �ӵ�

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State|Speed")
	float SprintSpeed;	// �� �� �ӵ�

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State|Speed")
	float CreepSpeed;	// õõ�� ���� �� �ӵ�

	UFUNCTION(BlueprintCallable)
	void ShowStatBar();

	UFUNCTION(BlueprintCallable)
	void HideStatBar();

	void SetVisibleLockOnImage(bool IsVisisble);

	void AddDetectedWielder(AWeaponWielder* DetectedWielder);

	void RemoveDetectedWielder(AWeaponWielder* DetectedWielder);

	bool CheckEnemyWielder(AWeaponWielder* DetectedWielder);

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
	
	UFUNCTION()
	void ChangeToRetreatMode();

	/* �����κ��� �Ÿ��� ���� �� ����Ǵ� �Լ� */
	UFUNCTION(BlueprintCallable)
	void RetreatFromEnemy();

#pragma region Monitoring
public:
	/* ����͸��� ������ �� ����Ǵ� �Լ� */
	UFUNCTION(BlueprintCallable) void Monitoring();

protected:
	/* ����͸� �ϴ� �ð��� ���ϴ� �Լ� */
	UFUNCTION() void CalculateMonitoringTime();

	/* ����͸�(���) ������ �� ���� �ֺ��� �������� ��ġ�� ���ϴ� �Լ� */
	UFUNCTION(BlueprintCallable)
	FVector CirclingAroundTheEnemy();

	/* ����͸� ���¿��� ���Ǵ� �������� �ʱ�ȭ�ϴ� �Լ� */
	void ClearMonitoring();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monitor")
	float MonitoringDistance;		// ��� �Ÿ�

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monitor")
	float ElapsedTime;				// ��� �ð�

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monitor")
	float MonitoringTimerDuration;	// ����͸� Ÿ�̸� �ð�
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monitor")
	bool bIsStartMonitoring;		// ����͸��� �����ߴ���

	FTimerHandle MonitoringTimer;
#pragma endregion

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "State")
	EStateOfEnemy CurState;			// ���� AI�� ����

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "State")
	EBattleState BattleState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State")
	ETypeOfWielder WielderType;		// Wielder Ÿ��

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

	UPROPERTY(EditDefaultsOnly, BLueprintReadOnly, Category = "Range")
	float RetreatDistanceMin;		// �ּ� ȸ�� �Ÿ�

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Range")
	bool bIsRecognizedSomething;	// ���𰡸� �ν��ߴ���

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Range")
	bool bIsEnemyApproached;		// �÷��̾ �����ߴ���

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Range")
	TArray<AWeaponWielder*> DetectedWielders;	// ������ Wielder��

	// Initial Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initial Setting")
	bool bStartWithPatrol;

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

	FORCEINLINE EBattleState CurBattleState() { return BattleState; }
	FORCEINLINE void SetBattleState(EBattleState State) { BattleState = State; }

	FORCEINLINE ETypeOfWielder GetWielderType() { return WielderType; }
	FORCEINLINE float GetRecognizeRange() { return RecognizeRange; }
	FORCEINLINE float GetAttackRange() { return AttackRange; }

	FORCEINLINE TArray<AWeaponWielder*> GetDetectedWielders() { return DetectedWielders; }
};
