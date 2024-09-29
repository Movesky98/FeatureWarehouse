// Fill out your copyright notice in the Description page of Project Settings.


#include "WielderAnimInstance.h"
#include "Characters/WielderBase.h"
#include "Enums/PoseType.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "KismetAnimationLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

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
		if (bIsCrouch) PoseType = EPoseType::EPT_Crouch;
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

		if (MontageString.Equals(FString("GetUpFromFrontMontage")))
		{
			GetUpFromFrontMontage = IsValid(AnimMontage.Value) ? AnimMontage.Value : nullptr;
		}

		if (MontageString.Equals(FString("GetUpFromBackMontage")))
		{
			GetUpFromBackMontage = IsValid(AnimMontage.Value) ? AnimMontage.Value : nullptr;
		}
	}
}

void UWielderAnimInstance::PlayReactionMontage(EMontageType MontageType)
{
	// 재생할 몽타주
	UAnimMontage* PlayMontage = nullptr;
	void (UWielderAnimInstance:: *MontageEndFunction)(UAnimMontage*, bool) = nullptr;

	switch (MontageType)
	{
	case EMontageType::EMT_Death:
		PlayMontage = DeathMontage;
		MontageEndFunction = &UWielderAnimInstance::OnDeath;
		break;
	case EMontageType::EMT_GetHit:
		PlayMontage = HitMontage;
		MontageEndFunction = &UWielderAnimInstance::OnHitEnded;
		break;
	case EMontageType::EMT_Retreat:
		PlayMontage = RetreatMontage;
		MontageEndFunction = &UWielderAnimInstance::OnRetreatEnded;
		break;
	case EMontageType::EMT_GroggyHitReaction:
		PlayMontage = GroggyHitReactionMontage;
		MontageEndFunction = &UWielderAnimInstance::OnGetGroggyHitEnded;
		break;
	case EMontageType::EMT_GroggyHitDeath:
		PlayMontage = GroggyHitDeathMontage;
		MontageEndFunction = &UWielderAnimInstance::OnGetGroggyHitEnded;
		break;
	case EMontageType::EMT_GetUpFromFront:
		PlayMontage = GetUpFromFrontMontage;
		// Set MontageEndFunction.
		break;
	case EMontageType::EMT_GetUpFromBack:
		PlayMontage = GetUpFromBackMontage;
		// Set MontageEndFunction.
		break;
	default:
		break;
	}

	if (PlayMontage)
	{
		bool bPlayedSuccessfully = false;
		const float MontageLength = Montage_Play(PlayMontage);

		if (!MontageEndFunction) return;

		bPlayedSuccessfully = MontageLength > 0.f;

		if (bPlayedSuccessfully)
		{
			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, MontageEndFunction);
			Montage_SetEndDelegate(EndDelegate, PlayMontage);
		}
	}
}

void UWielderAnimInstance::SetPoseType(EPoseType Type)
{
	PoseType = Type;
}

EPoseType UWielderAnimInstance::GetPoseType()
{
	return PoseType;
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

			// 캐릭터가 다시 일어나야함.
			PlayReactionMontage(EMontageType::EMT_GetUpFromFront);
		}
	}
}
