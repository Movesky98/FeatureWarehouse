// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"

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
	PrimaryActorTick.bCanEverTick = false;

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

	AFW_PlayerController* PlayerController = Cast<AFW_PlayerController>(Controller);

	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	SetTPV();
}

#pragma region InputAction
// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
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

	if (Controller != nullptr)
	{
		FVector RightVector = UKismetMathLibrary::GetRightVector(GetMesh()->GetComponentRotation());
		AddMovementInput(RightVector, Movement);
	}
}

void APlayerCharacter::MoveRight(const FInputActionValue& Value)
{
	float Movement = Value.Get<float>();

	if (Controller != nullptr)
	{
		FVector ForwardVector = UKismetMathLibrary::GetForwardVector(GetMesh()->GetComponentRotation());
		AddMovementInput(ForwardVector, Movement);
	}
}

void APlayerCharacter::Turn(const FInputActionValue& Value)
{
	float Movement = Value.Get<float>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(Movement);
	}
}

void APlayerCharacter::LookUp(const FInputActionValue& Value)
{
	float Movement = Value.Get<float>();

	if (Controller != nullptr)
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
	if (!MainWeapon) return;

	MainWeapon->Attack();
}

void APlayerCharacter::StopAttack()
{
	if (!MainWeapon) return;

	MainWeapon->StopAttack();
}

#pragma region View
void APlayerCharacter::SetTPV()
{
	ViewState = EStateOfViews::TPV;

	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 150.0f));
	SpringArm->SetRelativeRotation(FRotator(0.0f, 0.0f, 90.0f));
	SpringArm->TargetArmLength = 300.0f;
}

void APlayerCharacter::SetFPV()
{
	ViewState = EStateOfViews::FPV;
	
	bool IsSuccess = SpringArm->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("CameraSocket"));

	GetMesh()->HideBoneByName(FName("head"), EPhysBodyOp::PBO_None);
	SpringArm->bUsePawnControlRotation = true;

	if (IsSuccess)
	{
		SpringArm->SetRelativeLocation(FVector(20.0f, 0.0f, 0.0f));
		SpringArm->TargetArmLength = 0.0f;
	}
}

void APlayerCharacter::SetTopView()
{
	ViewState = EStateOfViews::TopView;

	GetMesh()->UnHideBoneByName(FName("head"));

	SpringArm->bUsePawnControlRotation = false;
	SpringArm->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, FName("None")); // ���̰� �ִ��� Ȯ���ؾ��� ��.
	SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 150.0f));
	SpringArm->SetRelativeRotation(FRotator(-90.0f, 0.0f, 90.0f));
	SpringArm->TargetArmLength = 1000.0f;
}

void APlayerCharacter::SwitchView()
{
	switch (ViewState)
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

void APlayerCharacter::PlayMontage(UAnimMontage* Montage)
{
	UPlayerAnimInstance* PlayerAnim = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());

	if (PlayerAnim)
	{
		PlayerAnim->Montage_Play(Montage);
	}
}