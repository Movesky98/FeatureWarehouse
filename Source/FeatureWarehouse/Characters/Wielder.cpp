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

void AWielder::OnRecognizeRangeBeginOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		// [Ÿ�����ϰ� �ִ� ���� != �ݸ����� ������ ����]�� �ƹ��͵� ���� ����.
		if (GetSeeingPawn() && GetSeeingPawn() != OtherActor) return;

		bIsRecognizedSomething = true;

		AWielderController* WielderController = Cast<AWielderController>(GetController());

		// �̹� ��Ʈ�ѷ��� �÷��̾ �ĺ��ϰ� �ν� ������ ������ ���.
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
		// [Ÿ�����ϰ� �ִ� ���� != �ݸ����� ������ ����]�� �ƹ��͵� ���� ����.
		if (GetSeeingPawn() && GetSeeingPawn() != OtherActor)
		{
			AWeaponWielder* WeaponWielder = Cast<AWeaponWielder>(OtherActor);
			if (WeaponWielder)
			{
				RemoveDetectedWielder(WeaponWielder);
			}

			return;
		}

		bIsRecognizedSomething = false;
	}
}

void AWielder::OnDetectionRangeBeginOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		// [Ÿ�����ϰ� �ִ� ���� != �ݸ����� ������ ����]�� �ƹ��͵� ���� ����.
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
		// [Ÿ�����ϰ� �ִ� ���� != �ݸ����� ������ ����]�� �ƹ��͵� ���� ����.
		if (GetSeeingPawn() && GetSeeingPawn() != OtherActor) return;

		bIsEnemyApproached = false;
	}
}

void AWielder::OnAttackRangeBeginOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		// [Ÿ�����ϰ� �ִ� ���� != �ݸ����� ������ ����]�� �ƹ��͵� ���� ����.
		if (GetSeeingPawn() && GetSeeingPawn() != OtherActor) return;

		AWielderController* WielderController = Cast<AWielderController>(GetController());

		if (CurState != EStateOfEnemy::In_Battle)
		{
			ShowStatBar();
		}

		if (GetSeeingPawn() == OtherActor)
		{
			BattleState = EBattleState::Attacking;
			WielderController->NotifyBattleState(BattleState);
			WielderController->NotifyEnemyInAttackRange(true);
		}
	}
}

void AWielder::OnAttackRangeEndOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		// [Ÿ�����ϰ� �ִ� ���� != �ݸ����� ������ ����]�� �ƹ��͵� ���� ����.
		if (GetSeeingPawn() && GetSeeingPawn() != OtherActor) return;

		AWielderController* WielderController = Cast<AWielderController>(GetController());

		// ���� AI�� ���°� ���������� ��.
		if (IsValid(WielderController) && CurState == EStateOfEnemy::In_Battle)
		{
			WielderController->NotifyEnemyInAttackRange(false);
		}
	}
}

void AWielder::AddDetectedWielder(AWeaponWielder* DetectedWielder)
{
	if (DetectedWielder)
	{
		if (CheckEnemyWielder(DetectedWielder))
			DetectedWielders.AddUnique(DetectedWielder);
	}
}

void AWielder::RemoveDetectedWielder(AWeaponWielder* DetectedWielder)
{
	if (DetectedWielder)
	{
		if (DetectedWielders.Find(DetectedWielder))
			DetectedWielders.Remove(DetectedWielder);
	}
}

bool AWielder::CheckEnemyWielder(AWeaponWielder* DetectedWielder)
{
	return Faction != DetectedWielder->GetFaction() ? true : false;
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
	
	// ���� �޾��� ��, ������ ����� Ÿ������ ����.
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

	// ȸ�ǰ� �ʿ��� ��Ȳ�̶��
	if (IsRetreat)
	{
		ChangeToRetreatMode();
		return;
	}

	// ������Ȳ�� �ƴ϶�� �ٷ� ȸ��
	if (CurState == EStateOfEnemy::In_Battle) return;

	ChangeToRetreatMode();
}

void AWielder::ChangeToRetreatMode()
{
	// ���� �޾��� ��, Retreat ���·� ����
	AWielderController* WielderController = Cast<AWielderController>(GetController());
	if (IsValid(WielderController))
	{
		WielderController->NotifyRetreat();
	}
}

void AWielder::RetreatFromEnemy()
{
	// TO DO
	// 1. ���� Ÿ������ �ϰ� �ִ� ���Ϳ��� �Ÿ��� '�ּ� ȸ�� �Ÿ�'�� �Ѵ��� üũ 
	// 2. ���ͷ� ���� �־����� ���� �齺�� �ִϸ��̼� ����

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	// �齺�� ��Ÿ�ְ� ��� ���� �ƴ� �� ���
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
	// ���� ���⸦ �����ϰ� ���� �ʴٸ� ����.
	if (!IsValid(EquipWeapon))
		EquipFirstWeapon();

	// ���⸦ �������̶�� �ƹ��͵� ���� ����.
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

	// ������ �Ÿ��� ����͸� �Ÿ� ���غ��� �����ٸ�
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
		// �ƴ� ��� �÷��̾�� õõ�� ����
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
	if (OnKilled.IsBound())
	{
		OnKilled.Execute(false);
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