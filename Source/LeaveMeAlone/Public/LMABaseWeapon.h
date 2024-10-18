// LeaveMeAlone Game by Netologiya. All RightsReserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LMABaseWeapon.generated.h"

class USkeletalMeshComponent;

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

	void Fire();

	void FireOff();

	void ChangeClip();

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

	float ShootPeriod = 0.3f;

	virtual void BeginPlay() override;

	void Shoot();

	void DecrementBullets();

	bool IsCurrentClipEmpty() const;
	

private:
	FAmmoWeapon CurrentAmmoWeapon;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
