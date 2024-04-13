// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponWielder.h"
#include "AnimInstance/WielderAnimInstance.h"
#include "GamePlay/FW_GameMode.h"

#include "Enums/ActionState.h"
#include "Enums/MovementState.h"
#include "Enums/Direction.h"
#include "Enums/FactionType.h"
#include "Weapon.h"

#include "Components/StatComponent.h"
#include "Components/WeaponComponent.h"
#include "Components/CapsuleComponent.h"
#include "NiagaraComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

AWeaponWielder::AWeaponWielder()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	StatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("StatComponent"));
	WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("WeaponComponent"));

	AfterImageComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("AfterImageComponent"));
	AfterImageComponent->SetupAttachment(RootComponent);

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

void AWeaponWielder::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	this->OnTakePointDamage.AddDynamic(this, &AWeaponWielder::OnReceivePointDamageEvent);

	TeamId = FGenericTeamId((uint8)Faction);

	UWielderAnimInstance* WielderAnim = Cast<UWielderAnimInstance>(GetMesh()->GetAnimInstance());
	if (WielderAnim)
	{
		WielderAnim->OnHitEnd.BindUObject(this, &AWeaponWielder::OnHitEnded);
	}
}

void AWeaponWielder::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeaponWielder::NotifyToGameMode()
{
	AFW_GameMode* GameMode = Cast<AFW_GameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		GameMode->BindCharacterDeathEvent(this);
	}
}

void AWeaponWielder::OnReceivePointDamageEvent(AActor* DamagedActor, float Damage, AController* InstigatedBy, 
	FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, 
	FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser)
{
	if (bIsDead) return;
	UWielderAnimInstance* WielderAnim = Cast<UWielderAnimInstance>(GetMesh()->GetAnimInstance());
	if (!WielderAnim) return;

	ActionState = EActionState::EAS_GetDamaged;
	StatComponent->DecreaseHP(Damage);

	bool IsDead = StatComponent->CheckDeathStatus();
	
	if (IsDead)
	{
		bIsDead = true;
		KilledByWielder = Cast<AWeaponWielder>(InstigatedBy->GetPawn());
		
		WielderAnim->PlayDeathMontage();
	}
	else
	{
		WielderAnim->PlayHitMontage();

		FRotator ImpactRotation = UKismetMathLibrary::MakeRotFromZ(ShotFromDirection);
		StatComponent->ShowBloodEffect(HitLocation, ImpactRotation);
	}
}

void AWeaponWielder::OnHitEnded()
{
	UE_LOG(LogTemp, Warning, TEXT("%s || Hit Ended. Change ActionState to Idle."), *UKismetSystemLibrary::GetDisplayName(this));
	ActionState = EActionState::EAS_Idle;
}

void AWeaponWielder::Attack()
{

}

void AWeaponWielder::StopAttack()
{

}

void AWeaponWielder::HeavyAttack()
{

}

void AWeaponWielder::EquipFirstWeapon()
{

}

void AWeaponWielder::EquipSecondWeapon()
{

}

void AWeaponWielder::OnEquipEnded()
{
	ActionState = EActionState::EAS_Idle;

	
}

void AWeaponWielder::OnUnequipEnded()
{
	ActionState = EActionState::EAS_Idle;
}

void AWeaponWielder::Unequip()
{

}

void AWeaponWielder::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	ACharacter::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	if (PrevMovementMode != EMovementMode::MOVE_Falling) return;

	if (GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Walking)
	{
		// Jump Ended.
		if (IsValid(EquipWeapon) && EquipWeapon->IsAttacking())
			WeaponComponent->JumpAttackLanding();

		MovementState = EMovementState::EMS_Idle;
	}
}

void AWeaponWielder::Die()
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

	SetLifeSpan(3.0f);
}

bool AWeaponWielder::CheckTakeAction(EActionState SpecificAction, bool bCanTakeContinuously)
{
	// 특정 액션을 취하고 있지 않는 경우, 실행 가능.
	if (ActionState == EActionState::EAS_Idle)
	{
		ActionState = SpecificAction;
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

FGenericTeamId AWeaponWielder::GetGenericTeamId() const
{
	return TeamId;
}

void AWeaponWielder::PlayMontage(UAnimMontage* Montage)
{
	UAnimInstance* CharacterAnim = GetMesh()->GetAnimInstance();

	if (CharacterAnim)
	{
		CharacterAnim->Montage_Play(Montage);
	}
}

void AWeaponWielder::TransferReactionMontages(TMap<FString, UAnimMontage*> AnimMontages)
{
	UWielderAnimInstance* WielderAnim = Cast<UWielderAnimInstance>(GetMesh()->GetAnimInstance());

	if (WielderAnim)
	{
		WielderAnim->SetReactionMontages(AnimMontages);
	}
}