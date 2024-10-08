// Fill out your copyright notice in the Description page of Project Settings.


#include "WielderBase.h"
#include "AnimInstance/WielderAnimInstance.h"
#include "GamePlay/FW_GameMode.h"

#include "DamageTypes/CustomDamageType.h"
#include "DamageTypes/CriticalDamageType.h"

#include "Enums/ActionState.h"
#include "Enums/MovementState.h"
#include "Enums/Direction.h"
#include "Enums/FactionType.h"
#include "Weapon.h"

#include "Components/StatComponent.h"
#include "Components/WeaponComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

AWielderBase::AWielderBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	StatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("StatComponent"));
	WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("WeaponComponent"));

	AfterImageComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("AfterImageComponent"));
	AfterImageComponent->SetupAttachment(RootComponent);

	CriticalTriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("CriticalTriggerVolume"));
	CriticalTriggerVolume->SetupAttachment(RootComponent);
	CriticalTriggerVolume->SetRelativeScale3D(FVector(2.75f, 1.35f, 3.25f));
	CriticalTriggerVolume->SetCollisionProfileName(FName("Trigger"));
	CriticalTriggerVolume->SetActive(true);

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NS_AfterImage(TEXT("/Game/Project/Materials/NS_AfterImage"));
	if (NS_AfterImage.Succeeded())
	{
		AfterImageComponent->SetAsset(NS_AfterImage.Object);
	}

	AfterImageComponent->bAutoActivate = false;
	AfterImageComponent->Deactivate();

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SKM_Manny(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny"));
	if (SKM_Manny.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SKM_Manny.Object);
	}

	MovementState = EMovementState::EMS_Idle;

	GetMesh()->SetGenerateOverlapEvents(true);
}

void AWielderBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	this->OnTakeAnyDamage.AddDynamic(this, &AWielderBase::OnReceiveAnyDamageEvent);
	this->OnTakePointDamage.AddDynamic(this, &AWielderBase::OnReceivePointDamageEvent);

	TeamId = FGenericTeamId((uint8)Faction);

	UWielderAnimInstance* WielderAnim = Cast<UWielderAnimInstance>(GetMesh()->GetAnimInstance());
	if (WielderAnim)
	{
		WielderAnim->OnHitEnd.BindUObject(this, &AWielderBase::OnHitEnded);
		WielderAnim->OnGroggyAttackPointImpact.BindUObject(this, &AWielderBase::ExecuteCriticalHitOnTarget);
	}

	CriticalTriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &AWielderBase::CriticalTriggerVolumeBeginOverlap);
	CriticalTriggerVolume->OnComponentEndOverlap.AddDynamic(this, &AWielderBase::CriticalTriggerVolumeEndOverlap);
}

void AWielderBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWielderBase::HandleWielderState(EActionState State)
{
	ActionState = State;

	switch (ActionState)
	{
	case EActionState::EAS_Idle:
		StatComponent->SetDamageableType(EDamageableType::EDT_CRITICALLY_HITTABLE);
		SweepWieldersInCriticalVolume();
		CriticalTriggerVolume->SetActive(true);
		break;
	default:
		StatComponent->SetDamageableType(EDamageableType::EDT_NORMAL);
		CriticalTriggerVolume->SetActive(false);
		break;
	}
}

void AWielderBase::SweepWieldersInCriticalVolume()
{
	UWorld* World = GetWorld();
	if (World)
	{
		TArray<FHitResult> Hits;
		FVector Start = GetActorLocation();
		FVector End = GetActorLocation();
		FQuat Rot = CriticalTriggerVolume->GetComponentQuat();
		FName ProfileName = FName("Trigger");
		FCollisionShape CollisionShape = CriticalTriggerVolume->GetCollisionShape();

		bool IsHit = World->SweepMultiByProfile(Hits, Start, End, Rot, ProfileName, CollisionShape);

		if (IsHit)
		{
			for (FHitResult& Hit : Hits)
			{
				AWielderBase* Wielder = Cast<AWielderBase>(Hit.GetActor());
				if (Wielder)
				{
					OverlappingWieldersInCritiacalVolume.Add(Wielder);
					Wielder->SetCanCriticalHit(true);
				}
			}
		}
	}
}

void AWielderBase::NotifyToGameMode()
{
	AFW_GameMode* GameMode = Cast<AFW_GameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		GameMode->BindCharacterDeathEvent(this);
	}
}

