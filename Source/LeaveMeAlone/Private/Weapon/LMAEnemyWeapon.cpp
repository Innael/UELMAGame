// LeaveMeAlone Game by Netologiya. All RightsReserved.


#include "Weapon/LMAEnemyWeapon.h"

// Sets default values
ALMAEnemyWeapon::ALMAEnemyWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	WeaponComponent = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	SetRootComponent(WeaponComponent);

}

// Called when the game starts or when spawned
void ALMAEnemyWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

