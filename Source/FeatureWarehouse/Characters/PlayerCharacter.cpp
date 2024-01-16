// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"
#include "GamePlay/FW_PlayerController.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "Items/Weapon.h"
#include "Enums/StateOfViews.h"
#include "Enums/UseTypeOfWeapon.h"
#include "Components/HealthComponent.h"
#include "Components/WeaponComponent.h"

#include "Interfaces/InteractInterface.h"

#include "AnimInstance/PlayerAnimInstance.h"
#include "GamePlay/FW_PlayerController.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetMesh());
	SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 160.0f));
	SpringArm->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	GetCapsuleComponent()->SetCollisionProfileName(FName("Player"));
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 200.0f;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("WeaponComponent"));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SKM_Manny(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny"));
	if (SKM_Manny.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SKM_Manny.Object);
	}

	static ConstructorHelpers::FClassFinder<UPlayerAnimInstance> ABP_Player(TEXT("/Game/Project/Animations/ABP_Player"));
	if (ABP_Player.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(ABP_Player.Class);
	}
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = Cast<AFW_PlayerController>(Controller);

	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	SetTPV();
}

void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}

#pragma region InputAction
// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString("setting up player input components."));
		// Move forward.
		EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &APlayerCharacter::MoveForward);

		EnhancedInputComponent->BindAction(MoveRightAction, ETriggerEvent::Triggered, this, &APlayerCharacter::MoveRight);

		EnhancedInputComponent->BindAction(TurnAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Turn);

		EnhancedInputComponent->BindAction(LookUpAction, ETriggerEvent::Triggered, this, &APlayerCharacter::LookUp);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &APlayerCharacter::JumpTriggered);

		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &APlayerCharacter::CrouchTriggered);

		EnhancedInputComponent->BindAction(SwitchViewAction, ETriggerEvent::Triggered, this, &APlayerCharacter::SwitchViewTriggered);

		EnhancedInputComponent->BindAction(SwitchRunAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Run);

		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Interact);

		EnhancedInputComponent->BindAction(FirstWeaponAction, ETriggerEvent::Triggered, this, &APlayerCharacter::FirstWeapon);

		EnhancedInputComponent->BindAction(SecondWeaponAction, ETriggerEvent::Triggered, this, &APlayerCharacter::SecondWeapon);

		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &APlayerCharacter::AttackTriggered);
	}
}

void APlayerCharacter::MoveForward(const FInputActionValue& Value)
{
	float Movement = Value.Get<float>();
	FVector Start = GetActorLocation() + FVector(0, 0, 50);

	if (Controller != nullptr)
	{
		FVector ForwardVector;
		if (PlayerController->GetView() == EStateOfViews::TopView)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString("MoveForward :: PlayerController"));
			FRotator ControlRotation = GetControlRotation();
			ControlRotation.Pitch = 0.0f;
			ControlRotation.Roll = 0.0f;

			ForwardVector = UKismetMathLibrary::GetForwardVector(ControlRotation);

			FVector ActorEnd = Start + GetActorRotation().Vector() * 500;
			FVector ControlEnd = Start + ControlRotation.Vector() * 500;

			DrawDebugLine(GetWorld(), Start, ActorEnd, FColor::Red, false, 0, 0, 10);
			DrawDebugLine(GetWorld(), Start, ControlEnd, FColor::Blue, false, 0, 0, 10);
		}
		else
		{
			ForwardVector = UKismetMathLibrary::GetForwardVector(GetActorRotation());
		}

		AddMovementInput(ForwardVector, Movement);
	}
}

void APlayerCharacter::MoveRight(const FInputActionValue& Value)
{
	float Movement = Value.Get<float>();

	FVector Start = GetActorLocation() + FVector(0, 0, 50);
	if (Controller != nullptr)
	{
		FVector RightVector;
		if (PlayerController->GetView() == EStateOfViews::TopView)
		{
			FRotator ControlRotation = GetControlRotation();
			ControlRotation.Pitch = 0.0f;

			RightVector = UKismetMathLibrary::GetRightVector(ControlRotation);

			FVector ActorEnd = Start + GetActorRotation().Vector() * 500;
			FVector ControlEnd = Start + ControlRotation.Vector() * 500;

			DrawDebugLine(GetWorld(), Start, ActorEnd, FColor::Red, false, 0, 0, 10);
			DrawDebugLine(GetWorld(), Start, ControlEnd, FColor::Blue, false, 0, 0, 10);
		}
		else
		{
			RightVector = UKismetMathLibrary::GetRightVector(GetActorRotation());
		}

		AddMovementInput(RightVector, Movement);
	}
}

