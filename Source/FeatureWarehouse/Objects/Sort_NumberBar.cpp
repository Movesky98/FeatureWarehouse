// Fill out your copyright notice in the Description page of Project Settings.
#include "Sort_NumberBar.h"

#include "Components/TextRenderComponent.h" 
#include "DrawDebugHelpers.h"


// Sets default values
ASort_NumberBar::ASort_NumberBar()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(RootComponent);

	TextRenderComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRenderComponent"));
	TextRenderComponent->SetupAttachment(RootComponent);
	TextRenderComponent->SetRelativeLocation(FVector(51.0f, 21.5f, -13.0f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_Cube_Chamfer(TEXT("/Game/ImportAssets/A_Surface_Footstep/Environment_Assets/1M_Cube_Chamfer"));
	if (SM_Cube_Chamfer.Succeeded())
	{
		StaticMesh->SetStaticMesh(SM_Cube_Chamfer.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> NumberBar(TEXT("/Game/Project/Sort/Materials/MI_NumberBar"));
	if (NumberBar.Succeeded())
	{
		UMaterialInstance* MI_NumberBar = NumberBar.Object;
		StaticMesh->SetMaterial(0, MI_NumberBar);
	}
}

void ASort_NumberBar::SetNumber(int Num)
{
	Number = Num;
	TextRenderComponent->SetText(FText::FromString(FString::FromInt(Number)));
}

void ASort_NumberBar::VisualizeBoundingBox()
{
	FVector Origin;
	FVector BoxExtent;

	// Get the actor's bounding box
	GetActorBounds(true, Origin, BoxExtent);

	// Draw the bounding box
	DrawDebugBox(
		GetWorld(),
		Origin,
		BoxExtent,
		FColor::Blue,
		false,
		10.0f,
		0,
		5.0f
	);
}

void ASort_NumberBar::SetNumberBarScale3D_Z(float Z)
{
	FVector CurrentScale3D = StaticMesh->GetRelativeScale3D();
	CurrentScale3D.Z += Z;

	StaticMesh->SetRelativeScale3D(CurrentScale3D);
}

float ASort_NumberBar::GetNumberBarScale3D_Z()
{
	return StaticMesh->GetRelativeScale3D().Z;
}

// Called when the game starts or when spawned
void ASort_NumberBar::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASort_NumberBar::ChangeColor(FColor Color)
{


}

