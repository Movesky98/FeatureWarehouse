// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "Components/StatComponent.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AmmoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AmmoMesh"));
	RootComponent = AmmoMesh;
	AmmoMesh->SetCollisionProfileName(FName("Projectile"));
	
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(AmmoMesh);
	ProjectileMovementComponent->InitialSpeed = 3000.0f;
	ProjectileMovementComponent->MaxSpeed = 3500.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = false;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	AmmoMesh->OnComponentHit.AddDynamic(this, &AProjectile::OnBulletHit);
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::OnBulletHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NomalImpulse, const FHitResult& Hit)
{
	FString DebugMessage = FString("OnBulletHit :: Hit Actor is ") + UKismetSystemLibrary::GetDisplayName(Hit.GetActor());

	if (!Hit.GetActor()) return;

	AActor* HitActor = Hit.GetActor();
	UStatComponent* StatComponent = Cast<UStatComponent>(HitActor->GetComponentByClass(UStatComponent::StaticClass()));

	if (StatComponent)
	{
		StatComponent->GetDamaged(30.0f);
	}
	else
	{
		;
	}

	Destroy();
}

void AProjectile::FireDirection(const FVector& ShootDirection)
{
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}
