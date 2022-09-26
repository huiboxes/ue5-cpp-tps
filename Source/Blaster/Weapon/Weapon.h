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
class BLASTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class USphereComponent* AreaSphere;

	UPROPERTY(VisibleAnywhere)
	EWeaponState WeaponState;

public:	
	

};