void APlayerCharacter::Turn(const FInputActionValue& Value)
{
	float Movement = Value.Get<float>();
	
	if (PlayerController != nullptr && PlayerController->GetView() != EStateOfViews::TopView)
	{
		AddControllerYawInput(Movement);
	}
}

void APlayerCharacter::LookUp(const FInputActionValue& Value)
{
	float Movement = Value.Get<float>();

	if (PlayerController != nullptr && PlayerController->GetView() != EStateOfViews::TopView)
	{
		AddControllerPitchInput(Movement);
	}
}

void APlayerCharacter::JumpTriggered(const FInputActionValue& Value)
{
	bool IsPressed = Value.Get<bool>();

	if (IsPressed && Controller != nullptr)
	{
		bIsCrouched ? UnCrouch() : Jump();
	}
}

void APlayerCharacter::CrouchTriggered(const FInputActionValue& Value)
{
	bool IsPressed = Value.Get<bool>();

	if (IsPressed && Controller != nullptr)
	{
		bIsCrouched ? UnCrouch() : Crouch();
	}
}

void APlayerCharacter::Run(const FInputActionValue& Value)
{
	bool IsPressed = Value.Get<bool>();

	IsPressed ? GetCharacterMovement()->MaxWalkSpeed = 600.0f : GetCharacterMovement()->MaxWalkSpeed = 300.0f;
}

void APlayerCharacter::SwitchViewTriggered(const FInputActionValue& Value)
{
	bool IsPressed = Value.Get<bool>();

	if (IsPressed && Controller != nullptr)
	{
		SwitchView();
	}
}

void APlayerCharacter::Interact(const FInputActionValue& Value)
{
	bool IsPressed = Value.Get<bool>();

	if (IsPressed && Controller != nullptr)
	{
		FVector Start = Camera->GetComponentLocation();
		FVector End = Camera->GetComponentLocation() + Camera->GetForwardVector() * 1000.0f;

		AActor* Actor = FindInteractableActor(Start, End);

		IInteractInterface* InteractableActor = Cast<IInteractInterface>(Actor);

		if (InteractableActor)
		{
			InteractableActor->Interact(this);
		}
	}
}

void APlayerCharacter::FirstWeapon(const FInputActionValue& Value)
{
	bool IsPressed = Value.Get<bool>();

	if (IsPressed && Controller != nullptr)
	{
		EquipFirstWeapon();
	}
}

void APlayerCharacter::SecondWeapon(const FInputActionValue& Value)
{
	bool IsPressed = Value.Get<bool>();

	if (IsPressed && Controller != nullptr)
	{
		EquipSecondWeapon();
	}
}

void APlayerCharacter::AttackTriggered(const FInputActionValue& Value)
{
	bool IsPressed = Value.Get<bool>();

	if (Controller != nullptr)
	{
		if (IsPressed)
		{
			Attack();
		}
		else
		{
			StopAttack();
		}
	}
}

#pragma endregion


#pragma region View
void APlayerCharacter::SetTPV()
{
	if (!PlayerController) return;

	PlayerController->SetView(EStateOfViews::TPV);

	bool IsSuccess = SpringArm->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);

	if (IsSuccess)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString("Successed springarm attach to skeletalmesh."));

		bUseControllerRotationYaw = true;
		PlayerController->bShowMouseCursor = false;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		SpringArm->bUsePawnControlRotation = true;
		SpringArm->bInheritPitch = true;
		SpringArm->bInheritYaw = true;

		SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 150.0f));
		SpringArm->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
		SpringArm->TargetArmLength = 300.0f;
	}
}

