// LeaveMeAlone Game by Netologiya. All RightsReserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LMAHealthComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnDeath)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateHealthBar, float, Procent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRunLoseMenu);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LEAVEMEALONE_API ULMAHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULMAHealthComponent();

	UFUNCTION(BlueprintCallable)
	float GetHealth() const { return Health; }

	UFUNCTION(BlueprintCallable)
	bool IsDead() const;

	FOnDeath OnDeath;
	FOnHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FRunLoseMenu RunLoseMenu;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FUpdateHealthBar UpdateHealthBar;	

	UFUNCTION(BlueprintCallable)
	bool AddHealth(float NewHealth);

	bool IsHealthFull() const;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageAbsorption = 0.0f;


protected:
	// Called when the game starts


	virtual void BeginPlay() override;

private:
	float Health = 0.0f;
		
	UFUNCTION()
	void OnTakeAnyDamage(
		AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
		
};