void AWielderBase::OnReceiveAnyDamageEvent(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	// TODO : 일반적인 데미지 처리.
#if GAME_TESTING
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString("Wielder's ReceiveAnyDamageEvent is called."));
#endif
	// 같은 팀 번호이면 처리하지 않음.
	AWielderBase* WielderBase = Cast<AWielderBase>(InstigatedBy->GetPawn());
	if (WielderBase && WielderBase->GetGenericTeamId() == TeamId) return;

	HandleWielderState(EActionState::EAS_GetDamaged);

	StatComponent->DecreaseHP(Damage);

	bIsDead = StatComponent->CheckDeathStatus();

	// PointDamage, RadiaDamage로 전달되지 않는 커스텀 데미지 타입인지 체크.
	if (DamageType->IsA<UCustomDamageType>())
	{
		ProcessCustomDamageType(DamageType);
		return;
	}

	if (bIsDead)
	{
		KilledByWielder = Cast<AWielderBase>(InstigatedBy->GetPawn());
		ProcessDeath();
	}
}

void AWielderBase::OnReceivePointDamageEvent(AActor* DamagedActor, float Damage, AController* InstigatedBy,
	FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, 
	FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser)
{
	// TODO : 타격 위치 정보에 따른 후처리 (캐릭터 애니메이션, 회피 등)
#if GAME_TESTING
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString("Wielder's ReceivePointDamageEvent is called."));
#endif
	
	AWielderBase* WielderBase = Cast<AWielderBase>(InstigatedBy->GetPawn());
	if (WielderBase && WielderBase->GetGenericTeamId() == TeamId) return;

	UWielderAnimInstance* WielderAnim = Cast<UWielderAnimInstance>(GetMesh()->GetAnimInstance());
	if (!IsValid(WielderAnim)) return;
	
	if (!bIsDead)
	{
		WielderAnim->PlayReactionMontage(EMontageType::EMT_GetHit);
	}

	FRotator ImpactRotation = UKismetMathLibrary::MakeRotFromZ(ShotFromDirection);
	StatComponent->ShowBloodEffect(HitLocation, ImpactRotation);
}

void AWielderBase::ProcessCustomDamageType(const UDamageType* DamageType)
{
#if GAME_TESTING
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString("Wielder's ProcessCustomDamageType is called."));
#endif

	UWielderAnimInstance* WielderAnim = Cast<UWielderAnimInstance>(GetMesh()->GetAnimInstance());
	if (!IsValid(WielderAnim)) return;

	if (const UCriticalDamageType* CustomDamage = Cast<UCriticalDamageType>(DamageType))
	{
		// 우선 GroggyHitReaction, GroggyHitDeath 몽타주를 통일했기에 이렇게 작성함.
		EMontageType MontageType = bIsDead ? EMontageType::EMT_GroggyHitDeath : EMontageType::EMT_GroggyHitReaction;
		
		WielderAnim->PlayReactionMontage(MontageType);
	}
	else
	{
		// DamageType is UCustomDamageType class.
	}
}

void AWielderBase::OnHitEnded()
{
	UE_LOG(LogTemp, Warning, TEXT("%s || Hit Ended. Change ActionState to Idle."), *UKismetSystemLibrary::GetDisplayName(this));
	HandleWielderState(EActionState::EAS_Idle);
}

void AWielderBase::Attack()
{

}

bool AWielderBase::IsCriticallyHittable()
{
	return StatComponent->GetDamageableType() == EDamageableType::EDT_CRITICALLY_HITTABLE;
}

void AWielderBase::SetCanCriticalHit(bool CanCriticalHit)
{
	bCanCriticalHit = CanCriticalHit;
}

void AWielderBase::ExecuteCriticalHitOnTarget()
{
	if (!IsValid(CriticalHitTarget) || !IsValid(EquipWeapon)) return;
	
	TSubclassOf<UCriticalDamageType> DamageType = UCriticalDamageType::StaticClass();

	// Apply damage.
	UGameplayStatics::ApplyDamage(CriticalHitTarget, EquipWeapon->GetDamage(), GetController(), EquipWeapon, DamageType);

	// Play sound.
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), EquipWeapon->GetCriticalHitSound(), GetActorLocation());
}

void AWielderBase::EnterCriticalHit()
{
	
}

void AWielderBase::ExitCriticalHit()
{

}

