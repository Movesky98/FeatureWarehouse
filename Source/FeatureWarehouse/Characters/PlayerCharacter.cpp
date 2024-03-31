// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"
#include "GamePlay/FW_PlayerController.h"
#include "Widgets/PlayerMenu.h"
#include "Characters/Wielder.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "Weapon.h"
#include "Guns/Gun.h"

#include "Enums/Direction.h"
#include "Enums/TypeOfWeapon.h"
#include "Enums/StateOfViews.h"
#include "Enums/UseTypeOfWeapon.h"
#include "Enums/MovementState.h"
#include "Enums/ActionState.h"

#include "Components/WeaponComponent.h"
#include "Components/StatComponent.h"

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

	GetCapsuleComponent()->SetCollisionProfileName(FName("Pawn"));
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 200.0f;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	GetMesh()->SetCollisionProfileName(FName("Player"));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SKM_Manny(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny"));
	if (SKM_Manny.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SKM_Manny.Object);
	}

	static ConstructorHelpers::FClassFinder<UPlayerAnimInstance> ABP_Player(TEXT("/Game/Project/Animations/Player/ABP_Player"));
	if (ABP_Player.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(ABP_Player.Class);
	}

	MovementState = EMovementState::EMS_Idle;

	Tags.Add(FName("Player"));

	LockChangeElapsedTime = 0.0f;
	LockChangeDuration = 1.0f;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = Cast<AFW_PlayerController>(Controller);

	SetTPP();
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
		// Move forward.
		EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &APlayerCharacter::MoveForward);
		EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Completed, this, &APlayerCharacter::SetMovementStateIdle);

		EnhancedInputComponent->BindAction(MoveRightAction, ETriggerEvent::Triggered, this, &APlayerCharacter::MoveRight);

		EnhancedInputComponent->BindAction(TurnAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Turn);

		EnhancedInputComponent->BindAction(LookUpAction, ETriggerEvent::Triggered, this, &APlayerCharacter::LookUp);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &APlayerCharacter::JumpTriggered);

		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &APlayerCharacter::CrouchTriggered);

		EnhancedInputComponent->BindAction(SwitchRunAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Run);

		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Interact);

		EnhancedInputComponent->BindAction(FirstWeaponAction, ETriggerEvent::Triggered, this, &APlayerCharacter::FirstWeapon);

		EnhancedInputComponent->BindAction(SecondWeaponAction, ETriggerEvent::Triggered, this, &APlayerCharacter::SecondWeapon);

		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &APlayerCharacter::AttackTriggered);

		EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Zoom);

		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &APlayerCharacter::DodgePressed);

		EnhancedInputComponent->BindAction(LockOnAction, ETriggerEvent::Triggered, this, &APlayerCharacter::LockOn);
	}
}

