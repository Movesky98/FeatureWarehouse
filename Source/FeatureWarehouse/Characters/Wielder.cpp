// Fill out your copyright notice in the Description page of Project Settings.


#include "Wielder.h"
#include "AnimInstance/WielderAnimInstance.h"
#include "GamePlay/WielderController.h"

#include "Components/WeaponComponent.h"
#include "Components/StatComponent.h"
#include "Components/StatBarComponent.h"
#include "Components/LockOnComponent.h"

#include "Enums/TypeOfWeapon.h"
#include "Enums/ActionState.h"
#include "Enums/StateOfEnemy.h"
#include "Enums/BattleState.h"

#include "Weapons/Weapon.h"
#include "Melees/Katana.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"

#include "DrawDebugHelpers.h"

AWielder::AWielder()
{
	UncertainDetectionRangeComponent = CreateDefaultSubobject<USphereComponent>(TEXT("UncertainDetectionRange"));
	UncertainDetectionRangeComponent->SetupAttachment(RootComponent);
	UncertainDetectionRangeComponent->SetCollisionProfileName(FName("Trigger"));
	UncertainDetectionRangeComponent->ShapeColor = FColor::Purple;

	DetectionRangeComponent = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionRange"));
	DetectionRangeComponent->SetupAttachment(RootComponent);
	DetectionRangeComponent->SetCollisionProfileName(FName("Trigger"));
	DetectionRangeComponent->ShapeColor = FColor::Green;

	AttackRangeComponent = CreateDefaultSubobject<USphereComponent>(TEXT("AttackRange"));
	AttackRangeComponent->SetupAttachment(RootComponent);
	AttackRangeComponent->SetCollisionProfileName(FName("Trigger"));

	StatBarComponent = CreateDefaultSubobject<UStatBarComponent>(TEXT("StatBarComponent"));
	StatBarComponent->SetupAttachment(RootComponent);

	LockOnComponent = CreateDefaultSubobject<ULockOnComponent>(TEXT("LockOnComponent"));
	LockOnComponent->SetupAttachment(RootComponent);

	GetMesh()->SetCollisionProfileName(FName("Enemy"));

	AIControllerClass = AWielderController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bIsClockWiseRotation = true;
	TargetRetainTime = 2.0f;
}

void AWielder::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UncertainDetectionRangeComponent->SetSphereRadius(UncertainDetectionRange);
	DetectionRangeComponent->SetSphereRadius(DetectionRange);
	AttackRangeComponent->SetSphereRadius(AttackRange);

	UncertainDetectionRangeComponent->OnComponentBeginOverlap.AddDynamic(this, &AWielder::OnUncertainDetectionRangeBeginOverlap);
	UncertainDetectionRangeComponent->OnComponentEndOverlap.AddDynamic(this, &AWielder::OnUncertainDetectionRangeEndOverlap);

	DetectionRangeComponent->OnComponentBeginOverlap.AddDynamic(this, &AWielder::OnDetectionRangeBeginOverlap);
	DetectionRangeComponent->OnComponentEndOverlap.AddDynamic(this, &AWielder::OnDetectionRangeEndOverlap);

	AttackRangeComponent->OnComponentBeginOverlap.AddDynamic(this, &AWielder::OnAttackRangeBeginOverlap);
	AttackRangeComponent->OnComponentEndOverlap.AddDynamic(this, &AWielder::OnAttackRangeEndOverlap);

	StatBarComponent->Init();

	LockOnComponent->Init();

	TeamId = FGenericTeamId((uint8)Faction);

	if (bStartWithPatrol)
	{
		// Set Patrol.
		AWielderController* WielderController = Cast<AWielderController>(GetController());
		ensureMsgf(WielderController, TEXT("%s's Controller is not wielder controller class !!"), *UKismetSystemLibrary::GetDisplayName(this));

		WielderController->HandleEnemyState(EStateOfEnemy::Patrol);
	}
	else
	{
		HandleWielderState(EActionState::EAS_Idle);
	}

	UWielderAnimInstance* WielderAnim = Cast<UWielderAnimInstance>(GetMesh()->GetAnimInstance());
	if (WielderAnim)
	{
		WielderAnim->OnHitEnd.BindUObject(this, &AWielder::OnHitEnded);
	}
}

