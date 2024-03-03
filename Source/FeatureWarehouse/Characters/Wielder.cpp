// Fill out your copyright notice in the Description page of Project Settings.


#include "Wielder.h"
#include "Enums/StateOfEnemy.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SphereComponent.h"

AWielder::AWielder()
{
	TriggerRangeComponent = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerRange"));
	TriggerRangeComponent->SetupAttachment(RootComponent);
	TriggerRangeComponent->SetCollisionProfileName(FName("PlayerTrigger"));
	TriggerRangeComponent->ShapeColor = FColor::Purple;

	DetectionRangeComponent = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionRange"));
	DetectionRangeComponent->SetupAttachment(RootComponent);
	DetectionRangeComponent->SetCollisionProfileName(FName("PlayerTrigger"));
	DetectionRangeComponent->ShapeColor = FColor::Green;

	AttackRangeComponent = CreateDefaultSubobject<USphereComponent>(TEXT("AttackRange"));
	AttackRangeComponent->SetupAttachment(RootComponent);
	AttackRangeComponent->SetCollisionProfileName(FName("PlayerTrigger"));

	GetMesh()->SetCollisionProfileName(FName("Enemy"));
}

void AWielder::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	TriggerRangeComponent->SetSphereRadius(TriggerRange);
	DetectionRangeComponent->SetSphereRadius(DetectionRange);
	AttackRangeComponent->SetSphereRadius(AttackRange);
}

void AWielder::OnTriggerRangeBeginOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void AWielder::OnTriggerRangesEndOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AWielder::OnDetectionRangeBeginOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void AWielder::OnDetectionRangeEndOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AWielder::OnAttackRangeBeginOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void AWielder::OnAttackRangeEndOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

bool AWielder::IsPlayerApproached()
{
	return false;
}

EStateOfEnemy AWielder::IdentifiedPlayer()
{
	return EStateOfEnemy::Chase;
}

void AWielder::EngagingInCombat(AActor* AdversaryActor)
{

}

void AWielder::SetMovementSpeed(float Speed)
{
	GetCharacterMovement()->MaxWalkSpeed = Speed;
}