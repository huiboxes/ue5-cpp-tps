// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/BlasterComponents/CombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"

ABlasterCharacter::ABlasterCharacter() {
	PrimaryActorTick.bCanEverTick = true;

	// 创建弹簧臂
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	// 将摄像机附加到网格组件
	// 如果附加到胶囊体，在蹲下时胶囊变小则弹簧臂也会移动
	CameraBoom->SetupAttachment(GetMesh());
	// 设置弹簧臂长度
	CameraBoom->TargetArmLength = 600.f;
	// 能动的物体，都会有控制器来进行操控，设置摇臂根据控制器旋转
	CameraBoom->bUsePawnControlRotation = true;


	// 创建相机
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	// 附加到弹簧臂上
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// 因为它附加到 CameraBoom ，所以不需要启用 Pwan 控制
	FollowCamera->bUsePawnControlRotation = false;

	// 让角色独立于控制器旋转
	bUseControllerRotationYaw = false;
	// 角色向加速方向旋转，RotationRate作为旋转变化率
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// 创建一个 OverheadWidget 实例对象
	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	// 将它附加到角色中
	OverheadWidget->SetupAttachment(RootComponent);
	
	// 组件设置为 Replicated ，不需要注册
	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	// 设置胶囊体忽略摄像机的碰撞
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	// 设置网格体忽略摄像机的碰撞
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 注册要复制的重叠武器变量（只复制给与武器范围重叠的人物的客户端）
	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly);
	//DOREPLIFETIME(ABlasterCharacter, OverlappingWeapon);
}

// 组件初始化后将本类中 Combat 实例中绑定的 ABlasterCharacter 实例设置为当前实例
void ABlasterCharacter::PostInitializeComponents() {
	Super::PostInitializeComponents();
	if (Combat) {
		Combat->Character = this;
	}
}

void ABlasterCharacter::BeginPlay() {
	Super::BeginPlay();

}

void ABlasterCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	AimOffset(DeltaTime);
}

void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 绑定父类的 Jump 方法
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ThisClass::EquipButtonPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ThisClass::CrouchButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ThisClass::AimButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ThisClass::AimButtonReleased);

	// 绑定当前类的方法
	PlayerInputComponent->BindAxis("MoveForward", this, &ThisClass::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ThisClass::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ThisClass::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ThisClass::LookUp);
}

void ABlasterCharacter::MoveForward(float Value) {
	// Controller 是继承的父类的成员变量
	if (Controller != nullptr && Value != 0.f) {
		// 不使用 GetActorForwardVector 是因为它获取的向前向量是根分量（胶囊）的前向量
		// 使用控制器的前进方向，而不是角色的前进方向
		// FRotator 是存储三位变量的结构体，Pitch、Yaw、Roll 分别代表 Y、Z、X 
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		// 根据 Z 轴获得旋转矩阵，得到他 X 方向的单位向量
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		// 指定运动的方向和运动的距离
		AddMovementInput(Direction, Value);
	}
}

void ABlasterCharacter::MoveRight(float Value) {
	if (Controller != nullptr && Value != 0.f) {
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		// 与前后移动的区别是他获取的是 Y 方向的单位向量
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(Direction, Value);
	}
}

void ABlasterCharacter::Turn(float Value) {
	// 左右移动视角，传入 Z 轴偏移	
	AddControllerYawInput(Value);
}

void ABlasterCharacter::LookUp(float Value) {
	// 上下移动视角，传入 Y 轴偏移
	AddControllerPitchInput(Value);
}

void ABlasterCharacter::EquipButtonPressed() {
	// 装备武器应该由服务器负责
	if (Combat) {
		if (HasAuthority()) {
			Combat->EquipWeapon(OverlappingWeapon);
		} else { // 如果是
			ServerEquipButtonPressed();
		}
	}

}



// 得在实现的函数名后加上 _Implementation ，UE 会帮忙创建它实际的函数定义的
void ABlasterCharacter::ServerEquipButtonPressed_Implementation() {
	if (Combat) {
		Combat->EquipWeapon(OverlappingWeapon);
	}
}

void ABlasterCharacter::CrouchButtonPressed() {
	if (bIsCrouched) {
		UnCrouch();
	} else {
		Crouch();
	}
	
}

void ABlasterCharacter::AimButtonPressed() {
	if (Combat) {
		Combat->SetAiming(true);
	}
}

void ABlasterCharacter::AimButtonReleased() {
	if (Combat) {
		Combat->SetAiming(false);
	}
}

void ABlasterCharacter::AimOffset(float DeltaTime) {
	if (Combat && Combat->EquippedWeapon == nullptr) return;
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	float Speed = Velocity.Size();
	bool bIsInAir = GetCharacterMovement()->IsFalling();


	if (Speed == 0.f && !bIsInAir) { // 站立且没有跳起时
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		// 当前和起始瞄准旋转之间的差值
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		// 鼠标水平旋转不改变角色朝向
		bUseControllerRotationYaw = false;

	}
	if (Speed > 0.f || bIsInAir) { // 跑步或跳起时
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f); // 存储 Yaw
		AO_Yaw = 0.f;
		// 鼠标水平旋转可以改变角色朝向
		bUseControllerRotationYaw = true;
	}

	AO_Pitch = GetBaseAimRotation().Pitch;
	if (AO_Pitch > 90.f && !IsLocallyControlled()) {
		// 将 [270, 360) 映射到 [-90, 0)
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void ABlasterCharacter::SetOverlappingWeapon(AWeapon* Weapon) {
	if (OverlappingWeapon) {
		OverlappingWeapon->ShowPickupWidget(false);
	}
	OverlappingWeapon = Weapon;
	if (IsLocallyControlled()) {
		if (OverlappingWeapon) {
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

// OverlappingWeapon 值发生变化时，会复制到客户端，同时调用这个函数
void ABlasterCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon) {
	if (OverlappingWeapon) {
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (LastWeapon) {
		LastWeapon->ShowPickupWidget(false);
	}
}

bool ABlasterCharacter::IsWeaponEquipped() {
	return (Combat && Combat->EquippedWeapon);
}

bool ABlasterCharacter::IsAiming() {
	return (Combat && Combat->bAiming);
}

AWeapon* ABlasterCharacter::GetEquippedWeapon() {
	if (Combat == nullptr) return nullptr;
	return Combat->EquippedWeapon;
}