void APlayerCharacter::MoveForward(const FInputActionValue& Value)
{
	float Movement = Value.Get<float>();
	bool IsPressed = Value.Get<bool>();

	if (ActionState != EActionState::EAS_Idle) return;

	FVector Start = GetActorLocation() + FVector(0, 0, 50);

	if (Controller != nullptr)
	{
		FVector ForwardVector;
		if (PlayerController->GetPerspective() == EStateOfViews::TDP)
		{
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

		if (MovementState == EMovementState::EMS_Idle)
		{
			MovementState = EMovementState::EMS_Walking;
		}
	}
}

void APlayerCharacter::MoveRight(const FInputActionValue& Value)
{
	float Movement = Value.Get<float>();

	if (ActionState != EActionState::EAS_Idle) return;

	FVector Start = GetActorLocation() + FVector(0, 0, 50);
	if (Controller != nullptr)
	{
		FVector RightVector;
		if (PlayerController->GetPerspective() == EStateOfViews::TDP)
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
	
	if (PlayerController != nullptr && PlayerController->GetPerspective() != EStateOfViews::TDP)
	{
		if (bIsTargetLocked)
		{
			FindNearbyLockTarget(Movement);
		}
		else
		{
			AddControllerYawInput(Movement);
		}
	}
}

void APlayerCharacter::LookUp(const FInputActionValue& Value)
{
	float Movement = Value.Get<float>();

	if (PlayerController != nullptr && PlayerController->GetPerspective() != EStateOfViews::TDP)
	{
		if (bIsTargetLocked)
		{
			FindNearbyLockTarget(0.0f, Movement);
		}
		else
		{
			AddControllerPitchInput(Movement);	
		}
	}
}

void APlayerCharacter::JumpTriggered(const FInputActionValue& Value)
{
	bool IsPressed = Value.Get<bool>();

	if (IsPressed && Controller != nullptr)
	{
		if (bIsCrouched)
		{
			UnCrouch();

			MovementState = EMovementState::EMS_Idle;
		}
		else
		{
			Jump();

			MovementState = EMovementState::EMS_Jumping;
		}
	}
}

void APlayerCharacter::CrouchTriggered(const FInputActionValue& Value)
{
	bool IsPressed = Value.Get<bool>();

	if (IsPressed && Controller != nullptr)
	{
		if (bIsCrouched)
		{
			UnCrouch();

			MovementState = EMovementState::EMS_Idle;
		}
		else
		{
			Crouch();

			MovementState = EMovementState::EMS_Crouching;
		}
	}
}

void APlayerCharacter::Run(const FInputActionValue& Value)
{
	bool IsPressed = Value.Get<bool>();

	if (IsPressed)
	{
		GetCharacterMovement()->MaxWalkSpeed = 600.0f;

		if (MovementState == EMovementState::EMS_Idle || MovementState == EMovementState::EMS_Walking)
		{
			MovementState = EMovementState::EMS_Running;
		}
	} 
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = 300.0f;

		if (MovementState == EMovementState::EMS_Running)
		{
			MovementState = EMovementState::EMS_Walking;
		}
	}
}

void APlayerCharacter::Interact(const FInputActionValue& Value)
{
	if (bIsDead) return;

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
	if (bIsDead) return;

	bool IsPressed = Value.Get<bool>();

	if (IsPressed && Controller != nullptr)
	{
		EquipFirstWeapon();
	}
}

void APlayerCharacter::SecondWeapon(const FInputActionValue& Value)
{
	if (bIsDead) return;

	bool IsPressed = Value.Get<bool>();

	if (IsPressed && Controller != nullptr)
	{
		EquipSecondWeapon();
	}
}

void APlayerCharacter::AttackTriggered(const FInputActionValue& Value)
{
	if (bIsDead) return;

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

void APlayerCharacter::Zoom(const FInputActionValue& Value)
{
	if (!IsValid(EquipWeapon) || bIsDead) return;

	bool IsPressed = Value.Get<bool>();

	if (Controller != nullptr)
	{
		switch (EquipWeapon->GetWeaponType())
		{
		case ETypeOfWeapon::Gun:
			if (IsPressed)
			{
				StartZoom();
			}
			else
			{
				StopZoom();
			}
			break;
		case ETypeOfWeapon::Melee:
			HeavyAttack();
			break;
		default:
			break;
		}
	}
}

void APlayerCharacter::SetMovementStateIdle()
{
	if (IsValid(Controller))
	{
		if(MovementState == EMovementState::EMS_Walking || MovementState == EMovementState::EMS_Running)
			MovementState = EMovementState::EMS_Idle;
	}
}

void APlayerCharacter::DodgePressed(const FInputActionValue& Value)
{
	bool IsPressed = Value.Get<bool>();

	if (IsPressed && !DodgeMontages.IsEmpty())
	{
		Dodge();
	}
}

void APlayerCharacter::LockOn(const FInputActionValue& Value)
{
	bool IsPressed = Value.Get<bool>();

	UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter's Lock On functio is called"));

	if (IsPressed)
	{
		TryLockTarget();
	}
}

#pragma endregion

#pragma region View
void APlayerCharacter::SetTPP()
{
	if (!PlayerController) return;

	PlayerController->SetPerspective(EStateOfViews::TPP);

	bool IsSuccess = SpringArm->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);

	if (IsSuccess)
	{
		GetMesh()->UnHideBoneByName(FName("head"));

		bUseControllerRotationYaw = true;
		PlayerController->bShowMouseCursor = false;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		SpringArm->bUsePawnControlRotation = true;
		SpringArm->bInheritPitch = true;
		SpringArm->bInheritYaw = true;

		MaxArmLength = 300.0f;
		MinArmLength = 200.0f;

		SpringArm->SetRelativeLocation(FVector(-80.0f, 0.0f, 160.0f));
		SpringArm->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
		SpringArm->TargetArmLength = MaxArmLength;
	}
}

void APlayerCharacter::SetFPP()
{
	if (!PlayerController) return;

	PlayerController->SetPerspective(EStateOfViews::FPP);
	
	bool IsSuccess = SpringArm->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("CameraSocket"));

	if (IsSuccess)
	{
		PlayerController->bShowMouseCursor = false;
		GetMesh()->HideBoneByName(FName("head"), EPhysBodyOp::PBO_None);

		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		SpringArm->bUsePawnControlRotation = true;
		SpringArm->bInheritYaw = true;

		MaxArmLength = 0.0f;
		MinArmLength = 0.0f;

		SpringArm->SetRelativeLocation(FVector(20.0f, 0.0f, 0.0f));
		SpringArm->TargetArmLength = MaxArmLength;
	}
}

