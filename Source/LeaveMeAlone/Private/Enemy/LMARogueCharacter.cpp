// LeaveMeAlone Game by Netologiya. All RightsReserved.


#include "Enemy/LMARogueCharacter.h"
#include "Components/LMAHealthComponent.h"

// Sets default values
ALMARogueCharacter::ALMARogueCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	HealthComponent = CreateDefaultSubobject<ULMAHealthComponent>("HealthComponent");
	HealthComponent->MaxHealth = 10;
}

// Called when the game starts or when spawned
void ALMARogueCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALMARogueCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