void AWielder::BeginPlay()
{
	Super::BeginPlay();

	if (bIsEquipWeapon) return;
	
	if (IsValid(WeaponBlueprint))
	{
		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			FRotator Rotator;
			FVector SpawnLocation = GetActorLocation();

			AWeapon* SpawnWeapon = World->SpawnActor<AWeapon>(WeaponBlueprint, SpawnLocation, Rotator, SpawnParams);
			SpawnWeapon->Interact(this);

			bIsEquipWeapon = true;
		}
	}

	HideStatBar();
	NotifyToGameMode();
}

AActor* AWielder::GetSeeingPawn()
{
	AWielderController* WielderController = Cast<AWielderController>(GetController());

	if (IsValid(WielderController))
	{
		return WielderController->GetSeeingPawn();
	}

	return nullptr;
}

void AWielder::DesignateEnemy(AActor* Enemy)
{
	AWielderController* WielderController = Cast<AWielderController>(GetController());

	if (IsValid(WielderController))
	{
		WielderController->DesignateEnemy(Enemy);
	}
}

#pragma region Detection
void AWielder::OnUncertainDetectionRangeBeginOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this) return;

	// WielderBase 클래스인지 체크.
	AWielderBase* WielderBase = Cast<AWielderBase>(OtherActor);
	if (!IsValid(WielderBase)) return;

	UpdateDetectionRange(WielderBase, EPerceptionState::EPS_InRecognizedRange);

	// 타겟 접근이 아닌 경우
	if (!GetSeeingPawn())
	{
		AWielderController* WielderController = Cast<AWielderController>(GetController());

		// 컨트롤러가 존재하고, 인식되어 Wielder의 감지리스트 내에 있는 경우.
		if (IsValid(WielderController) && PerceivedWielders.Contains(WielderBase))
		{
			// Wielder의 상태를 Check로 변경하고 위치로 이동.
			WielderController->HandleEnemyState(EStateOfEnemy::Check, WielderBase->GetActorLocation());
		}
		return;
	}

	// 타겟 접근인 경우
	if (GetSeeingPawn() == OtherActor)
	{
		CancelClearTarget();
		return;
	}
}

void AWielder::OnUncertainDetectionRangeEndOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// WielderBase 클래스인지 체크.
	AWielderBase* WielderBase = Cast<AWielderBase>(OtherActor);
	if (!IsValid(WielderBase)) return;

	// TO DO
	// 1. Remove WielderBase from InRangeWielders.
	// 2. Remove WielderBase from PerceivedWielders.
	// 3. if WielderBase is target, reserve the ClearTarget function.

	RemoveInRangeWielders(WielderBase);

	if (PerceivedWielders.Contains(WielderBase))
	{
		RemovePerceivedWielders(WielderBase);
	}

	// Range를 벗어난 Actor가 타겟인 경우
	if (GetSeeingPawn() && GetSeeingPawn() == OtherActor)
	{
		// ClearTarget 예약.
		ReserveClearTarget(TargetRetainTime);
		return;
	}
}

void AWielder::OnDetectionRangeBeginOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this) return;

	AWielderBase* WielderBase = Cast<AWielderBase>(OtherActor);
	if (!IsValid(WielderBase)) return;

	UpdateDetectionRange(WielderBase, EPerceptionState::EPS_InDetectedRange);

	// [타게팅하고 있는 액터 != 콜리전에 접근한 액터]면 아무것도 하지 않음.
	if (GetSeeingPawn() && GetSeeingPawn() != OtherActor) return;

	AWielderController* WielderController = Cast<AWielderController>(GetController());
	if (!IsValid(WielderController)) return;

	// 이미 식별된 Wielder라면
	if (PerceivedWielders.Contains(WielderBase))
	{
		WielderController->HandleEnemyState(EStateOfEnemy::In_Battle, FVector(0.0f), WielderBase);
		ShowStatBar();

		return;
	}
}

