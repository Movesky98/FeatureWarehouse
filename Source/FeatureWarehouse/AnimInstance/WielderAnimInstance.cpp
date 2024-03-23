// Fill out your copyright notice in the Description page of Project Settings.


#include "WielderAnimInstance.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

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
		Direction = CalculateDirection(MovementComponent->Velocity, Pawn->GetActorRotation());
		bIsCrouch = MovementComponent->IsCrouching();
		bIsFalling = MovementComponent->IsFalling();
		bShouldMove = (MovementComponent->GetCurrentAcceleration() != FVector::ZeroVector) && (Speed > 3.0f);
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

void UWielderAnimInstance::AnimNotify_DeathEnd() 
{
	if (OnDeathEnd.IsBound())
	{
		OnDeathEnd.Execute();
	}
}