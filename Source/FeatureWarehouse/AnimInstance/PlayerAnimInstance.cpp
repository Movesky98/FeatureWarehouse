// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"

#include "Enums/TypeOfGun.h"
#include "Enums/TypeOfWeapon.h"
#include "Characters/PlayerCharacter.h"
#include "Components/WeaponComponent.h"

#include "GameFramework/CharacterMovementComponent.h"

UPlayerAnimInstance::UPlayerAnimInstance()
{

}

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	APawn* Pawn = TryGetPawnOwner();
	if (Pawn)
	{
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Pawn);

		if (PlayerCharacter)
		{
			Player = PlayerCharacter;
			MovementComponent = Player->GetCharacterMovement();
		}
	}
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (Player)
	{
		Speed = MovementComponent->Velocity.Length();
		Direction = CalculateDirection(MovementComponent->Velocity, Player->GetActorRotation());
		bIsCrouch = MovementComponent->IsCrouching();
		bIsFalling = MovementComponent->IsFalling();
		bShouldMove = (MovementComponent->GetCurrentAcceleration() != FVector::ZeroVector) && (Speed > 3.0f);
		Pitch = Player->GetBaseAimRotation().Pitch >= 180 ? Player->GetBaseAimRotation().Pitch - 360 : Player->GetBaseAimRotation().Pitch;
	}
}