void AWielder::OnDetectionRangeEndOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AWielderBase* WielderBase = Cast<AWielderBase>(OtherActor);
	if (!WielderBase) return;

	UpdateDetectionRange(WielderBase, EPerceptionState::EPS_InRecognizedRange);
}

void AWielder::OnAttackRangeBeginOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this) return;

	AWielderBase* WielderBase = Cast<AWielderBase>(OtherActor);
	if (!WielderBase) return;

	UpdateDetectionRange(WielderBase, EPerceptionState::EPS_InAttackRange);

	// [타게팅하고 있는 액터 != 콜리전에 접근한 액터]면 아무것도 하지 않음.
	if (GetSeeingPawn() && GetSeeingPawn() != OtherActor) return;

	AWielderController* WielderController = Cast<AWielderController>(GetController());
	if (!IsValid(WielderController)) return;

	WielderController->HandleBattleState(EBattleState::Attacking, true);
}

void AWielder::OnAttackRangeEndOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AWielderBase* WielderBase = Cast<AWielderBase>(OtherActor);
	if (!WielderBase) return;

	UpdateDetectionRange(WielderBase, EPerceptionState::EPS_InDetectedRange);

	// [타게팅하고 있는 액터 != 콜리전에 접근한 액터]면 아무것도 하지 않음.
	if (GetSeeingPawn() && GetSeeingPawn() != OtherActor) return;

	AWielderController* WielderController = Cast<AWielderController>(GetController());
	if (!IsValid(WielderController)) return;

	// 현재 AI의 상태가 전투상태일 때.
	if (CurState == EStateOfEnemy::In_Battle && BattleState == EBattleState::Attacking)
	{
		WielderController->HandleBattleState(EBattleState::Attacking, false);
	}
}

bool AWielder::FindPerceivedWielder(AWielderBase* Wielder)
{
	return PerceivedWielders.Contains(Wielder);
}

void AWielder::UpdateDetectionRange(AWielderBase* WielderBase, EPerceptionState NewState)
{
	if (WielderBase && !WielderBase->IsDead())
	{
		BindToDeathEvent(WielderBase);
		InRangeWielders.Add(WielderBase, NewState);
	}
}

void AWielder::RemoveInRangeWielders(AWielderBase* WielderBase)
{
	if (WielderBase)
	{
		UnbindFromDeathEvent(WielderBase);
		InRangeWielders.Remove(WielderBase);
	}
}

void AWielder::UpdatePerceivedWielders(AWielderBase* PerceivedWielder)
{
	if (PerceivedWielder && !PerceivedWielder->IsDead())
	{
		PerceivedWielders.Add(PerceivedWielder);
	}
}

void AWielder::RemovePerceivedWielders(AWielderBase* PerceivedWielder)
{
	if (PerceivedWielder)
	{
		PerceivedWielders.Remove(PerceivedWielder);
	}
}

void AWielder::BindToDeathEvent(AWielderBase* WielderBase)
{
	if (!InRangeHandles.Contains(WielderBase))
	{
		FDelegateHandle DelegateHandle = WielderBase->OnKilled.AddUObject(this, &AWielder::ManageWielderDeathInRange);
		InRangeHandles.Add(WielderBase, DelegateHandle);
	}
}

void AWielder::UnbindFromDeathEvent(AWielderBase* WielderBase)
{
	if (FDelegateHandle* Handle = InRangeHandles.Find(WielderBase))
	{
		WielderBase->OnKilled.Remove(*Handle);
		InRangeHandles.Remove(WielderBase);
	}
}

