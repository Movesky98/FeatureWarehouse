// Fill out your copyright notice in the Description page of Project Settings.


#include "WielderAnimInstance.h"
#include "Characters/WielderBase.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "KismetAnimationLibrary.h"

void UWielderAnimInstance::NativeInitializeAnimation()
{
	APawn* Pawn = TryGetPawnOwner();
	if (Pawn)
	{
		ACharacter* Character = Cast<ACharacter>(Pawn);
		if (Character)
		{
			MovementComponent = Character->GetCharacterMovement();
		}
	}
}

void UWielderAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	APawn* Pawn = TryGetPawnOwner();
	if (Pawn)
	{
		Speed = MovementComponent->Velocity.Length();
		Direction = UKismetAnimationLibrary::CalculateDirection(MovementComponent->Velocity, Pawn->GetActorRotation());
		bIsCrouch = MovementComponent->IsCrouching();
		bIsFalling = MovementComponent->IsFalling();
		bShouldMove = (MovementComponent->GetCurrentAcceleration() != FVector::ZeroVector) && (Speed > 3.0f);
	}
}

void UWielderAnimInstance::SetReactionMontages(TMap<FString, UAnimMontage*> AnimMontages)
{
	for (auto& AnimMontage : AnimMontages)
	{
		FString MontageString = AnimMontage.Key;

		if (MontageString.Equals(FString("GetDamagedMontage")))
		{
			HitMontage = IsValid(AnimMontage.Value) ? AnimMontage.Value : nullptr;
			continue;
		}

		if (MontageString.Equals(FString("DeathMontage")))
		{
			DeathMontage = IsValid(AnimMontage.Value) ? AnimMontage.Value : nullptr;
			continue;
		}

		if (MontageString.Equals(FString("KnockdownMontage")))
		{

			continue;
		}

		if (MontageString.Equals(FString("RetreatMontage")))
		{
			RetreatMontage = IsValid(AnimMontage.Value) ? AnimMontage.Value : nullptr;
			continue;
		}
	}
}

void UWielderAnimInstance::PlayDeathMontage()
{
	if (DeathMontage)
	{
		bool bPlayedSuccessfully = false;
		const float MontageLength = Montage_Play(DeathMontage);

		bPlayedSuccessfully = MontageLength > 0.f;

		if (bPlayedSuccessfully)
		{
			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &UWielderAnimInstance::OnDeath);
			Montage_SetEndDelegate(EndDelegate, DeathMontage);
		}
	}
	else
	{
		AWielderBase* Wielder = Cast<AWielderBase>(TryGetPawnOwner());
		if (Wielder)
		{
			Wielder->Die();
		}
	}
}

void UWielderAnimInstance::PlayHitMontage()
{
	if (HitMontage)
	{
		bool bPlayedSuccessfully = false;
		const float MontageLength = Montage_Play(HitMontage);

		bPlayedSuccessfully = MontageLength > 0.f;

		if (bPlayedSuccessfully)
		{
			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &UWielderAnimInstance::OnHitEnded);
			Montage_SetEndDelegate(EndDelegate, HitMontage);
		}
	}
	else
	{
		if (OnHitEnd.IsBound()) OnHitEnd.Execute();
	}
}

void UWielderAnimInstance::PlayRetreatMontage()
{
	if (RetreatMontage)
	{
		if (Montage_IsPlaying(RetreatMontage)) return;

		bool bPlayedSuccessfully = false;
		const float MontageLength = Montage_Play(RetreatMontage);

		bPlayedSuccessfully = MontageLength > 0.f;

		if (bPlayedSuccessfully)
		{
			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &UWielderAnimInstance::OnRetreatEnded);
			Montage_SetEndDelegate(EndDelegate, RetreatMontage);
		}
	}
	else
	{
		if (OnRetreatEnd.IsBound()) OnRetreatEnd.Execute();
	}
}

void UWielderAnimInstance::OnDeath(UAnimMontage* Montage, bool bInterrupted)
{
	if (DeathMontage != Montage) return;

	AWielderBase* WielderBase = Cast<AWielderBase>(TryGetPawnOwner());
	if (IsValid(WielderBase))
	{
		WielderBase->Die();
	}
}

void UWielderAnimInstance::OnHitEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (HitMontage != Montage) return;

	if (!Montage_IsPlaying(HitMontage))
	{
		if (OnHitEnd.IsBound()) OnHitEnd.Execute();
	}
}

void UWielderAnimInstance::OnRetreatEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (RetreatMontage != Montage) return;

	if (!Montage_IsPlaying(RetreatMontage))
	{
		if (OnRetreatEnd.IsBound()) OnRetreatEnd.Execute();
	}
}

void UWielderAnimInstance::AnimNotify_NextAttackCheck()
{
	if (OnNextAttackCheck.IsBound())
	{
		OnNextAttackCheck.Execute();
	}
}

void UWielderAnimInstance::AnimNotify_HoldMeleeWeapon()
{
	if (OnHoldMeleeWeapon.IsBound())
	{
		OnHoldMeleeWeapon.Execute();
	}
}

void UWielderAnimInstance::AnimNotify_PlaySlashSound()
{
	if (OnPlaySlashSound.IsBound())
	{
		OnPlaySlashSound.Execute();
	}
}

void UWielderAnimInstance::AnimNotify_PlayEquipSound()
{
	if (OnPlayEquipSound.IsBound())
	{
		OnPlayEquipSound.Execute();
	}
}

void UWielderAnimInstance::AnimNotify_PlayUnequipSound()
{
	if (OnPlayUnequipSound.IsBound())
	{
		OnPlayUnequipSound.Execute();
	}
}