void APlayerCharacter::SetFPV()
{
	if (!PlayerController) return;

	PlayerController->SetView(EStateOfViews::FPV);
	
	bool IsSuccess = SpringArm->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("CameraSocket"));

	if (IsSuccess)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString("Successed springarm attach to skeletalmesh's CameraSocket."));

		PlayerController->bShowMouseCursor = false;
		GetMesh()->HideBoneByName(FName("head"), EPhysBodyOp::PBO_None);

		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		SpringArm->bUsePawnControlRotation = true;
		SpringArm->bInheritYaw = true;

		SpringArm->SetRelativeLocation(FVector(20.0f, 0.0f, 0.0f));
		SpringArm->TargetArmLength = 0.0f;
	}
}

void APlayerCharacter::SetTopView()
{
	if (!PlayerController) return;
	
	PlayerController->SetControlRotation(FRotator::ZeroRotator);
	PlayerController->SetView(EStateOfViews::TopView);

	bool IsSuccess = SpringArm->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::KeepRelativeTransform);

	if (IsSuccess)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString("Successed springarm attach to the CapsuleComponent."));

		PlayerController->bShowMouseCursor = true;
		GetMesh()->UnHideBoneByName(FName("head"));

		GetCharacterMovement()->bOrientRotationToMovement = true;
		SpringArm->bUsePawnControlRotation = false;
		SpringArm->bInheritRoll = false;
		SpringArm->bInheritPitch = false;
		SpringArm->bInheritYaw = false;
		bUseControllerRotationYaw = false;

		SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 150.0f));
		SpringArm->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
		SpringArm->TargetArmLength = 1400.0f;
	}
}

void APlayerCharacter::SwitchView()
{
	if (!PlayerController) return;

	switch (PlayerController->GetView())
	{
	case EStateOfViews::TPV:
		SetFPV();
		break;
	case EStateOfViews::FPV:
		SetTopView();
		break;
	case EStateOfViews::TopView:
		SetTPV();
		break;
	default:
		break;
	}
}
#pragma endregion

AActor* APlayerCharacter::FindInteractableActor(const FVector Start, const FVector End)
{
	TArray<AActor*> IgnoreActors;
	FHitResult Hit;
	bool IsSuccess = UKismetSystemLibrary::LineTraceSingle(
		GetWorld(), 
		Start, 
		End, 
		ETraceTypeQuery::TraceTypeQuery3, 
		false, 
		IgnoreActors, 
		EDrawDebugTrace::ForDuration, 
		Hit, 
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		3.0f
	);

	if (IsSuccess)
	{
		return Hit.GetActor();
	}

	return nullptr;
}

void APlayerCharacter::EquipFirstWeapon()
{
	if (!WeaponComponent->GetFirstWeapon())
		return;

	if (WeaponComponent->GetFirstWeapon()->GetUseType() == EUseTypeOfWeapon::Sub)
	{
		WeaponComponent->SwapWeapon();

		MainWeapon = WeaponComponent->GetFirstWeapon();

		MainWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, MainWeapon->GetAttachSocketName());

		WeaponComponent->NotifyToAnimInstance();
	}
}

void APlayerCharacter::EquipSecondWeapon()
{
	if (!WeaponComponent->GetSecondWeapon())
		return;

	if (WeaponComponent->GetSecondWeapon()->GetUseType() == EUseTypeOfWeapon::Sub)
	{
		WeaponComponent->SwapWeapon();

		MainWeapon = WeaponComponent->GetSecondWeapon();

		MainWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, MainWeapon->GetAttachSocketName());

		WeaponComponent->NotifyToAnimInstance();
	}
}

void APlayerCharacter::Attack()
{
	if (!PlayerController) return;
	PlayerController->ViewClickLocation();

	if (!MainWeapon) return;
	MainWeapon->Attack();
}

void APlayerCharacter::StopAttack()
{
	if (!MainWeapon) return;

	MainWeapon->StopAttack();
}

void APlayerCharacter::PlayMontage(UAnimMontage* Montage)
{
	UPlayerAnimInstance* PlayerAnim = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());

	if (PlayerAnim)
	{
		PlayerAnim->Montage_Play(Montage);
	}
}