void AWielder::ManageWielderDeathInRange(AActor* Actor)
{
	AWielderBase* WielderBase = Cast<AWielderBase>(Actor);
	if (!WielderBase) return;

	RemoveInRangeWielders(WielderBase);
	RemovePerceivedWielders(WielderBase);

	if (BattleState == EBattleState::None) return;

	UE_LOG(LogTemp, Warning, TEXT("Wielder || %s's Target is %s, and dead actor is %s"),
		*UKismetSystemLibrary::GetDisplayName(this),
		*UKismetSystemLibrary::GetDisplayName(GetSeeingPawn()),
		*UKismetSystemLibrary::GetDisplayName(Actor));

	if (!GetSeeingPawn() || GetSeeingPawn() == Actor)
		RequestFindNewTarget();
}

void AWielder::RequestFindNewTarget()
{
	AWielderController* WielderController = Cast<AWielderController>(GetController());
	if (!WielderController) return;

	WielderController->FindNewTarget();
}

void AWielder::ReserveClearTarget(float Time)
{
	if (Time >= 0)
	{
		GetWorldTimerManager().SetTimer(ClearTargetHandle, this, &AWielder::ClearTarget, Time);
	}
	else
	{
		UE_LOG(LogStats, Error, TEXT("AWielder || Please change the clear target time. it should not be a negative number."));
	}
}

void AWielder::CancelClearTarget()
{
	GetWorldTimerManager().ClearTimer(ClearTargetHandle);
}

void AWielder::ClearTarget()
{
	AWielderController* WielderController = Cast<AWielderController>(GetController());
	if (WielderController)
	{
		UE_LOG(LogTemp, Warning, TEXT("Wielder || ClearTarget!"));
		WielderController->ClearTarget();
	}
}

#pragma endregion

bool AWielder::CheckEnemyWielder(AWielderBase* DetectedWielder)
{
	return Faction != DetectedWielder->GetFaction() ? true : false;
}

void AWielder::OnReceivePointDamageEvent(AActor* DamagedActor, float Damage, AController* InstigatedBy,
	FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName,
	FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser)
{	
	if (bIsDead) return;

	AWielderBase* WielderBase = Cast<AWielderBase>(InstigatedBy->GetPawn());
	if (WielderBase && WielderBase->GetGenericTeamId() == TeamId) return;

	UWielderAnimInstance* WielderAnim = Cast<UWielderAnimInstance>(GetMesh()->GetAnimInstance());
	if (!WielderAnim) return;

	ActionState = EActionState::EAS_GetDamaged;

	StatComponent->DecreaseHP(Damage);

	bool IsDead = StatComponent->CheckDeathStatus();

	if (IsDead)
	{
		bIsDead = true;
		KilledByWielder = Cast<AWielderBase>(InstigatedBy->GetPawn());

		WielderAnim->PlayReactionMontage(EMontageType::EMT_Death);
		return;
	}
	else
	{
		WielderAnim->PlayReactionMontage(EMontageType::EMT_GetHit);

		FRotator ImpactRotation = UKismetMathLibrary::MakeRotFromZ(ShotFromDirection);
		StatComponent->ShowBloodEffect(HitLocation, ImpactRotation);
	}

	if (!StatBarComponent->IsShowing())
	{
		StatBarComponent->ShowUI();
	}
	
	// 공격 받았을 때, 공격한 대상을 타겟으로 지정.
	AWielderController* WielderController = Cast<AWielderController>(GetController());
	if (IsValid(WielderController))
	{
		WielderController->DesignateEnemy(InstigatedBy->GetPawn());
		WielderController->StopMovement();
		WielderController->NotifyUnderAttack(true);
	}
}

void AWielder::OnGetDamaged(bool IsRetreat)
{
	AWielderController* WielderController = Cast<AWielderController>(GetController());
	if (IsValid(WielderController))
	{
		WielderController->NotifyUnderAttack(false);
	}

	// 회피가 필요한 상황이라면
	if (IsRetreat)
	{
		ChangeToRetreatMode();
		return;
	}

	// 전투상황이 아니라면 바로 회피
	if (CurState == EStateOfEnemy::In_Battle) return;

	ChangeToRetreatMode();
}