void APlayerCharacter::SetTDP()
{
	if (!PlayerController) return;
	
	PlayerController->SetControlRotation(FRotator::ZeroRotator);
	PlayerController->SetPerspective(EStateOfViews::TDP);

	bool IsSuccess = SpringArm->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::KeepRelativeTransform);

	if (IsSuccess)
	{
		PlayerController->bShowMouseCursor = true;
		GetMesh()->UnHideBoneByName(FName("head"));

		GetCharacterMovement()->bOrientRotationToMovement = true;
		SpringArm->bUsePawnControlRotation = false;
		SpringArm->bInheritRoll = false;
		SpringArm->bInheritPitch = false;
		SpringArm->bInheritYaw = false;
		bUseControllerRotationYaw = false;

		MaxArmLength = 1400.0f;
		MinArmLength = 800.0f;

		SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 150.0f));
		SpringArm->SetRelativeRotation(FRotator(-50.0f, 0.0f, 0.0f));
		SpringArm->TargetArmLength = MaxArmLength;
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
	if (ActionState == EActionState::EAS_Swapping) return;
	if (!WeaponComponent->GetMainWeapon() || EquipWeapon == WeaponComponent->GetMainWeapon()) return;

	EActionState SpecificAction = EActionState::EAS_Equipping;

	bool CanTakeAction = CheckTakeAction(SpecificAction, false);
	if (!CanTakeAction) return;

	// 현재 들고 있는 무기가 두번째 무기인 경우
	if (WeaponComponent->CurEquipState() == EEquipState::SubWeapon)
	{
		if (EquipWeapon->GetWeaponType() == ETypeOfWeapon::Gun)
		{
			// 총의 경우 장착 해제 몽타주가 없으므로 바로 상태를 바꿔줌.
			EquipWeapon->Unequip();
			ActionState = EActionState::EAS_Idle;
		}

		if (EquipWeapon->HasEquipMontage())
		{
			WeaponComponent->Swap();
			return;
		}
	}

	WeaponComponent->EquipMainWeapon();
}

void APlayerCharacter::EquipSecondWeapon()
{
	if (ActionState == EActionState::EAS_Swapping) return;
	if (!WeaponComponent->GetSubWeapon() || EquipWeapon == WeaponComponent->GetSubWeapon()) return;

	EActionState SpecificAction = EActionState::EAS_Equipping;

	bool CanTakeAction = CheckTakeAction(SpecificAction, false);
	if (!CanTakeAction) return;

	if (WeaponComponent->CurEquipState() == EEquipState::MainWeapon)
	{
		if (EquipWeapon->GetWeaponType() == ETypeOfWeapon::Gun)
		{
			// 총의 경우 장착 해제 몽타주가 없으므로 바로 상태를 바꿔줌.
			EquipWeapon->Unequip();
			ActionState = EActionState::EAS_Idle;
		}

		if (EquipWeapon->HasEquipMontage())
		{
			WeaponComponent->Swap();
			return;
		}
	}

	WeaponComponent->EquipSubWeapon();
}

void APlayerCharacter::Attack()
{
	if (!PlayerController) return;
	if (!EquipWeapon) return;

	EActionState SpecificAction = EActionState::EAS_Attacking;

	bool CanTakeAction = CheckTakeAction(SpecificAction, true);
	if (!CanTakeAction) return;


	if (PlayerController->GetPerspective() == EStateOfViews::TDP)
	{
		FVector HitLocation = PlayerController->ViewClickLocation();
		EquipWeapon->Attack(PlayerController->GetPerspective(), HitLocation);

		return;
	}

	EquipWeapon->Attack(PlayerController->GetPerspective());
}

void APlayerCharacter::HeavyAttack()
{
	if (!PlayerController) return;
	if (!EquipWeapon) return;

	EActionState SpecificAction = EActionState::EAS_HeavyAttacking;

	bool CanTakeAction = CheckTakeAction(SpecificAction, true);
	if (!CanTakeAction) return;

	if (PlayerController->GetPerspective() == EStateOfViews::TDP)
	{
		FVector HitLocation = PlayerController->ViewClickLocation();
		EquipWeapon->Attack(PlayerController->GetPerspective(), HitLocation);

		return;
	}

	EquipWeapon->Attack(PlayerController->GetPerspective());
}

