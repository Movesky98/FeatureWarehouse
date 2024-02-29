// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"

#include "Components/ItemDescriptionComponent.h"
#include "Enums/TypeOfItem.h"

#include "Components/SphereComponent.h"


// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	RootComponent = SkeletalMesh;
	SkeletalMesh->SetCollisionProfileName(FName("NoCollision"));

	ItemCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ItemCollision"));
	ItemCollision->SetupAttachment(RootComponent);
	ItemCollision->SetCollisionProfileName(FName("Item"));
	ItemCollision->ShapeColor = FColor::Red;

	TriggerZone = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerZone"));
	TriggerZone->SetupAttachment(RootComponent);
	TriggerZone->SetCollisionProfileName(FName("PlayerTrigger"));
	TriggerZone->ShapeColor = FColor::Green;
	TriggerZone->InitSphereRadius(300.0f);

	ItemDescriptionComponent = CreateDefaultSubobject<UItemDescriptionComponent>(TEXT("ItemDescriptionComponent"));
	ItemDescriptionComponent->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MI_ItemOutline(TEXT("/Game/Project/Materials/MI_Item_Outline"));
	if (MI_ItemOutline.Succeeded())
	{
		OutlineMaterial = MI_ItemOutline.Object;
	}
}

void AItem::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ItemDescriptionComponent->Init();

	TriggerZone->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnTriggerBeginOverlap);
	TriggerZone->OnComponentEndOverlap.AddDynamic(this, &AItem::OnTriggerEndOverlap);
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

}

void AItem::OnTriggerBeginOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag(FName("Player")))
	{
		ItemDescriptionComponent->ShowUI();
		ActiveOverlay();
	}
}

void AItem::OnTriggerEndOverlap(class UPrimitiveComponent* SelfComp, class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag(FName("Player")))
	{
		ItemDescriptionComponent->HideUI();
		DeactiveOverlay();
	}
}

void AItem::ActiveOverlay()
{
	SkeletalMesh->SetOverlayMaterial(OutlineMaterial);
}

void AItem::DeactiveOverlay()
{
	SkeletalMesh->SetOverlayMaterial(nullptr);
}