void AWielder::OnHitEnded()
{
	HandleWielderState(EActionState::EAS_Idle);

	AWielderController* WielderController = Cast<AWielderController>(GetController());
	if (IsValid(WielderController))
	{
		WielderController->NotifyUnderAttack(false);
	}

	// 회피가 필요한 상황이라면
	bool ShouldRetreat = StatComponent->CheckShouldEvade();
	if (ShouldRetreat)
	{
		ChangeToRetreatMode();
		return;
	}

	// 전투상황이 아니라면 바로 회피
	if (CurState == EStateOfEnemy::In_Battle) return;

	ChangeToRetreatMode();
}

void AWielder::ChangeToRetreatMode()
{
	// 공격 받았을 때, Retreat 상태로 변경
	AWielderController* WielderController = Cast<AWielderController>(GetController());
	if (IsValid(WielderController))
	{
		WielderController->HandleBattleState(EBattleState::Retreating);
	}
}

void AWielder::RetreatFromEnemy()
{
	// TO DO
	// 1. 현재 타겟으로 하고 있는 액터와의 거리가 '최소 회피 거리'를 넘는지 체크 
	// 2. 액터로 부터 멀어지기 위해 백스텝 애니메이션 실행

	UWielderAnimInstance* WielderAnim = Cast<UWielderAnimInstance>(GetMesh()->GetAnimInstance());
	if (!WielderAnim) return;

	if (!IsValid(GetSeeingPawn())) return;

	UWorld* World = GetWorld();
	if (World)
	{
		FVector Start = GetActorLocation();
		FVector End = GetActorLocation() - 100.0f * GetActorForwardVector();
		float Radius = GetCapsuleComponent()->GetScaledCapsuleRadius();
		float Height = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		ETraceTypeQuery TraceType = ETraceTypeQuery::TraceTypeQuery1;
		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(this);

		FHitResult Result;

		bool IsHit = UKismetSystemLibrary::CapsuleTraceSingle(
			World,
			Start,
			End,
			Radius,
			Height,
			TraceType,
			true,
			IgnoreActors,
			EDrawDebugTrace::None,
			Result,
			true
		);

		if (IsHit)
		{
			FString HitActorName = UKismetSystemLibrary::GetDisplayName(Result.GetActor());

			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, HitActorName);

			// Change BattleState to Approaching or monitoring
			bool IsApproach = FMath::RandBool();

			if (IsApproach)
			{
				AWielderController* WielderController = Cast<AWielderController>(GetController());
				if (IsValid(WielderController))
				{
					WielderController->NotifyApproachToEnemy(GetSeeingPawn());
				}
			}
			else
			{
				Monitoring();
			}
		}
		else
		{
			// 타겟이 회피 최소 거리보다 가까이 있을 경우 실행
			if (GetDistanceTo(GetSeeingPawn()) <= RetreatDistanceMin)
			{
				UE_LOG(LogTemp, Warning, TEXT("Wielder || Target is %s, and The Distance with Target is %f / Retreat Distance Min is %f"), *UKismetSystemLibrary::GetDisplayName(GetSeeingPawn()), GetDistanceTo(GetSeeingPawn()), RetreatDistanceMin);
				
				WielderAnim->PlayReactionMontage(EMontageType::EMT_Retreat);
			}
			else
			{
				// Change BattleState to Approaching or monitoring
				bool IsApproach = FMath::RandBool();

				if (IsApproach)
				{
					AWielderController* WielderController = Cast<AWielderController>(GetController());
					if (IsValid(WielderController))
					{
						WielderController->NotifyApproachToEnemy(GetSeeingPawn());
					}
				}
				else
				{
					Monitoring();
				}
			}
		}
	}
}

void AWielder::Attack()
{
	if (bIsDead || !IsValid(EquipWeapon)) return;

	EActionState SpecificAction = EActionState::EAS_Attacking;
	bool CanTakeAction = CheckTakeAction(SpecificAction, true);

	EquipWeapon->Attack();
}

