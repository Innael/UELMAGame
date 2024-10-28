// LeaveMeAlone Game by Netologiya. All RightsReserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LMABaseWeapon.generated.h"

class USkeletalMeshComponent;
class USoundWave;
class UNiagaraSystem;

DECLARE_MULTICAST_DELEGATE(FForcedRecharge)
DECLARE_MULTICAST_DELEGATE(FAmmoChange)


USTRUCT(BlueprintType)
struct FAmmoWeapon
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	int32 Bullets;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	int32 Clips;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	bool Infinite;
};


UCLASS()
class LEAVEMEALONE_API ALMABaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALMABaseWeapon();

	FForcedRecharge ForcedRecharge;
	FAmmoChange AmmoChange;

	void Fire();

	void FireOff();

	void ChangeClip();

	bool CheckIfTheClipIsFull();

	bool ReloadStatus = false;

	FAmmoWeapon GetCurrentAmmoWeapon() const { return CurrentAmmoWeapon; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	int32 FireMode = 1;

	void SetFireMode(int32 Value);	

protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	USkeletalMeshComponent* SkMeshWeaponComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	float TraceDistance = 800.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	FAmmoWeapon DefaultAmmoWeapon{30, 0, true};

	UPROPERTY(EditDefaultsOnly)
	FName MuzzleSocketName = "Muzzle";

	FTimerHandle ShootPeriodTimer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	float ShootPeriod = 0.3f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	USoundWave* ShootWave;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	USoundWave* ShootWaveTwo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	USoundWave* ShootWaveThree;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	UNiagaraSystem* TraceEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	UNiagaraSystem* TraceEffectTwo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	UNiagaraSystem* TraceEffectThree;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	FString TraceName = "Tracer";
	

	virtual void BeginPlay() override;

	void Shoot();

	void DecrementBullets();

	bool IsCurrentClipEmpty() const;	

	FString CurrAmmo = "30";

	int CartridgeConsumption = 1;
	

private:
	FAmmoWeapon CurrentAmmoWeapon;

	void SpawnTrace(const FVector& TraceStart, const FVector& TraceEnd);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