void APlayerCharacter::StopAttack()
{
	if (!EquipWeapon) return;

	EquipWeapon->StopAttack();
}

void APlayerCharacter::PlayMontage(UAnimMontage* Montage)
{
	UPlayerAnimInstance* PlayerAnim = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());

	if (PlayerAnim)
	{
		PlayerAnim->Montage_Play(Montage);
	}
}

void APlayerCharacter::ZoomIn()
{
	if (!PlayerController) return;

	switch (PlayerController->GetPerspective())
	{
	case EStateOfViews::FPP:
		// Adjust the camera's FOV value.
		

		break;
	case EStateOfViews::TDP:
	case EStateOfViews::TPP:
		// Adjust SpringArm's length value.
		SpringArm->TargetArmLength -= MaxArmLength * 0.1f;

		if (SpringArm->TargetArmLength <= MinArmLength)
		{
			SpringArm->TargetArmLength = MinArmLength;
			GetWorldTimerManager().ClearTimer(ZoomTimerHandle);
		}
		break;
	}
}

void APlayerCharacter::ZoomOut()
{
	if (!PlayerController) return;

	switch (PlayerController->GetPerspective())
	{
	case EStateOfViews::FPP:
		// Adjust the camera's FOV value.


		break;
	case EStateOfViews::TDP:
	case EStateOfViews::TPP:
		// Adjust SpringArm's length value.
		SpringArm->TargetArmLength += MaxArmLength * 0.1f;

		if (SpringArm->TargetArmLength >= MaxArmLength)
		{
			SpringArm->TargetArmLength = MaxArmLength;
			GetWorldTimerManager().ClearTimer(ZoomTimerHandle);
		}
		break;
	}
}

void APlayerCharacter::StartZoom()
{
	if (!PlayerController) return;

	bIsZoom = true;

	switch (PlayerController->GetPerspective())
	{
	case EStateOfViews::FPP:

		break;
	case EStateOfViews::TDP:

		break;
	case EStateOfViews::TPP:
		GetWorldTimerManager().SetTimer(AimingTimerHandle, this, &APlayerCharacter::Aiming, 0.001f, true);
		GetWorldTimerManager().SetTimer(ZoomTimerHandle, this, &APlayerCharacter::ZoomIn, 0.01f, true);
		break;
	}
}

void APlayerCharacter::StopZoom()
{
	if (!PlayerController) return;

	bIsZoom = false;
	GetWorldTimerManager().ClearTimer(AimingTimerHandle);

	LookYaw = 0.0f;
	LookPitch = 0.0f;

	switch (PlayerController->GetPerspective())
	{
	case EStateOfViews::FPP:

		break;
	case EStateOfViews::TDP:

		break;
	case EStateOfViews::TPP:
		GetWorldTimerManager().SetTimer(ZoomTimerHandle, this, &APlayerCharacter::ZoomOut, 0.01f, true);
		break;
	}
}

void APlayerCharacter::Aiming()
{
	AGun* Gun = Cast<AGun>(EquipWeapon);
	if (!Gun) return;

	FVector HitLocation = DrawCameraLineTrace();
	FVector HitDirection = (HitLocation - GetActorLocation());
	FRotator GoalRotation = UKismetMathLibrary::MakeRotFromX(HitDirection.GetSafeNormal());
	FVector GoalLocation = GetActorLocation() + GoalRotation.Vector() * 1000.0f;

	DrawDebugLine(GetWorld(), GetActorLocation(), GoalLocation, FColor::Blue, false, 0, 0, 10);
	DrawDebugLine(GetWorld(), GetActorLocation(), HitLocation, FColor::Green, false, 0, 0, 10);

	// Set character's LookYaw, LookPitch
	LookYaw = UKismetMathLibrary::ClampAngle(GoalRotation.Yaw - GetActorRotation().Yaw, -90.0f, 90.0f);
	LookPitch = UKismetMathLibrary::ClampAngle(GoalRotation.Pitch - GetActorRotation().Pitch, -90.0f, 90.0f);
}