void AWielder::OnEquipEnded()
{
	HandleWielderState(EActionState::EAS_Idle);

	AWielderController* WielderController = Cast<AWielderController>(GetController());
	if (IsValid(WielderController))
	{
		WielderController->NotifyEquipWeapon();
	}

	UWielderAnimInstance* WielderAnim = Cast<UWielderAnimInstance>(GetMesh()->GetAnimInstance());
	if (IsValid(WielderAnim))
	{
		WielderAnim->SetEquipWeapon(true);
	}
}

void AWielder::OnUnequipEnded()
{
	HandleWielderState(EActionState::EAS_Idle);

	AWielderController* WielderController = Cast<AWielderController>(GetController());
	if (IsValid(WielderController))
	{
		WielderController->NotifyUnequip();
	}

	UWielderAnimInstance* WielderAnim = Cast<UWielderAnimInstance>(GetMesh()->GetAnimInstance());
	if (IsValid(WielderAnim))
	{
		WielderAnim->SetEquipWeapon(false);
	}
}

void AWielder::CheckEquipWeapon()
{
	// 현재 무기를 장착하고 있지 않다면 착용.
	if (!IsValid(EquipWeapon))
		EquipFirstWeapon();

	// 무기를 착용중이라면 아무것도 하지 않음.
}

void AWielder::EquipFirstWeapon()
{
	if (bIsDead) return;

	if (ActionState == EActionState::EAS_Swapping) return;
	if (!WeaponComponent->GetMainWeapon() || EquipWeapon == WeaponComponent->GetMainWeapon()) return;

	EActionState SpecificAction = EActionState::EAS_Equipping;

	bool CanTakeAction = CheckTakeAction(SpecificAction, false);
	if (!CanTakeAction) return;

	if (WeaponComponent->CurEquipState() == EEquipState::SubWeapon)
	{
		if (EquipWeapon->GetWeaponType() == ETypeOfWeapon::Gun)
		{
			EquipWeapon->Unequip();
		}

		if (EquipWeapon->HasEquipMontage())
		{
			WeaponComponent->Swap();
			return;
		}
	}

	WeaponComponent->EquipMainWeapon();
}

void AWielder::Unequip()
{
	if (ActionState != EActionState::EAS_Idle) return;
	
	WeaponComponent->Unequip();
}

#pragma region Monitor
void AWielder::Monitoring()
{
	bIsStartMonitoring = true;
	bIsClockWiseRotation = FMath::RandBool();

	AWielderController* WielderController = Cast<AWielderController>(GetController());
	if (IsValid(WielderController))
	{
		WielderController->HandleBattleState(EBattleState::Monitoring);
		WielderController->SetFocus(WielderController->GetSeeingPawn());
	}

	MonitoringTimerDuration = 5.0f + UKismetMathLibrary::RandomFloatInRange(0.0f, 3.0f);
	GetWorldTimerManager().SetTimer(MonitoringTimer, this, &AWielder::CalculateMonitoringTime, 0.01f, true);
}

void AWielder::CalculateMonitoringTime()
{
	if (MonitoringTimerDuration < ElapsedTime)
	{
		ClearMonitoring();

		// Change Battle State to Approaching.
		AWielderController* WielderController = Cast<AWielderController>(GetController());
		if (WielderController)
		{
			WielderController->ClearFocus(EAIFocusPriority::LastFocusPriority);
			WielderController->HandleBattleState(EBattleState::Approaching);
		}
	}
	else
	{
		ElapsedTime += 0.01f;
	}
}

