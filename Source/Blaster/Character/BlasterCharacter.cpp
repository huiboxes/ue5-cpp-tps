// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Blaster/Weapon/Weapon.h"

ABlasterCharacter::ABlasterCharacter()
{
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
}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 注册要复制的重叠武器变量（只复制给与武器范围重叠的人物的客户端）
	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly);
	//DOREPLIFETIME(ABlasterCharacter, OverlappingWeapon);
}

void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABlasterCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	// 绑定父类的 Jump 方法
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

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

