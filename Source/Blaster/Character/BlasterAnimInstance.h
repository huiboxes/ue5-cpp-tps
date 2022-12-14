// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Blaster/BlasterTypes/TurnningInPlace.h"
#include "BlasterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UBlasterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	// 游戏开始时被调用，加载虚幻引擎并进行编译时也会被调用
	virtual void NativeInitializeAnimation() override;
	// 每一帧都被调用
	virtual void NativeUpdateAnimation(float DeltaTime) override;

private:
	// 蓝图设为只读
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	class ABlasterCharacter* BlasterCharacter; // 存储角色的变量

	// 速度值
	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float Speed; 

	// 是否在空中
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool bIsInAir; 

	// 是否是加速状态
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool bIsAcceleration; 

	// 是否装备了武器
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool bWeaponEquipped; 

	// 装备着的武器
	class AWeapon* EquippedWeapon;

	// 是否蹲下了
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool bIsCrouched; 

	// 是否在瞄准中
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool bAiming; 

	// 输入方向和相机方向的夹角对应的偏移量
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	float YawOffset;

	// 表示倾斜程度
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	float Lean;

	// 存储上一帧角色 Rotation
	FRotator CharacterRotationLastFrame;
	// 存储当前帧角色 Rotation
	FRotator CharacterRotation;
	// 存储当前帧旋转信息
	FRotator DeltaRotation;

	// 驱动 AimOffset 的 Yaw
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	float AO_Yaw;
	
	// 驱动 AimOffset 的 Pitch
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	float AO_Pitch;

	// 持枪时放置左手的位置
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	FTransform LeftHandTransform;

	// 转向信息
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	ETurnningInPlace TurnningInPlcae;
};