void AWielderBase::StopAttack()
{

}

void AWielderBase::HeavyAttack()
{

}

void AWielderBase::CriticalTriggerVolumeBeginOverlap(UPrimitiveComponent* SelfComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AWielderBase* WielderBase = Cast<AWielderBase>(OtherActor);
	if (!WielderBase) return;

	if (StatComponent->GetDamageableType() != EDamageableType::EDT_NORMAL)
	{
		OverlappingWieldersInCritiacalVolume.Add(WielderBase);
		WielderBase->SetCanCriticalHit(true);
	}
}

void AWielderBase::CriticalTriggerVolumeEndOverlap(UPrimitiveComponent* SelfComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AWielderBase* WielderBase = Cast<AWielderBase>(OtherActor);
	if (!WielderBase) return;

	if (StatComponent->GetDamageableType() != EDamageableType::EDT_NORMAL)
	{
		OverlappingWieldersInCritiacalVolume.Remove(WielderBase);
		WielderBase->SetCanCriticalHit(false);
	}
}

void AWielderBase::EquipFirstWeapon()
{

}

void AWielderBase::EquipSecondWeapon()
{

}

void AWielderBase::OnEquipEnded()
{
	HandleWielderState(EActionState::EAS_Idle);
}

void AWielderBase::OnUnequipEnded()
{
	HandleWielderState(EActionState::EAS_Idle);
}

void AWielderBase::Unequip()
{

}

void AWielderBase::ProcessDeath()
{
	if (!bIsDead)
		return;
	
	// 모든 몽타주 정지.
	GetMesh()->GetAnimInstance()->StopAllMontages(0.2f);
}

void AWielderBase::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	if (PrevMovementMode != EMovementMode::MOVE_Falling) return;

	if (GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Walking)
	{
		// Jump Ended.
		if (IsValid(EquipWeapon) && EquipWeapon->IsAttacking())
			WeaponComponent->JumpAttackLanding();

		MovementState = EMovementState::EMS_Idle;
	}
}

void AWielderBase::Die()
{
	if (OnKilled.IsBound())
	{
		OnKilled.Broadcast(this);
	}

	WeaponComponent->RemoveOwnerWeapon();

	UWielderAnimInstance* Anim = Cast<UWielderAnimInstance>(GetMesh()->GetAnimInstance());
	if (Anim)
	{
		Anim->SetIsDead(true);
	}

	GetCapsuleComponent()->SetCollisionProfileName(FName("Ragdoll"));
	GetMesh()->SetCollisionProfileName(FName("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);

	SetLifeSpan(4.0f);
}

bool AWielderBase::CheckDamageable()
{
	if (StatComponent->GetDamageImmunityType() != EDamageImmunityType::EDIT_None)
		return false;

	return true;
}

/// <summary>
/// 액션을 취할 수 있는지 체크하는 함수. 
/// SpecificAction이 동작 가능하면, 상태를 바꾸고 해당 동작을 실행함.
/// </summary>
bool AWielderBase::CheckTakeAction(EActionState SpecificAction, bool bCanTakeContinuously)
{
	// 특정 액션을 취하고 있지 않는 경우, 실행 가능.
	if (ActionState == EActionState::EAS_Idle)
	{
		HandleWielderState(SpecificAction);
		return true;
	}

	// 연속으로 액션을 취할 수 있는지 여부
	if (bCanTakeContinuously)
	{
		// 특정 액션을 취하는데 액션 상태와 일치할 경우 true 아니면 false.
		return  ActionState == SpecificAction ? true : false;
	}
	else
	{
		// Idle 상태일 때만 액션을 취하도록 구현.
		return false;
	}
}

FGenericTeamId AWielderBase::GetGenericTeamId() const
{
	return TeamId;
}

void AWielderBase::PlayMontage(UAnimMontage* Montage)
{
	UAnimInstance* CharacterAnim = GetMesh()->GetAnimInstance();

	if (CharacterAnim)
	{
		CharacterAnim->Montage_Play(Montage);
	}
}

void AWielderBase::TransferReactionMontages(TMap<FString, UAnimMontage*> AnimMontages)
{
	UWielderAnimInstance* WielderAnim = Cast<UWielderAnimInstance>(GetMesh()->GetAnimInstance());

	if (WielderAnim)
	{
		WielderAnim->SetReactionMontages(AnimMontages);
	}
}