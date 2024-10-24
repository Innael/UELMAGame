// LeaveMeAlone Game by Netologiya. All RightsReserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LMAWeaponComponent.generated.h"

class ALMABaseWeapon;
class UAnimMontage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FShowCurrentAmmo, int32, CurrAmmo);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LEAVEMEALONE_API ULMAWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULMAWeaponComponent();

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FShowCurrentAmmo ShowCurrentAmmo;

	void Fire();

	UFUNCTION(BlueprintCallable)
	void FireOff();

	void Reload();

	void ChangeWeaponMode(int32 Value);

protected:
	// Called when the game starts

	UPROPERTY()
	ALMABaseWeapon* Weapon = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<ALMABaseWeapon> WeaponClass;

	UPROPERTY(EditDefaultsOnly)
	FName WeaponSocketName = "r_Weapon_Socket";

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	UAnimMontage* ReloadMontage;

	virtual void BeginPlay() override;


private:
	

	bool AnimReloading = false;

	int32 CurrAmmo = 30;

	void SpawnWeapon();

	void InitAnimNotify();

	void OnNotifyReloadFinished(USkeletalMeshComponent* SkeletalMesh);

	bool CanReload() const;

	void ReloadMechanics();

	void ForcedRecharge();

	void ShowAmmo();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
