// Fill out your copyright notice in the Description page of Project Settings.


#include "Door.h"

#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ADoor::ADoor()
{
	DoorFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorFrame"));
	RootComponent = DoorFrame;
	
	Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	Door->SetupAttachment(RootComponent);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->SetCollisionProfileName(FName("PlayerTrigger"));

	FrontDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("FrontDirection"));
	FrontDirection->SetupAttachment(RootComponent);

	OpenLimitAngle = 90.0f;
}

void ADoor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ADoor::OnTriggerBeginOverlap);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ADoor::OnTriggerEndOverlap);
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADoor::Interact(AActor* InteractActor)
{
	if (!bIsNearTheCharacter) return;

	if (!bIsDoorMoving && !bIsOpen)
	{
		// Set position for opened from behind or opened from front.
		FVector DoorFrontVector = UKismetMathLibrary::GetForwardVector(FrontDirection->GetComponentRotation());
		FVector DirectionVector = GetActorLocation() - InteractActor->GetActorLocation();

		double DotProductResult = FVector::DotProduct(DoorFrontVector, DirectionVector.GetSafeNormal());
		bOpenedFromBehind =	DotProductResult > 0.0f ? true : false;

		if (bOpenedFromBehind) UE_LOG(LogTemp, Warning, TEXT("ADoor :: Open from behind !"));
		
		OpenDoor();
	}
}

void ADoor::OnTriggerBeginOverlap(class UPrimitiveComponent* selfComp, class AActor* otherActor, UPrimitiveComponent* otherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(otherActor))
	{
		bIsNearTheCharacter = true;
	}
}

void ADoor::OnTriggerEndOverlap(class UPrimitiveComponent* selfComp, class AActor* otherActor, UPrimitiveComponent* otherComp, int32 OtherBodyIndex)
{
	if (IsValid(otherActor))
	{
		bIsNearTheCharacter = false;

		if (bIsOpen)
		{
			CloseDoor();
		}
	}
}

void ADoor::SetDoorRotation()
{
	FRotator CurrentRotation = Door->GetRelativeRotation();

	if (!bIsOpen)
	{
		if (CurrentRotation.Yaw > -OpenLimitAngle)
		{
			Door->AddRelativeRotation(FRotator(0.0f, -1.0f, 0.0f));
		}
		else
		{
			Door->SetRelativeRotation(FRotator(0.0f, -OpenLimitAngle, 0.0f));
			GetWorldTimerManager().ClearTimer(DoorTimerHandle);
			bIsDoorMoving = false;
			bIsOpen = true;
		}
	}
	else
	{
		if (CurrentRotation.Yaw < 0.0f)
		{
			Door->AddRelativeRotation(FRotator(0.0f, 1.0f, 0.0f));
		}
		else
		{
			Door->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
			GetWorldTimerManager().ClearTimer(DoorTimerHandle);
			bIsDoorMoving = false;
			bIsOpen = false;

			// bOpenedFromBehind ? UnloadLevel(CurrentLevel) : UnloadLevel(NextLevel);
		}
	}
}

void ADoor::OpenDoor()
{
	bIsDoorMoving = true;
	
	// 여기서 서브레벨 로드&언로드 필요
	bOpenedFromBehind ? LoadLevel(NextLevel) : LoadLevel(CurrentLevel);

	GetWorldTimerManager().SetTimer(DoorTimerHandle, this, &ADoor::SetDoorRotation, 0.01f, true);
}

void ADoor::CloseDoor()
{
	bIsDoorMoving = true;

	GetWorldTimerManager().SetTimer(DoorTimerHandle, this, &ADoor::SetDoorRotation, 0.01f, true);
}

void ADoor::LoadLevel(FName LoadToLevel)
{
	if (LoadToLevel == FName("None")) return;
	
	UGameplayStatics::LoadStreamLevel(this, LoadToLevel, true, false, FLatentActionInfo());
}

void ADoor::UnloadLevel(FName UnloadToLevel)
{
	if (UnloadToLevel == FName("None")) return;

	UGameplayStatics::UnloadStreamLevel(this, UnloadToLevel, FLatentActionInfo(), false);
}