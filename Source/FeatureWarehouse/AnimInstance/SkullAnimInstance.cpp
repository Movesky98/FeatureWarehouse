// Fill out your copyright notice in the Description page of Project Settings.


#include "SkullAnimInstance.h"
#include "Characters/Enemy.h"

#include "GameFramework/CharacterMovementComponent.h"

USkullAnimInstance::USkullAnimInstance()
{

}

void USkullAnimInstance::NativeInitializeAnimation()
{
	APawn* Pawn = TryGetPawnOwner();

	if (Pawn)
	{
		Enemy = Cast<AEnemy>(Pawn);
		MovementComponent = Enemy->GetCharacterMovement();
	}
}

void USkullAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (Enemy)
	{
		Speed = MovementComponent->Velocity.Length();
		Direction = CalculateDirection(MovementComponent->Velocity, Enemy->GetActorRotation());
		bShouldMove = (Speed > 3.0f) && (MovementComponent->GetCurrentAcceleration() != FVector::ZeroVector);
		bIsCrouch = MovementComponent->IsCrouching();
		bIsFalling = MovementComponent->IsFalling();
	}
}

void USkullAnimInstance::AnimNotify_NextAttackCheck()
{
	if (OnNextAttackCheck.IsBound())
	{
		OnNextAttackCheck.Execute();
	}
}