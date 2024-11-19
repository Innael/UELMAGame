// LeaveMeAlone Game by Netologiya. All RightsReserved.


#include "LevelActors/Damage/LMABearTrap.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Player/LMADefaultCharacter.h"

ALMABearTrap::ALMABearTrap()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	SphereComponent->SetSphereRadius(SphereRadius);
	SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	SetRootComponent(SphereComponent);

	StaticMeshOne = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	StaticMeshOne->SetupAttachment(SphereComponent);
	StaticMeshOne->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	StaticMeshOne->SetRelativeLocation(FVector(0.0f, 0.0f, -4.0f));

	StaticMeshTwo = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh2");
	StaticMeshTwo->SetupAttachment(SphereComponent);
	StaticMeshTwo->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	StaticMeshTwo->SetRelativeLocation(FVector(0.0f, 0.0f, -4.0f));
	StaticMeshTwo->SetVisibility(false, false);
}

// Called when the game starts or when spawned
void ALMABearTrap::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALMABearTrap::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (!Worked)
	{
		Super::NotifyActorBeginOverlap(OtherActor);
		const auto Character = Cast<ALMADefaultCharacter>(OtherActor);
		if (Character)
		{			
			UGameplayStatics::ApplyRadialDamage(GetWorld(), Damage, GetActorLocation(), SphereRadius, nullptr, {}, this, nullptr, true);
			SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			StaticMeshOne->SetVisibility(false, false);
			StaticMeshTwo->SetVisibility(true, false);
			const FVector SoundLoc = StaticMeshOne->GetComponentLocation();
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), TrapWave, SoundLoc);
			Worked = true;
		}
	}
}

