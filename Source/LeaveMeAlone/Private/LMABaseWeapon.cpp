// LeaveMeAlone Game by Netologiya. All RightsReserved.


#include "LMABaseWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LogWeapon, All, All);

// Sets default values
ALMABaseWeapon::ALMABaseWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SkMeshWeaponComponent = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	SetRootComponent(SkMeshWeaponComponent);
}

// Called when the game starts or when spawned
void ALMABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentAmmoWeapon = DefaultAmmoWeapon;
	GetWorldTimerManager().SetTimer(ShootPeriodTimer, this, &ALMABaseWeapon::Shoot, ShootPeriod, true);
	GetWorldTimerManager().PauseTimer(ShootPeriodTimer);
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
	if (IsCurrentClipEmpty() || ReloadStatus)
		return;
	const FTransform SocketTransform = SkMeshWeaponComponent->GetSocketTransform(MuzzleSocketName);
	const FVector TraceStart = SocketTransform.GetLocation();
	const FVector ShootDirection = SocketTransform.GetRotation().GetForwardVector();
	const FVector TraceEnd = TraceStart + ShootDirection * TraceDistance;
	//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Black, false, 1.0f, 0, 2.0f);	
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility);
	FVector TracerEnd = TraceEnd;

	if (HitResult.bBlockingHit)
	{
		TracerEnd = HitResult.ImpactPoint;
	}

	SpawnTrace(TraceStart, TracerEnd);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShootWave, TraceStart);

	DecrementBullets();
}

void ALMABaseWeapon::ChangeClip()
{
	CurrentAmmoWeapon.Bullets = DefaultAmmoWeapon.Bullets;	
}

bool ALMABaseWeapon::IsCurrentClipEmpty() const
{
	return CurrentAmmoWeapon.Bullets == 0;
}

void ALMABaseWeapon::DecrementBullets()
{
	CurrentAmmoWeapon.Bullets--;
	AmmoChange.Broadcast();
	if (IsCurrentClipEmpty())
	{
		ForcedRecharge.Broadcast();
	}	
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
	const auto TraceFX = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TraceEffect, TraceStart);
	if (TraceFX)
	{
		TraceFX->SetNiagaraVariableVec3(TraceName, TraceEnd);
	}
}