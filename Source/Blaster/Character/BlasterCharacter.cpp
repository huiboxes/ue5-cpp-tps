// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

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
	// 启用 Pwan 控制
	CameraBoom->bUsePawnControlRotation = true;


	// 创建相机
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	// 附加到弹簧臂上
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// 因为它附加到 CameraBoom ，所以不需要启用 Pwan 控制
	FollowCamera->bUsePawnControlRotation = false;

}

void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

