// Fill out your copyright notice in the Description page of Project Settings.


#include "Wielder.h"
#include "AnimInstance/WielderAnimInstance.h"
#include "GamePlay/WielderController.h"

#include "Components/WeaponComponent.h"
#include "Components/StatComponent.h"

#include "Enums/TypeOfWeapon.h"
#include "Enums/ActionState.h"
#include "Enums/StateOfEnemy.h"
#include "Enums/BattleState.h"

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

	StatComponent->OnRetreatFromEnemy.BindUFunction(this, FName("RetreatFromEnemy"));
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

			AWeapon* SpawnWeapon = World->SpawnActor<AWeapon>(WeaponBlueprint, SpawnLocation, Rotator, SpawnParams);
			SpawnWeapon->Interact(this);

			bIsEquipWeapon = true;
		}
	}
}

AActor* AWielder::GetSeeingPawn()
{
	AWielderController* WielderController = Cast<AWielderController>(GetController());

	if (IsValid(WielderController))
	{
		return WielderController->GetSeeingPawn();
	}

	return nullptr;
}

void AWielder::DesignateEnemy(AActor* Enemy)
{
	AWielderController* WielderController = Cast<AWielderController>(GetController());

	if (IsValid(WielderController))
	{
		WielderController->DesignateEnemy(Enemy);
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

		AWielderController* WielderController = Cast<AWielderController>(GetController());

		if (IsValid(WielderController) && WielderController->IsIdentifiedPlayer())
		{
			EquipFirstWeapon();
			WielderController->NotifyEngageInBattle(OtherActor);
		}
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
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString("Player is In Attack Range."));

		AWielderController* WielderController = Cast<AWielderController>(GetController());

		if (IsValid(WielderController) && WielderController->IsIdentifiedPlayer())
		{
			EquipFirstWeapon();
			
			BattleState = EBattleState::Attacking;
			WielderController->NotifyBattleState(BattleState);
			WielderController->NotifyEnemyInAttackRange();
		}
	}
}

void AWielder::OnAttackRangeEndOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag(FName("Player")))
	{

	}
}

void AWielder::Attack()
{
	if (!IsValid(EquipWeapon))
		return;

	// 기본 공격
	ActionState = EActionState::EAS_Attacking;

	EquipWeapon->Attack();
}

void AWielder::RetreatFromEnemy()
{
	AWielderController* WielderController = Cast<AWielderController>(GetController());
	if (IsValid(WielderController))
	{
		// 잠시 일시정지.
		// WielderController->NotifyRetreat();
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

void AWielder::CheckEquipWeapon()
{
	// 현재 무기를 장착하고 있지 않다면 착용.
	if (!IsValid(EquipWeapon))
		EquipFirstWeapon();

	// 무기를 착용중이라면 아무것도 하지 않음.

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

bool AWielder::IsRecognizedSomething()
{
	return bIsRecognizedSomething;
}

// 이거 필요없을수도 있음. 참고할 것.
void AWielder::EngagingInCombat(AActor* AdversaryActor)
{

}

void AWielder::SetMovementSpeed(float Speed)
{
	GetCharacterMovement()->MaxWalkSpeed = Speed;
}