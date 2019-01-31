// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "YorHighnessTestCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Classes/GameFramework/Actor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "public/PickUpObject.h"
#include "Classes/GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"

//////////////////////////////////////////////////////////////////////////
// AYorHighnessTestCharacter

AYorHighnessTestCharacter::AYorHighnessTestCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void AYorHighnessTestCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AYorHighnessTestCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AYorHighnessTestCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AYorHighnessTestCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AYorHighnessTestCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AYorHighnessTestCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AYorHighnessTestCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AYorHighnessTestCharacter::OnResetVR);

	PlayerInputComponent->BindAction("prendi", IE_Pressed, this, &AYorHighnessTestCharacter::PickUp);
}


void AYorHighnessTestCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AYorHighnessTestCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AYorHighnessTestCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void AYorHighnessTestCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AYorHighnessTestCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AYorHighnessTestCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AYorHighnessTestCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AYorHighnessTestCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}

FHitResult AYorHighnessTestCharacter::FindObjectUnderCursor()//TODO proteggere HitresultUnderCursor.GetActior()
{
	FHitResult HitResultUnderCursor;

	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	PlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_WorldStatic, false, HitResultUnderCursor);

	
		return HitResultUnderCursor;
	
			
}

void AYorHighnessTestCharacter::PickUp()
{
	auto MyController = GetWorld()->GetFirstPlayerController();
	APickUpObject* Actor = Cast<APickUpObject>(FindObjectUnderCursor().GetActor());
	if (Actor != nullptr)
	{
		Actor->Destroy();

		ObjectHeld = Actor;

		if (MyController)
			MyController->CurrentMouseCursor = EMouseCursor::Crosshairs;
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("no controller!!!!!"))
		}
	}
}

void AYorHighnessTestCharacter::ChangeCursor()
{

	auto MyController = GetWorld()->GetFirstPlayerController();
	if (MyController)
		MyController->CurrentMouseCursor = EMouseCursor::Default;
}
float AYorHighnessTestCharacter::GetHealth()
{
	return Health;
}

APickUpObject* AYorHighnessTestCharacter::GetObjectHeld()
{
	return ObjectHeld;
}

void AYorHighnessTestCharacter::ResetObjectHeld()
{
	ObjectHeld = nullptr;
}