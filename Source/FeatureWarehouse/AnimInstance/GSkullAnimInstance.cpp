// This page is Gold Skull Enemy character class's definition part.

#include "GSkullAnimInstance.h"
#include "Characters/GoldSkull.h"

#include "GameFramework/CharacterMovementComponent.h"

UGSkullAnimInstance::UGSkullAnimInstance()
{

}

void UGSkullAnimInstance::NativeInitializeAnimation()
{
	APawn* Pawn = TryGetPawnOwner();
	
	if (Pawn)
	{
		GoldSkull = Cast<AGoldSkull>(Pawn);
		MovementComponent = GoldSkull->GetCharacterMovement();
	}
}

void UGSkullAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (GoldSkull)
	{
		Speed = MovementComponent->Velocity.Length();
		Direction = CalculateDirection(MovementComponent->Velocity, GoldSkull->GetActorRotation());
		bShouldMove = (Speed > 3.0f) && (MovementComponent->GetCurrentAcceleration() != FVector::ZeroVector);
		bIsCrouch = MovementComponent->IsCrouching();
		bIsFalling = MovementComponent->IsFalling();
	}
}