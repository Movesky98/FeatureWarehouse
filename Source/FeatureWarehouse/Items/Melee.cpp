// Fill out your copyright notice in the Description page of Project Settings.


#include "Melee.h"

#include "Characters/PlayerCharacter.h"
#include "Characters/Dummy.h"
#include "AnimInstance/PlayerAnimInstance.h"
#include "Components/StatComponent.h"

#include "Animation/AnimMontage.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"

AMelee::AMelee()
{
	BladeCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BladeCollision"));
	BladeCollision->SetupAttachment(RootComponent);
	BladeCollision->SetCollisionProfileName(FName("EnemyTrigger"));
	BladeCollision->SetGenerateOverlapEvents(true);
}

void AMelee::BeginPlay()
{
	Super::BeginPlay();

	BladeCollision->OnComponentBeginOverlap.AddDynamic(this, &AMelee::OnBladeBeginOverlap);
	BladeCollision->OnComponentEndOverlap.AddDynamic(this, &AMelee::OnBladeEndOverlap);
}

void AMelee::OnBladeBeginOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

void AMelee::OnBladeEndOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AMelee::BindMontage()
{
	if (!GetWeaponOwner()) return;

	APlayerCharacter* Player = Cast<APlayerCharacter>(GetWeaponOwner());
	if (!Player) return;
	
	UPlayerAnimInstance* PlayerAnim = Cast<UPlayerAnimInstance>(Player->GetMesh()->GetAnimInstance());

	if (PlayerAnim)
	{
		PlayerAnim->OnMontageEnded.AddDynamic(this, &AMelee::OnAttackEnded);
		PlayerAnim->OnNextAttackCheck.AddUFunction(this, FName("OnNextAttackChecked"));
	}
}

void AMelee::Attack(EStateOfViews CurView, FVector HitLocation)
{
	if (!CanAttack())
		return;

	if (CanCombo)
	{
		// Combo attack.
		if (!GetWeaponOwner()) 
			return;

		APlayerCharacter* Player = Cast<APlayerCharacter>(GetWeaponOwner());
		if (!Player) 
			return;

		UPlayerAnimInstance* PlayerAnim = Cast<UPlayerAnimInstance>(Player->GetMesh()->GetAnimInstance());

		if (PlayerAnim)
		{
			FName SectionName = AttackMontage->GetSectionName(MontageIndex);
			PlayerAnim->Montage_JumpToSection(SectionName, AttackMontage);

			MontageIndex++;
			CanCombo = false;
		}
	}
	else
	{
		// First attack.
		SetIsAttacking(true);

		APlayerCharacter* Player = Cast<APlayerCharacter>(GetWeaponOwner());
		if (Player)
		{
			Player->PlayMontage(AttackMontage);
			MontageIndex++;
		}
	}
}

bool AMelee::CanAttack()
{
	bool CanAttack = !UKismetMathLibrary::BooleanXOR(CanCombo, IsAttacking());

	return CanAttack;
}

void AMelee::OnAttackEnded(class UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == AttackMontage)
	{
		MontageIndex = 0;
		CanCombo = false;
		SetIsAttacking(false);
	}
}

void AMelee::OnNextAttackChecked()
{
	CanCombo = true;
}

void AMelee::StartAttackTrace()
{
	IgnoreActor.Add(GetWeaponOwner());

	GetWorldTimerManager().SetTimer(AttackTraceTimer, this, &AMelee::AttackTrace, 0.01f, true);
}

void AMelee::StopAttackTrace()
{
	GetWorldTimerManager().ClearTimer(AttackTraceTimer);

	IgnoreActor.Empty();
}

void AMelee::AttackTrace()
{
	FVector Start = GetSkeletalMesh()->GetSocketLocation(FName("BladeBottom"));
	FVector End = GetSkeletalMesh()->GetSocketLocation(FName("BladeTop"));
	FVector HalfSize = FVector(3.0f, 3.0f, 0.0f);

	FHitResult Hit;
	bool IsHit = UKismetSystemLibrary::LineTraceSingle(
		GetWorld(), 
		Start, 
		End, 
		ETraceTypeQuery::TraceTypeQuery4, 
		false, 
		IgnoreActor, 
		EDrawDebugTrace::ForDuration, 
		Hit, 
		true, 
		FLinearColor::Red, 
		FLinearColor::Green, 
		1.0f
	);

	if (IsHit)
	{
		// Dummy
		UStatComponent* HitActorComponent = Cast<UStatComponent>(Hit.GetActor()->GetComponentByClass(UStatComponent::StaticClass()));

		if (HitActorComponent)
		{
			IgnoreActor.Add(Hit.GetActor());

			HitActorComponent->GetDamaged(30.0f);

			FRotator ImpactRotation = UKismetMathLibrary::MakeRotFromZ(-Hit.ImpactNormal);
			HitActorComponent->ShowBloodEffect(Hit.ImpactPoint, ImpactRotation);
		}
	}
}