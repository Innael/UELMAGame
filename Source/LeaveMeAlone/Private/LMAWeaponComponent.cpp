// LeaveMeAlone Game by Netologiya. All RightsReserved.


#include "LMAWeaponComponent.h"
#include "GameFramework/Character.h"
#include "Animations/LMAReloadFinishedAnimNotify.h"
#include "LMABaseWeapon.h"

// Sets default values for this component's properties
ULMAWeaponComponent::ULMAWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void ULMAWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	SpawnWeapon();
	InitAnimNotify();	
}


// Called every frame
void ULMAWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void ULMAWeaponComponent::SpawnWeapon()
{
	Weapon = GetWorld()->SpawnActor<ALMABaseWeapon>(WeaponClass);
	if (Weapon)
	{
		const auto Character = Cast<ACharacter>(GetOwner());
		if (Character)
		{
			FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
			Weapon->AttachToComponent(Character->GetMesh(), AttachmentRules, WeaponSocketName);
			Weapon->ForcedRecharge.AddUObject(this, &ULMAWeaponComponent::ForcedRecharge);
			Weapon->AmmoChange.AddUObject(this, &ULMAWeaponComponent::ShowAmmo);
		}
	}
}

void ULMAWeaponComponent::Fire()
{
	if (Weapon && !AnimReloading)
	{
		Weapon->Fire();
	}
}

void ULMAWeaponComponent::FireOff() {
	if (Weapon)
	{
		Weapon->FireOff();
	}
}

void ULMAWeaponComponent::InitAnimNotify()
{
	if (!ReloadMontage)
		return;
	const auto NotifiesEvents = ReloadMontage->Notifies;
	for (auto NotifyEvent : NotifiesEvents)
	{
		auto ReloadFinish = Cast<ULMAReloadFinishedAnimNotify>(NotifyEvent.Notify);
		if (ReloadFinish)
		{
			ReloadFinish->OnNotifyReloadFinished.AddUObject(this, &ULMAWeaponComponent::OnNotifyReloadFinished);			
			break;
		}
	}
}

void ULMAWeaponComponent::OnNotifyReloadFinished(USkeletalMeshComponent* SkeletalMesh)
{
	const auto Character = Cast<ACharacter>(GetOwner());
	if (Character->GetMesh() == SkeletalMesh)
	{
		AnimReloading = false;
		Weapon->ReloadStatus = false;
	}
}

bool ULMAWeaponComponent::CanReload() const
{
	return !AnimReloading;
}

void ULMAWeaponComponent::Reload()
{
	if (Weapon->CheckIfTheClipIsFull() != true)
		ReloadMechanics();
}

void ULMAWeaponComponent::ReloadMechanics()
{
	if (!CanReload())
		return;
	Weapon->ReloadStatus = true;
	Weapon->ChangeClip();	
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	Character->PlayAnimMontage(ReloadMontage);
	AnimReloading = true;
	ShowAmmo();
}

void ULMAWeaponComponent::ForcedRecharge() 
{
	ReloadMechanics();
}

void ULMAWeaponComponent::ShowAmmo()
{
	CurrAmmo = Weapon->GetCurrentAmmoWeapon().Bullets;
	ShowCurrentAmmo.Broadcast(CurrAmmo);
}

void ULMAWeaponComponent::ChangeWeaponMode(int32 Value)
{
	Weapon->SetFireMode(Value);	
	FireModeChanged.Broadcast(Weapon->FireMode);
}

void ULMAWeaponComponent::LightOnOff() 
{
	Weapon->ChangeLight();
}