FVector AWielder::CirclingAroundTheEnemy()
{
	FVector MoveLocation;

	AActor* Enemy = GetSeeingPawn();
	if (!IsValid(Enemy)) return GetActorLocation();

	// 시계 방향으로 돌건지
	FVector GoalDirection = bIsClockWiseRotation ? GetActorRightVector() : -GetActorRightVector();

	// 적과의 거리가 모니터링 거리 기준보다 가깝다면
	if (GetDistanceTo(Enemy) < MonitoringDistance)
	{
		MoveLocation = GetActorLocation() + GoalDirection * 100.0f;

		UWorld* World = GetWorld();
		if (!ensure(World != nullptr)) return GetActorLocation();

		TArray<AActor*> IgnoreActors;
		FHitResult Hit;

		bool IsSuccess = UKismetSystemLibrary::LineTraceSingle(
			GetWorld(),
			MoveLocation,
			MoveLocation - FVector(0.0f, 0.0f, 1000.0f),
			ETraceTypeQuery::TraceTypeQuery1,
			false,
			IgnoreActors,
			EDrawDebugTrace::None,
			Hit,
			true
		);

		if (IsSuccess)
		{
			return Hit.ImpactPoint;
		}

		return MoveLocation;
	}
	else
	{
		// 아닐 경우 플레이어에게 천천히 접근
		MoveLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;

		/*DrawDebugLine(GetWorld(), GetActorLocation(), MoveLocation, FColor::Green, true, -1, 0, 3);
		DrawDebugSolidBox(GetWorld(), MoveLocation, FVector(5.0f), FColor::Purple);*/

		UWorld* World = GetWorld();
		if (!ensure(World != nullptr)) return GetActorLocation();

		TArray<AActor*> IgnoreActors;
		FHitResult Hit;

		bool IsSuccess = UKismetSystemLibrary::LineTraceSingle(
			GetWorld(),
			MoveLocation,
			MoveLocation - FVector(0.0f, 0.0f, 1000.0f),
			ETraceTypeQuery::TraceTypeQuery1,
			false,
			IgnoreActors,
			EDrawDebugTrace::None,
			Hit,
			true
		);

		if (IsSuccess)
		{
			return Hit.ImpactPoint;
		}

		return MoveLocation;
	}
}

void AWielder::ClearMonitoring()
{
	GetWorldTimerManager().ClearTimer(MonitoringTimer);
	ElapsedTime = 0.0f;
	MonitoringTimerDuration = 0.0f;
	bIsStartMonitoring = false;
}

#pragma endregion

void AWielder::SetMovementSpeed(float Speed)
{
	GetCharacterMovement()->MaxWalkSpeed = Speed;
}

void AWielder::Die()
{
	for (auto& InRangeHandle : InRangeHandles)
	{
		InRangeHandle.Key->OnKilled.Remove(InRangeHandle.Value);
	}

	InRangeHandles.Empty();

	if (OnKilled.IsBound())
	{
		OnKilled.Broadcast(this);
	}

	WeaponComponent->RemoveOwnerWeapon();

	AWielderController* WielderController = Cast<AWielderController>(GetController());
	if (!IsValid(WielderController)) return;

	UWielderAnimInstance* Anim = Cast<UWielderAnimInstance>(GetMesh()->GetAnimInstance());
	if (!IsValid(Anim)) return;

	WielderController->NotifyDead();
	Anim->SetIsDead(true);
	StatBarComponent->HideUI();

	GetCapsuleComponent()->SetCollisionProfileName(FName("Ragdoll"));
	GetMesh()->SetCollisionProfileName(FName("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);

	SetLifeSpan(5.0f);
}

void AWielder::ShowStatBar()
{
	if (!StatBarComponent->IsShowing())
	{
		StatBarComponent->ShowUI();
	}
}

void AWielder::HideStatBar()
{
	if (StatBarComponent->IsShowing())
	{
		StatBarComponent->HideUI();
	}
}

void AWielder::SetVisibleLockOnImage(bool IsVisisble)
{
	IsVisisble ? LockOnComponent->ShowUI() : LockOnComponent->HideUI();
}

void AWielder::DrawPerceptionArea()
{
	DrawDebugCircle(GetWorld(), GetActorLocation(), UncertainDetectionRange, 128, FColor::Blue, false, 0.001f, 0, 10.0f, GetActorForwardVector(), GetActorRightVector());
	DrawDebugCircle(GetWorld(), GetActorLocation(), DetectionRange, 128, FColor::Green, false, 0.001f, 0, 10.0f, GetActorForwardVector(), GetActorRightVector());
	DrawDebugCircle(GetWorld(), GetActorLocation(), AttackRange, 128, FColor::Red, false, 0.001f, 0, 10.0f, GetActorForwardVector(), GetActorRightVector());
}