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

		if (MontageString.Equals(FString("GroggyHitReactionMontage")))
		{
			GroggyHitReactionMontage = IsValid(AnimMontage.Value) ? AnimMontage.Value : nullptr;
			continue;
		}

		if (MontageString.Equals(FString("GroggyHitDeathMontage")))
		{
			GroggyHitDeathMontage = IsValid(AnimMontage.Value) ? AnimMontage.Value : nullptr;
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

void UWielderAnimInstance::PlayGetGroggyHitMontage(bool IsDead)
{
	if (GroggyHitDeathMontage && GroggyHitReactionMontage)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString("There is GroggyHit Reaction Montages."));

		if (IsDead)
		{
			return;
		}

		if (Montage_IsPlaying(GroggyHitReactionMontage)) return;

		bool bPlayedSuccessfully = false;
		const float MontageLength = Montage_Play(GroggyHitReactionMontage);

		bPlayedSuccessfully = MontageLength > 0.f;

		if (bPlayedSuccessfully)
		{
			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &UWielderAnimInstance::OnGetGroggyHitEnded);
			Montage_SetEndDelegate(EndDelegate, GroggyHitReactionMontage);
		}
	}

	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString("There is no GroggyHit Reaction Montages."));
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

void UWielderAnimInstance::OnGetGroggyHitEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// 일단 뭘 할지는 보류

	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString("GetGroggyHit montage is ended."));
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

void UWielderAnimInstance::AnimNotify_GroggyAttackPoint()
{
	AWielderBase* WielderOwner = Cast<AWielderBase>(TryGetPawnOwner());
	WielderOwner->ExecuteCriticalHitOnTarget();
}

void UWielderAnimInstance::AnimNotify_KnockdownStart()
{
	float Duration = FMath::RandRange(1.0f, 4.0f);

	ExecutionDuration = Duration;
	ExecuteElapsedTime = 0.0f;

	GetWorld()->GetTimerManager().SetTimer(ExecutionTimerHandle, this, &UWielderAnimInstance::CalculateExeuctionTime, 0.01f, true);
}

void UWielderAnimInstance::CalculateExeuctionTime()
{
	if (!bIsDead)
	{
		if (ExecuteElapsedTime < ExecutionDuration)
		{
			ExecuteElapsedTime += 0.01f;
		}
		else
		{
			ExecuteElapsedTime = 0.0f;
			ExecutionDuration = 0.0f;

			GetWorld()->GetTimerManager().ClearTimer(ExecutionTimerHandle);
			
			Montage_Stop(1.0f, GroggyHitReactionMontage);
		}
	}
}