FVector APlayerCharacter::DrawCameraLineTrace()
{
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);

	FVector Start;
	FVector End;

	Start = CameraManager->GetCameraLocation();
	End = CameraManager->GetCameraLocation() + UKismetMathLibrary::GetForwardVector(CameraManager->GetCameraRotation()) * 50000.0f;

	TArray<AActor*> IgnoreActor;
	FHitResult Hit;

	bool IsHit = UKismetSystemLibrary::LineTraceSingle(
		GetWorld(), 
		Start, 
		End, 
		ETraceTypeQuery::TraceTypeQuery1, 
		true, 
		IgnoreActor, 
		EDrawDebugTrace::None, 
		Hit, 
		true
	);

	if (IsHit)
	{
		return Hit.ImpactPoint;
	}
	else
	{
		return End;
	}
}

void APlayerCharacter::OnEquipEnded()
{
	ActionState = EActionState::EAS_Idle;

	PlayerController->SwitchPlayerMenu();

	EquipWeapon->SaveDodgeMontages(DodgeMontages);
}

void APlayerCharacter::OnUnequipEnded()
{
	if (ActionState == EActionState::EAS_Swapping)
	{
		ActionState = EActionState::EAS_Idle;
		// 무기 교체를 하는 경우
		WeaponComponent->EquipOtherWeapon();
	}
	else
	{
		ActionState = EActionState::EAS_Idle;
		// 무기 교체가 아닐 경우
		PlayerController->SwitchPlayerMenu();
	}

	DodgeMontages.Empty();
}

void APlayerCharacter::Dodge()
{
	UWielderAnimInstance* WielderAnim = Cast<UWielderAnimInstance>(GetMesh()->GetAnimInstance());	
	ensureMsgf(WielderAnim, TEXT("PlayerCharacter :: There is no Wielder AnimInstance for SkeletalMesh."));

	EActionState SpecificAction = EActionState::EAS_Dodging;
	bool CanTakeAction = CheckTakeAction(SpecificAction, false);
	if (!CanTakeAction) return;

	// Change ActionState to Dodging.
	ActionState = EActionState::EAS_Dodging;
	StatComponent->SetDamagableType(EDamagableType::EDT_EVADING);

	float Direction = WielderAnim->GetDirection();
	UAnimMontage* DodgeMontage = nullptr;

	// 나누기 위한 기준 각도
	float AngleDivider = 22.5f;
	bool IsNegative = Direction < 0 ? true : false;

	// 몫
	int Quotient = (IsNegative ? -Direction / AngleDivider : Direction / AngleDivider);

	switch (Quotient)
	{
	case 0:
		// Direction is forward.
		DodgeMontage = *DodgeMontages.Find(EDirection::ED_Forward);
		break;
	case 1:
	case 2:
		// Direction is Left Forward or Right Forward
		DodgeMontage = IsNegative ? *DodgeMontages.Find(EDirection::ED_LeftForward) : 
			*DodgeMontages.Find(EDirection::ED_RightForward);
		break;
	case 3:
	case 4:
		// Direction is Left or Right
		DodgeMontage = IsNegative ? *DodgeMontages.Find(EDirection::ED_Left) :
			*DodgeMontages.Find(EDirection::ED_Right);
		break;
	case 5:
	case 6:
		// Direction is Left Backward or Right Backward
		DodgeMontage = IsNegative ? *DodgeMontages.Find(EDirection::ED_LeftBackward) :
			*DodgeMontages.Find(EDirection::ED_RightBackward);
		break;
	case 7:
	case 8:
		// Direction is Left Forward or Right Forward
		DodgeMontage = *DodgeMontages.Find(EDirection::ED_Backward);
		break;
	default:
		break;
	}

	bool bPlayedSuccessfullly = false;
	const float MontageLength = WielderAnim->Montage_Play(DodgeMontage);

	bPlayedSuccessfullly = (MontageLength > 0.f);

	if (bPlayedSuccessfullly)
	{
		FOnMontageEnded DodgeEndDelegate;
		DodgeEndDelegate.BindUObject(this, &APlayerCharacter::OnDodgeEnded);

		WielderAnim->Montage_SetEndDelegate(DodgeEndDelegate, DodgeMontage);
	}
}

void APlayerCharacter::OnDodgeEnded(UAnimMontage* Montage, bool bInterrupted)
{
	ActionState = EActionState::EAS_Idle;
	StatComponent->SetDamagableType(EDamagableType::EDT_VULNERABLE);
}

