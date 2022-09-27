// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Blaster/Character/BlasterCharacter.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false; // 关闭每帧调用
	bReplicates = true; // 开启同步（属性复制）

	// 创建武器的 SkeletalMesh
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	// 设置根组件
	SetRootComponent(WeaponMesh);
	
	// 所有通道都设置碰撞响应 Block
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	// 忽略对 Pawn 的响应通道
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	// 关闭碰撞检测，武器没有处于被拾取状态时再设置为有
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 创建碰撞检测球，一旦它与角色重叠，就能捡起武器
	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	// 附加到根组件
	AreaSphere->SetupAttachment(RootComponent);
	// 忽略所有通道的响应
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	// 设置无碰撞
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 服务器将负责所有武器对象
	if (HasAuthority()) { // 如果拥有网络权限（在服务器上）
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); // 启用碰撞检测
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap); // 对 Pawn 启用重叠检测

		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereOverlap);
	}

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(RootComponent);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (PickupWidget) {
		PickupWidget->SetVisibility(false);
	}
	
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if (BlasterCharacter && PickupWidget) {
		PickupWidget->SetVisibility(true);
	}
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

