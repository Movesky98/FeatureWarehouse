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

	UFUNCTION(BlueprintCallable)
	bool IsPlayerApproached(); /* 플레이어가 AI에게 접근했는지 */

	/* AI가 플레이어를 식별했을 때 실행되는 함수 */
	virtual EStateOfEnemy IdentifiedPlayer();

	/* AI가 Combat에 들어가기 시작했을 때 실행하는 함수 */
	virtual void EngagingInCombat(AActor* AdversaryActor);
protected:

	virtual void PostInitializeComponents() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	EStateOfEnemy CurState;			// 현재 AI의 상태

	/* AI의 스피드를 설정하는 함수 */
	UFUNCTION(BlueprintCallable)
	void SetMovementSpeed(float Speed);

	////////////////////////////////////////////////////////////////////    Range    ////////////////////////////////////////////////////////////////////
	// Functions
	UFUNCTION()
	void OnTriggerRangeBeginOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnTriggerRangesEndOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

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
	float TriggerRange;				// 인식 범위

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
	class USphereComponent* TriggerRangeComponent;		// 무언가를 인식한 단계 (플레이어인지, 다른 무엇인지 모르는 단계)

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Range")
	class USphereComponent* DetectionRangeComponent;	// 이미 플레이어를 인식하고 Combat에 들어가는 단계

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Range")
	class USphereComponent* AttackRangeComponent;		//  Combat 상태에 들어갔을 때 공격 시작 범위
};
