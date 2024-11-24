// LeaveMeAlone Game by Netologiya. All RightsReserved.


#include "LMABaseWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SpotLightComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogWeapon, All, All);

// Sets default values
ALMABaseWeapon::ALMABaseWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SkMeshWeaponComponent = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	SetRootComponent(SkMeshWeaponComponent);

	RightLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("RightLight"));
	LeftLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("LeftLight"));

	RightLight->SetIntensity(5000.0f);				  // яркость
	RightLight->SetLightColor(FColor::White);	  // ÷вет света
	RightLight->SetOuterConeAngle(45.0f);			  // ”гол внешнего конуса
	RightLight->SetInnerConeAngle(30.0f);

	LeftLight->SetIntensity(5000.0f);		  
	LeftLight->SetLightColor(FColor::White); 
	LeftLight->SetOuterConeAngle(45.0f);	  
	LeftLight->SetInnerConeAngle(30.0f);
}

// Called when the game starts or when spawned
void ALMABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentAmmoWeapon = DefaultAmmoWeapon;
	GetWorldTimerManager().SetTimer(ShootPeriodTimer, this, &ALMABaseWeapon::Shoot, ShootPeriod, true);
	GetWorldTimerManager().PauseTimer(ShootPeriodTimer);

	if (SkMeshWeaponComponent)
	{	
		
		RightLight->AttachToComponent(SkMeshWeaponComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, RightLightSocketName);
		LeftLight->AttachToComponent(SkMeshWeaponComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, LeftLightSocketName);

		RightLight->SetVisibility(false);
		LeftLight->SetVisibility(false);
	}
}

// Called every frame
void ALMABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALMABaseWeapon::Fire()
{	
	GetWorldTimerManager().UnPauseTimer(ShootPeriodTimer);
}

void ALMABaseWeapon::FireOff()
{
	GetWorldTimerManager().PauseTimer(ShootPeriodTimer);
}

void ALMABaseWeapon::Shoot()
{
	if (IsCurrentClipEmpty() || ReloadStatus || CurrentAmmoWeapon.Bullets < CartridgeConsumption)
		return;
	const FTransform SocketTransform = SkMeshWeaponComponent->GetSocketTransform(MuzzleSocketName);
	const FVector TraceStart = SocketTransform.GetLocation();
	const FVector ShootDirection = SocketTransform.GetRotation().GetForwardVector();
	const FVector TraceEnd = TraceStart + ShootDirection * TraceDistance;
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility);
	FVector TracerEnd = TraceEnd;

	if (HitResult.bBlockingHit)
	{
		MakeDamage(HitResult);
		TracerEnd = HitResult.ImpactPoint;
	}

	SpawnTrace(TraceStart, TracerEnd);

	switch (FireMode)
	{
	    case 1:
	    {
		    UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShootWave, TraceStart);
		    break;
	    }
		case 2:
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShootWaveTwo, TraceStart);
			break;
		}
		case 3:
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShootWaveThree, TraceStart);
			break;
		}
	}

	DecrementBullets();
}

void ALMABaseWeapon::ChangeClip()
{
	CurrentAmmoWeapon.Bullets = DefaultAmmoWeapon.Bullets;	
}

bool ALMABaseWeapon::IsCurrentClipEmpty() const
{
	return CurrentAmmoWeapon.Bullets <= 0;
}

void ALMABaseWeapon::DecrementBullets()
{
	CurrentAmmoWeapon.Bullets -= CartridgeConsumption;
	if (IsCurrentClipEmpty())
	{
		ForcedRecharge.Broadcast();
	}
	AmmoChange.Broadcast();
}

bool ALMABaseWeapon::CheckIfTheClipIsFull() 
{
	if (CurrentAmmoWeapon.Bullets == DefaultAmmoWeapon.Bullets)
		return true;
	else
		return false;
}

void ALMABaseWeapon::SpawnTrace(const FVector& TraceStart, const FVector& TraceEnd)
{
	switch (FireMode)
	{
	case 1:
	{
		const auto TraceFX = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TraceEffect, TraceStart);
		if (TraceFX)
		{
			TraceFX->SetNiagaraVariableVec3(TraceName, TraceEnd);
		}
	}
	case 2:
	{
		const auto TraceFX = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TraceEffectTwo, TraceStart);
		if (TraceFX)
		{
			TraceFX->SetNiagaraVariableVec3(TraceName, TraceEnd);
		}
	}
	case 3:
	{
		const auto TraceFX = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TraceEffectThree, TraceStart);
		if (TraceFX)
		{
			TraceFX->SetNiagaraVariableVec3(TraceName, TraceEnd);
		}
	}
	}
}

void ALMABaseWeapon::SetFireMode(int32 Value)
{
	FireMode += Value;
	if (FireMode < 1)
		FireMode = 3;
	else if (FireMode > 3)
		FireMode = 1;	

	switch (FireMode)
	{
	case 1:
		CartridgeConsumption = 1;
		Damage = FMD1;
		break;
	case 2:
		CartridgeConsumption = 3;
		Damage = FMD2;
		break;
	case 3:
		CartridgeConsumption = 6;
		Damage = FMD3;
		break;
	}
		
}

void ALMABaseWeapon::MakeDamage(const FHitResult& HitResult)
{
	const auto Enemy = HitResult.GetActor();
	if (!Enemy)
		return;
	const auto Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!Pawn)
		return;
	const auto Controller = Pawn->GetController<APlayerController>();
	if (!Controller)
		return;
	Enemy->TakeDamage(Damage, FDamageEvent(), Controller, this);
}

void ALMABaseWeapon::ChangeLight() 
{
	CheckLight = !CheckLight;

	RightLight->SetVisibility(CheckLight);
	LeftLight->SetVisibility(CheckLight);
}