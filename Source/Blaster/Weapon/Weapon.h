// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

// 武器状态枚举类
// enum 常量为无符号的 8 位整数，它也是蓝图类型
UENUM(BlueprintType) // 让蓝图中也可使用此枚举
enum class EWeaponState : uint8 {
	EWS_Initial UMETA(DisplayName = "Initial State"), // 武器初始状态，未被捡起时
	EWS_Equipped UMETA(DisplayName = "Equipped"), // 武器被捡起，已装备时
	EWS_Drop UMETA(DisplayName = "Drop"), // 武器被丢弃时

	EWS_MAX UMETA(DisplayName = "DefaultMAX") // 该 enum 的最大值，可通过它知道有多少个常量
};


UCLASS()
class BLASTER_API AWeapon : public AActor {
	GENERATED_BODY()

public:
	AWeapon();
	// 设置是否显示拾取组件函数 
	void ShowPickupWidget(bool bShowWidget);
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(
			UPrimitiveComponent* OverlappedComponent, // 重叠的组件
			AActor* OtherActor, // 进入重叠区域的 Actor
			UPrimitiveComponent* OtherComp, // 其它组件
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult
		);

	UFUNCTION()
	void OnSphereEndOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex
		);


private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class USphereComponent* AreaSphere;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState WeaponState;

	UFUNCTION()
	void OnRep_WeaponState();

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UWidgetComponent* PickupWidget;

public:
	void SetWeaponState(EWeaponState State);
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; };
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; };
	
};
