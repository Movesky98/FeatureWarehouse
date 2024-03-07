// Fill out your copyright notice in the Description page of Project Settings.


#include "Wielder.h"
#include "AnimInstance/WielderAnimInstance.h"
#include "GamePlay/WielderController.h"

#include "Components/WeaponComponent.h"

#include "Enums/TypeOfWeapon.h"
#include "Enums/ActionState.h"
#include "Enums/StateOfEnemy.h"
#include "Weapons/Weapon.h"
#include "Melees/Katana.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SphereComponent.h"

AWielder::AWielder()
{
	RecognizeRangeComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RecognizeRange"));
	RecognizeRangeComponent->SetupAttachment(RootComponent);
	RecognizeRangeComponent->SetCollisionProfileName(FName("PlayerTrigger"));
	RecognizeRangeComponent->ShapeColor = FColor::Purple;

	DetectionRangeComponent = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionRange"));
	DetectionRangeComponent->SetupAttachment(RootComponent);
	DetectionRangeComponent->SetCollisionProfileName(FName("PlayerTrigger"));
	DetectionRangeComponent->ShapeColor = FColor::Green;

	AttackRangeComponent = CreateDefaultSubobject<USphereComponent>(TEXT("AttackRange"));
	AttackRangeComponent->SetupAttachment(RootComponent);
	AttackRangeComponent->SetCollisionProfileName(FName("PlayerTrigger"));

	GetMesh()->SetCollisionProfileName(FName("Enemy"));

	AIControllerClass = AWielderController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AWielder::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	RecognizeRangeComponent->SetSphereRadius(RecognizeRange);
	DetectionRangeComponent->SetSphereRadius(DetectionRange);
	AttackRangeComponent->SetSphereRadius(AttackRange);

	RecognizeRangeComponent->OnComponentBeginOverlap.AddDynamic(this, &AWielder::OnRecognizeRangeBeginOverlap);
	RecognizeRangeComponent->OnComponentEndOverlap.AddDynamic(this, &AWielder::OnRecognizeRangeEndOverlap);

	DetectionRangeComponent->OnComponentBeginOverlap.AddDynamic(this, &AWielder::OnDetectionRangeBeginOverlap);
	DetectionRangeComponent->OnComponentEndOverlap.AddDynamic(this, &AWielder::OnDetectionRangeEndOverlap);

	AttackRangeComponent->OnComponentBeginOverlap.AddDynamic(this, &AWielder::OnAttackRangeBeginOverlap);
	AttackRangeComponent->OnComponentEndOverlap.AddDynamic(this, &AWielder::OnAttackRangeEndOverlap);
}

void AWielder::BeginPlay()
{
	Super::BeginPlay();

	if (bIsEquipWeapon) return;
	
	if (IsValid(WeaponBlueprint))
	{
		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			FRotator Rotator;
			FVector SpawnLocation = GetActorLocation();
			SpawnLocation.Z += 200.0f;

			AWeapon* SpawnWeapon = World->SpawnActor<AWeapon>(WeaponBlueprint, SpawnLocation, Rotator, SpawnParams);
			SpawnWeapon->Interact(this);

			bIsEquipWeapon = true;
		}
	}
}

void AWielder::OnRecognizeRangeBeginOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag(FName("Player")))
	{
		bIsRecognizedSomething = true;

		AWielderController* WielderController = Cast<AWielderController>(GetController());

		// 이미 컨트롤러가 플레이어를 식별하고 인식 범위에 들어왔을 경우.
		if (IsValid(WielderController) && WielderController->IsIdentifiedPlayer())
		{
			EquipFirstWeapon();
			WielderController->NotifyPerceiveSomething(OtherActor->GetActorLocation());
		}
	}
}

void AWielder::OnRecognizeRangeEndOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag(FName("Player")))
	{
		bIsRecognizedSomething = false;
	}
}

void AWielder::OnDetectionRangeBeginOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag(FName("Player")))
	{
		bIsPlayerApproached = true;
	}
}

void AWielder::OnDetectionRangeEndOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag(FName("Player")))
	{
		bIsPlayerApproached = false;
	}
}

void AWielder::OnAttackRangeBeginOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag(FName("Player")))
	{

	}
}

void AWielder::OnAttackRangeEndOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag(FName("Player")))
	{

	}
}

void AWielder::EquipEnded()
{
	AWielderController* WielderController = Cast<AWielderController>(GetController());
	if (IsValid(WielderController))
	{
		WielderController->NotifyEquipWeapon();
	}

	UWielderAnimInstance* WielderAnim = Cast<UWielderAnimInstance>(GetMesh()->GetAnimInstance());
	if (IsValid(WielderAnim))
	{
		WielderAnim->SetEquipWeapon(true);
	}
}

void AWielder::UnequipEnded()
{
	AWielderController* WielderController = Cast<AWielderController>(GetController());
	if (IsValid(WielderController))
	{
		WielderController->NotifyUnequip();
	}

	UWielderAnimInstance* WielderAnim = Cast<UWielderAnimInstance>(GetMesh()->GetAnimInstance());
	if (IsValid(WielderAnim))
	{
		WielderAnim->SetEquipWeapon(false);
	}
}


void AWielder::EquipFirstWeapon()
{
	if (!WeaponComponent->GetMainWeapon() || ActionState == EActionState::EAS_Swapping)
		return;

	if (WeaponComponent->CurEquipState() == EEquipState::SubWeapon)
	{
		if (EquipWeapon->GetWeaponType() == ETypeOfWeapon::Gun)
		{
			EquipWeapon->Unequip();
		}

		if (EquipWeapon->HasEquipMontage())
		{
			WeaponComponent->Swap();
			return;
		}
	}

	WeaponComponent->EquipMainWeapon();
}

void AWielder::Unequip()
{
	if (ActionState != EActionState::EAS_Idle) return;
	
	WeaponComponent->Unequip();
}

bool AWielder::IsPlayerApproached()
{
	return bIsPlayerApproached;
}

void AWielder::EngagingInCombat(AActor* AdversaryActor)
{

}

void AWielder::SetMovementSpeed(float Speed)
{
	GetCharacterMovement()->MaxWalkSpeed = Speed;
}