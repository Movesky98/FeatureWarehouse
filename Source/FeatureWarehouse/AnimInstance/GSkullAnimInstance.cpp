// This page is Gold Skull Enemy character class's definition part.

#include "GSkullAnimInstance.h"
#include "Characters/GoldenSkull.h"

#include "GameFramework/CharacterMovementComponent.h"

UGSkullAnimInstance::UGSkullAnimInstance()
{

}

void UGSkullAnimInstance::NativeInitializeAnimation()
{
	APawn* Pawn = TryGetPawnOwner();
	
	if (Pawn)
	{
		GoldenSkull = Cast<AGoldenSkull>(Pawn);
		MovementComponent = GoldenSkull->GetCharacterMovement();
	}
}

void UGSkullAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (GoldenSkull)
	{
		Speed = MovementComponent->Velocity.Length();
		Direction = CalculateDirection(MovementComponent->Velocity, GoldenSkull->GetActorRotation());
		bShouldMove = (Speed > 3.0f) && (MovementComponent->GetCurrentAcceleration() != FVector::ZeroVector);
		bIsCrouch = MovementComponent->IsCrouching();
		bIsFalling = MovementComponent->IsFalling();
	}
}