void APlayerCharacter::TryLockTarget()
{
	if (bIsTargetLocked)
	{ 
		GetWorldTimerManager().ClearTimer(LockOnTimer);

		AWielder* Wielder = Cast<AWielder>(LockOnTarget);
		if (Wielder)
		{
			Wielder->SetVisibleLockOnImage(false);
		}

		bIsTargetLocked = false;
		LockOnTarget = nullptr;
		LockChangeElapsedTime = 0.0f;
		bIsLockChanged = false;
		return; 
	}

	FVector Start = GetActorLocation();
	FVector End = GetActorLocation() + UKismetMathLibrary::GetForwardVector(GetActorRotation()) * 1500.0f;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	TEnumAsByte<EObjectTypeQuery> Enemy = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel4);
	ObjectTypes.Add(Enemy);

	TArray<AActor*> IgnoreActors;
	TArray<FHitResult> Hits;

	bool IsHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetWorld(),
		Start,
		End,
		200.0f,
		ObjectTypes,
		false,
		IgnoreActors,
		EDrawDebugTrace::None,
		Hits,
		true
	);

	if (IsHit)
	{
		int Index = 1;

		for (FHitResult& Hit : Hits)
		{
			if (Hit.GetActor())
			{
				LockOnTarget = Hit.GetActor();
				bIsTargetLocked = true;

				AWielder* Wielder = Cast<AWielder>(LockOnTarget);
				if (Wielder)
				{
					Wielder->SetVisibleLockOnImage(true);
				}

				// Set Timer Lock Target
				GetWorldTimerManager().SetTimer(LockOnTimer, this, &APlayerCharacter::LockTarget, 0.001f, true);
				return;
			}
		}
	}
}

void APlayerCharacter::LockTarget()
{
	if (bIsTargetLocked && IsValid(LockOnTarget))
	{
		FVector Start = GetActorLocation();
		FVector End = LockOnTarget->GetActorLocation();

		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(Start, End);
		LookAtRotation.Pitch -= 20.0f;
		GetController()->SetControlRotation(LookAtRotation);

		if (bIsLockChanged)
		{
			LockChangeElapsedTime += 0.001f;
			if (LockChangeDuration < LockChangeElapsedTime)
			{
				LockChangeElapsedTime = 0.0f;
				bIsLockChanged = false;
			}
		}

		return;
	}

	if (!IsValid(LockOnTarget))
	{
		bIsTargetLocked = false;
		LockOnTarget = nullptr;
		LockChangeElapsedTime = 0.0f;
		bIsLockChanged = false;

		GetWorldTimerManager().ClearTimer(LockOnTimer);
	}
}

void APlayerCharacter::FindNearbyLockTarget(float DeltaYaw, float DeltaPitch)
{
	if (bIsLockChanged) return;

	FVector Start = GetActorLocation();
	FRotator NewRotation = GetActorRotation() + FRotator(DeltaPitch * 7.0f, DeltaYaw * 7.0f, 0.0f);

	FVector End = GetActorLocation() + UKismetMathLibrary::GetForwardVector(NewRotation) * 1500.0f;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	TEnumAsByte<EObjectTypeQuery> Enemy = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel4);
	ObjectTypes.Add(Enemy);

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(LockOnTarget);
	TArray<FHitResult> Hits;

	bool IsHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetWorld(),
		Start,
		End,
		200.0f,
		ObjectTypes,
		false,
		IgnoreActors,
		EDrawDebugTrace::None,
		Hits,
		true
	);

	if (IsHit)
	{
		int Index = 1;

		for (FHitResult& Hit : Hits)
		{
			if (Hit.GetActor())
			{
				AWielder* Wielder = Cast<AWielder>(LockOnTarget);
				if (Wielder)
				{
					Wielder->SetVisibleLockOnImage(false);
				}

				LockOnTarget = Hit.GetActor();

				Wielder = Cast<AWielder>(LockOnTarget);
				if (Wielder)
				{
					Wielder->SetVisibleLockOnImage(true);
				}

				bIsLockChanged = true;
				DrawDebugString(GetWorld(), FVector(0.0f, 0.0f, 150.0f), FString("Lock On!"), LockOnTarget, FColor::Red, 1.0f);
				return;
			}
		}
	}
}

#pragma region Movement_State

void APlayerCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	ACharacter::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	if (PrevMovementMode != EMovementMode::MOVE_Falling) return;
	
	if (GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Walking)
	{
		// Jump Ended.
		if(IsValid(EquipWeapon) && EquipWeapon->IsAttacking())
			WeaponComponent->JumpAttackLanding();
		
		MovementState = EMovementState::EMS_Idle;
	}
}
#pragma endregion