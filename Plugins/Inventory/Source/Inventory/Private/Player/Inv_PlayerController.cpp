#include "Player/Inv_PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ToolContextInterfaces.h"
#include "Engine/LocalPlayer.h"
#include "Items/Components/Inv_ItemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/Input/SVirtualJoystick.h"
#include "Widgets/HUD/Inv_HUDWidget.h"

AInv_PlayerController::AInv_PlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	TraceLength = 500.0;
	ItemTraceChannel = ECC_GameTraceChannel1;
}

void AInv_PlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TraceForItem();
}


void AInv_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (SVirtualJoystick::ShouldDisplayTouchInterface() && IsLocalPlayerController())
	{
		// 可选：移动端触控界面
	}

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Subsystem 获取成功！"));

		/*让两个 IMC 都生效，只要优先级不冲突即可*/
		if (OLDIMC)
		{
			Subsystem->AddMappingContext(OLDIMC, 0);
			UE_LOG(LogTemp, Warning, TEXT("添加 OLDIMC (优先级 0)"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("OLDIMC 是空的！"));
		}

		if (DefaultIMC)
		{
			Subsystem->AddMappingContext(DefaultIMC, 1);
			UE_LOG(LogTemp, Warning, TEXT("添加 DefaultIMC (优先级 1)"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("DefaultIMC 是空的！"));
		}

		
		Subsystem->RequestRebuildControlMappings();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Subsystem 获取失败！GetLocalPlayer() 可能是 NULL"));
	}

	CreateHUDWidget();
}


void AInv_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(PrimaryInteractAction, ETriggerEvent::Started, this, &AInv_PlayerController::PrimaryInteract);
}


void AInv_PlayerController::PrimaryInteract()
{
	UE_LOG(LogTemp, Warning, TEXT("PrimaryInteract triggered!"));
}

void AInv_PlayerController::CreateHUDWidget()
{
	if (!IsLocalController()) return;
	HUDWidget = CreateWidget<UInv_HUDWidget>(this, HUDWidgetClass);
	if (IsValid(HUDWidget))
	{
		HUDWidget->AddToViewport();
	}
	
}

void AInv_PlayerController::TraceForItem()
{
	if (!IsValid(GEngine)||!IsValid(GEngine->GameViewport)) return;
	FVector2d ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);
	const FVector2d ViewportCenter = ViewportSize / 2.f;
	FVector TraceStart;
	FVector Forward;
	if (!UGameplayStatics::DeprojectScreenToWorld(this,ViewportCenter,TraceStart,Forward)) return;

	const FVector TraceEnd = TraceStart + Forward * TraceLength;
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult,TraceStart,TraceEnd,ItemTraceChannel);
	
	LastActor = ThisActor;
	ThisActor = HitResult.GetActor();

	if (!ThisActor.IsValid())
	{
		if (IsValid(HUDWidget)) HUDWidget->HidePickupMessage();
	}
	
	if (ThisActor == LastActor) return;

	if (ThisActor.IsValid())
	{
		UInv_ItemComponent* ItemComponent = ThisActor->FindComponentByClass<UInv_ItemComponent>();
		if (!IsValid(ItemComponent)) return;

		if (IsValid(HUDWidget))
		{
			HUDWidget->ShowPickupMessage(ItemComponent->GetPickupMessage());
		}
	}

	
	
	if (ThisActor.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Started tracing a new actor"));
	}
	if (LastActor.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Stopped tracing last actor"));
	}
	
	
	
}
