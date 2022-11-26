// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Blaster/BlasterTypes/TurnningInPlace.h"
#include "BlasterCharacter.generated.h"

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter {
	GENERATED_BODY()

public:
	ABlasterCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 该函数用于获取网络复制的属性
	// 任何打算复制变量的类中，都需要重写它，在它里面注册要复制的变量
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override; // 在组件初始化后自动调用

protected:
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EquipButtonPressed();
	void CrouchButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	void AimOffset(float DeltaTime);
	virtual void Jump() override;

private:
	// 将它归类为相机
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera;

	// 加上 AllowPrivateAccess 后，就可以给私有变量设置 BlueprintReadOnly
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;

	// 存放重叠武器的变量
	// 把重叠武器指定为一个复制变量，当它在服务器上发生变化时，它可以改变其它客户端
	// 当它复制到客户端时，会调用 OnRep_OverlappingWeapon
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* Combat;

	// 在服务器上执行的，按下装备武器键后执行的函数
	UFUNCTION(Server, Reliable) // 声明一个在客户端上调用，但需要在服务器上执行的可靠 RPC
	void ServerEquipButtonPressed();

	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	ETurnningInPlace TurnningInPlace;
	void TurnInPlace(float DeltaTime);

public:

	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; };
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; };
	AWeapon* GetEquippedWeapon();
	FORCEINLINE ETurnningInPlace GetTurnningInPlace() const { return TurnningInPlace; };
};
