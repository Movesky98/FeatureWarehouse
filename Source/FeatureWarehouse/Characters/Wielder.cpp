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
	RecognizeRangeComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RecognizeRange"));
	RecognizeRangeComponent->SetupAttachment(RootComponent);
	RecognizeRangeComponent->SetCollisionProfileName(FName("Trigger"));
	RecognizeRangeComponent->ShapeColor = FColor::Purple;

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
}

void AWielder::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	RecognizeRangeComponent->SetSphereRadius(RecognizeRange);
	DetectionRangeComponent->SetSphereRadius(DetectionRange);
	AttackRangeComponent->SetSphereRadius(AttackRange);

	RecognizeRangeComponent->OnComponentBeginOverlap.AddDynamic(this, &AWielder::OnRecognizeRangeBeginOverlap);
	RecognizeRangeComponent->OnComponentEndOverlap.AddDynamic(this, &AWielder::OnRecognizeRangeEndOverlap);

	DetectionRangeComponent->OnComponentBeginOverlap.AddDynamic(this, &AWielder::OnDetectionRangeBeginOverlap);
	DetectionRangeComponent->OnComponentEndOverlap.AddDynamic(this, &AWielder::OnDetectionRangeEndOverlap);

	AttackRangeComponent->OnComponentBeginOverlap.AddDynamic(this, &AWielder::OnAttackRangeBeginOverlap);
	AttackRangeComponent->OnComponentEndOverlap.AddDynamic(this, &AWielder::OnAttackRangeEndOverlap);

	StatComponent->OnGetDamaged.BindUFunction(this, FName("OnGetDamaged"));

	StatBarComponent->Init();
	StatBarComponent->HideUI();

	LockOnComponent->Init();

	TeamId = FGenericTeamId((uint8)Faction);

	if (bStartWithPatrol)
	{
		// Set Patrol.
		AWielderController* WielderController = Cast<AWielderController>(GetController());
		ensureMsgf(WielderController, TEXT("%s's Controller is not wielder controller class !!"), *UKismetSystemLibrary::GetDisplayName(this));

		WielderController->NotifyPatrol();
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

void AWielder::OnRecognizeRangeBeginOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		// [타게팅하고 있는 액터 != 콜리전에 접근한 액터]면 아무것도 하지 않음.
		if (GetSeeingPawn() && GetSeeingPawn() != OtherActor) return;

		bIsRecognizedSomething = true;

		AWielderController* WielderController = Cast<AWielderController>(GetController());

		// 이미 컨트롤러가 플레이어를 식별하고 인식 범위에 들어왔을 경우.
		if (IsValid(WielderController) && WielderController->IsIdentifiedEnemy())
		{
			WielderController->NotifyPerceiveSomething(OtherActor->GetActorLocation());
		}
	}
}

void AWielder::OnRecognizeRangeEndOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		// [타게팅하고 있는 액터 != 콜리전에 접근한 액터]면 아무것도 하지 않음.
		if (GetSeeingPawn() && GetSeeingPawn() != OtherActor) return;

		bIsRecognizedSomething = false;
	}
}

void AWielder::OnDetectionRangeBeginOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		// [타게팅하고 있는 액터 != 콜리전에 접근한 액터]면 아무것도 하지 않음.
		if (GetSeeingPawn() && GetSeeingPawn() != OtherActor) return;
		
		bIsEnemyApproached = true;

		AWielderController* WielderController = Cast<AWielderController>(GetController());

		if (IsValid(WielderController) && WielderController->IsIdentifiedEnemy())
		{
			WielderController->NotifyEngageInBattle(OtherActor);
			ShowStatBar();
		}
	}
}

void AWielder::OnDetectionRangeEndOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		// [타게팅하고 있는 액터 != 콜리전에 접근한 액터]면 아무것도 하지 않음.
		if (GetSeeingPawn() && GetSeeingPawn() != OtherActor) return;

		bIsEnemyApproached = false;
	}
}

void AWielder::OnAttackRangeBeginOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		// [타게팅하고 있는 액터 != 콜리전에 접근한 액터]면 아무것도 하지 않음.
		if (GetSeeingPawn() && GetSeeingPawn() != OtherActor) return;

		AWielderController* WielderController = Cast<AWielderController>(GetController());

		if (IsValid(WielderController) && WielderController->IsIdentifiedEnemy())
		{
			BattleState = EBattleState::Attacking;
			WielderController->NotifyBattleState(BattleState);
			WielderController->NotifyEnemyInAttackRange(true);
			ShowStatBar();
		}
	}
}

