// LeaveMeAlone Game by Netologiya. All RightsReserved.


#include "Player/LMADefaultCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/DecalComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/LMAHealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/Engine.h"
#include "LMAWeaponComponent.h"


// Sets default values
ALMADefaultCharacter::ALMADefaultCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->SetUsingAbsoluteRotation(true);
	SpringArmComponent->TargetArmLength = ArmLength;
	SpringArmComponent->SetRelativeRotation(FRotator(YRotation, 0.0f, 0.0f));
	SpringArmComponent->bDoCollisionTest = false;
	SpringArmComponent->bEnableCameraLag = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->SetFieldOfView(FOV);
	CameraComponent->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	HealthComponent = CreateDefaultSubobject<ULMAHealthComponent>("HealthComponent");

	WeaponComponent = CreateDefaultSubobject<ULMAWeaponComponent>("Weapon");
}

// Called when the game starts or when spawned
void ALMADefaultCharacter::BeginPlay()
{
	Super::BeginPlay();

	if(CursorMaterial)
	{
		CurrentCursor = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), CursorMaterial, CursorSize, FVector(0));
	}
	
	OnHealthChanged(HealthComponent->GetHealth());
	HealthComponent->OnDeath.AddUObject(this, &ALMADefaultCharacter::OnDeath);
	HealthComponent->OnDeath.AddUObject(this, &ALMADefaultCharacter::StopUnnecessaryFire);
	HealthComponent->OnHealthChanged.AddUObject(this, &ALMADefaultCharacter::OnHealthChanged);
}

// Called every frame
void ALMADefaultCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!(HealthComponent->IsDead()))
	{
		RotationPlayerOnCursor();
	}

	if (Stamina > 10)
	{		
		if (SprintCheck)
		{
			Stamina = FMath::Clamp(Stamina - 1, 0.0f, MaxStamina);
		}		
	}
	else
	{
		if (SprintCheck)
		{
			SprintCheck = false;
			GetCharacterMovement()->MaxWalkSpeed = 300;
		}
	}
	

	if (!SprintCheck && Stamina < MaxStamina)
	{
		Stamina += 1;
	}
}


// Called to bind functionality to input
void ALMADefaultCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ALMADefaultCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ALMADefaultCharacter::MoveRight);
	PlayerInputComponent->BindAction("ZoomPlus", IE_Pressed, this, &ALMADefaultCharacter::ZoomPlus);
	PlayerInputComponent->BindAction("ZoomMinus", IE_Pressed, this, &ALMADefaultCharacter::ZoomMinus);
	PlayerInputComponent->BindAction("SprintStart", IE_Pressed, this, &ALMADefaultCharacter::SprintStart);
	PlayerInputComponent->BindAction("SprintStop", IE_Released, this, &ALMADefaultCharacter::SprintStop);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, WeaponComponent, &ULMAWeaponComponent::Fire);
	PlayerInputComponent->BindAction("Fire", IE_Released, WeaponComponent, &ULMAWeaponComponent::FireOff);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, WeaponComponent, &ULMAWeaponComponent::Reload);
	PlayerInputComponent->BindAction("FireModeMinus", IE_Pressed, this, &ALMADefaultCharacter::FireModeMinus);
	PlayerInputComponent->BindAction("FireModePlus", IE_Pressed, this, &ALMADefaultCharacter::FireModePlus);
	PlayerInputComponent->BindAction("Light", IE_Pressed, this, &ALMADefaultCharacter::SwitchLight);
}

void ALMADefaultCharacter::MoveForward(float Value)
{	
	if (SprintCheck && Value < 0)
	{
		SprintCheck = false;
		GetCharacterMovement()->MaxWalkSpeed = 100;
	}

	AddMovementInput(GetActorForwardVector(), Value);
}

void ALMADefaultCharacter::MoveRight(float Value)
{
	if (SprintCheck && Value != 0)
	{
		SprintCheck = false;
		GetCharacterMovement()->MaxWalkSpeed = 300;
	}
	AddMovementInput(GetActorRightVector(), Value);
}

void ALMADefaultCharacter::ZoomPlus()
{	
		if (ZoomMode <= 0)
			ZoomMode -= ZoomSmoothness;
		else
			ZoomMode = 0;

		ArmLength += ZoomMode;
		if (ArmLength <= MinArmLength)
		{
			ArmLength = MinArmLength;
		}
		SpringArmComponent->TargetArmLength = ArmLength;	
}

void ALMADefaultCharacter::ZoomMinus()
{	
		if (ZoomMode >= 0)
			ZoomMode += ZoomSmoothness;
		else
			ZoomMode = 0;

		ArmLength += ZoomMode;

		if (ArmLength >= MaxArmLength)
		{
			ArmLength = MaxArmLength;
		}
		SpringArmComponent->TargetArmLength = ArmLength;
}

void ALMADefaultCharacter::OnDeath()
{
	CurrentCursor->DestroyRenderState_Concurrent();

	PlayAnimMontage(DeathMontage);
	GetCharacterMovement()->DisableMovement();
	SetLifeSpan(5.0f);

	if (Controller)
	{
		Controller->ChangeState(NAME_Spectating);
	}
}


void ALMADefaultCharacter::StopUnnecessaryFire()
{
	WeaponComponent->FireOff();
}


void ALMADefaultCharacter::RotationPlayerOnCursor()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		FHitResult ResultHit;
		PC->GetHitResultUnderCursor(ECC_GameTraceChannel1, true, ResultHit);
		float FindRotatorResultYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;
		SetActorRotation(FQuat(FRotator(0.0f, FindRotatorResultYaw, 0.0f)));
		if (CurrentCursor)
		{
			CurrentCursor->SetWorldLocation(ResultHit.Location);
		}
	}
}

void ALMADefaultCharacter::OnHealthChanged(float NewHealth)
{
	
}


void ALMADefaultCharacter::SprintStart() 
{
	if (Stamina > 10)
	{
		SprintCheck = true;
		GetCharacterMovement()->MaxWalkSpeed = 600;
	}
	else
		SprintCheck = false;

}

void ALMADefaultCharacter::SprintStop()
{
	SprintCheck = false;
	GetCharacterMovement()->MaxWalkSpeed = 300;
}

void ALMADefaultCharacter::FireModeMinus()
{
	WeaponComponent->ChangeWeaponMode(-1);
}

void ALMADefaultCharacter::FireModePlus() 
{
	WeaponComponent -> ChangeWeaponMode(1);
}

void ALMADefaultCharacter::SwitchLight()
{
	WeaponComponent->LightOnOff();
}