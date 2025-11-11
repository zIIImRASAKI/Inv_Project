


#include "InventoryManagment/components/Inv_InventoryComponent.h"

#include "Widgets/Inventory/InventoryBase/Inv_InventoryBase.h"



UInv_InventoryComponent::UInv_InventoryComponent()
{

	PrimaryComponentTick.bCanEverTick = false;

	
}



void UInv_InventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	ConstructInventory();
	
}

void UInv_InventoryComponent::ConstructInventory()
{
	OwningController = Cast<APlayerController>(GetOwner());
	checkf(OwningController.IsValid(),TEXT("Inventory Component should have a player Controller as owner."));
	if (!OwningController->IsLocalController())return;

	InventoryMenu = CreateWidget<UInv_InventoryBase>(OwningController.Get(),InventoryMenuClass);
	InventoryMenu->AddToViewport();
}