void AWielder::OnAttackRangeEndOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		// [타게팅하고 있는 액터 != 콜리전에 접근한 액터]면 아무것도 하지 않음.
		if (GetSeeingPawn() && GetSeeingPawn() != OtherActor) return;

		AWielderController* WielderController = Cast<AWielderController>(GetController());

		// 현재 AI의 상태가 전투상태일 때.
		if (IsValid(WielderController) && CurState == EStateOfEnemy::In_Battle)
		{
			WielderController->NotifyEnemyInAttackRange(false);
		}
	}
}

void AWielder::OnReceivePointDamageEvent(AActor* DamagedActor, float Damage, AController* InstigatedBy,
	FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName,
	FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser)
{	
	if (bIsDead) return;

	AWeaponWielder* WeaponWielder = Cast<AWeaponWielder>(InstigatedBy->GetPawn());
	if (WeaponWielder && WeaponWielder->GetGenericTeamId() == TeamId) return;

	ActionState = EActionState::EAS_GetDamaged;

	StatComponent->DecreaseHP(Damage);

	FRotator ImpactRotation = UKismetMathLibrary::MakeRotFromZ(ShotFromDirection);
	StatComponent->ShowBloodEffect(HitLocation, ImpactRotation);

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

void AWielder::ChangeToRetreatMode()
{
	// 공격 받았을 때, Retreat 상태로 변경
	AWielderController* WielderController = Cast<AWielderController>(GetController());
	if (IsValid(WielderController))
	{
		WielderController->NotifyRetreat();
	}
}

void AWielder::RetreatFromEnemy()
{
	// TO DO
	// 1. 현재 타겟으로 하고 있는 액터와의 거리가 '최소 회피 거리'를 넘는지 체크 
	// 2. 액터로 부터 멀어지기 위해 백스텝 애니메이션 실행

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	// 백스텝 몽타주가 재생 중이 아닐 때 재생
	if (AnimInstance->Montage_IsPlaying(StatComponent->GetBackstepMontage())) return;
	if (!IsValid(GetSeeingPawn())) return;

	if (GetDistanceTo(GetSeeingPawn()) <= RetreatDistanceMin)
	{
		PlayMontage(StatComponent->GetBackstepMontage());
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

void AWielder::Attack()
{
	if (bIsDead || !IsValid(EquipWeapon)) return;

	EActionState SpecificAction = EActionState::EAS_Attacking;
	bool CanTakeAction = CheckTakeAction(SpecificAction, true);

	if (CanTakeAction) UE_LOG(LogTemp, Warning, TEXT("%s Attack is called"), *UKismetSystemLibrary::GetDisplayName(this));
	
	EquipWeapon->Attack();
}

void AWielder::OnEquipEnded()
{
	ActionState = EActionState::EAS_Idle;

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
	ActionState = EActionState::EAS_Idle;

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

bool AWielder::IsEnemyApproached()
{
	return bIsEnemyApproached;
}

bool AWielder::IsRecognizedSomething()
{
	return bIsRecognizedSomething;
}

#pragma region Monitor
void AWielder::Monitoring()
{
	bIsStartMonitoring = true;

	AWielderController* WielderController = Cast<AWielderController>(GetController());
	if (IsValid(WielderController))
	{
		WielderController->NotifyMonitoring();
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
			WielderController->NotifyApproaching();
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

	FVector GoalDirection = GetActorRightVector();

	// 적과의 거리가 모니터링 거리 기준보다 가깝다면
	if (GetDistanceTo(Enemy) < MonitoringDistance)
	{
		MoveLocation = GetActorLocation() + GoalDirection * 100.0f;

		DrawDebugLine(GetWorld(), GetActorLocation(), MoveLocation, FColor::Green, true, -1, 0, 3);
		DrawDebugSolidBox(GetWorld(), MoveLocation, FVector(5.0f), FColor::Green);

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
			EDrawDebugTrace::ForDuration,
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

		DrawDebugLine(GetWorld(), GetActorLocation(), MoveLocation, FColor::Green, true, -1, 0, 3);
		DrawDebugSolidBox(GetWorld(), MoveLocation, FVector(5.0f), FColor::Purple);

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
			EDrawDebugTrace::ForDuration,
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
	AWielderController* WielderController = Cast<AWielderController>(GetController());
	if (!IsValid(WielderController)) return;

	WielderController->NotifyDead();
	GetCapsuleComponent()->SetCollisionProfileName(FName("Ragdoll"));
	GetMesh()->SetCollisionProfileName(FName("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);

	SetLifeSpan(3.0f);
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