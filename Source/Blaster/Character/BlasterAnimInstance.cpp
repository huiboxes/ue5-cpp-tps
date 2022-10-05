// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterAnimInstance.h"
#include "BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


void UBlasterAnimInstance::NativeInitializeAnimation() {
	Super::NativeInitializeAnimation(); // 执行父类中原有逻辑
	BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner()); // 获取拥有这个动画实例的Pawn，将它转为我们的角色类

}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaTime) {
	Super::NativeUpdateAnimation(DeltaTime); // 执行父类中原有逻辑
	// 确保在设置 Blaster 角色和初始化动画之前访问它
	if (BlasterCharacter == nullptr) {
		BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
	}

	if (BlasterCharacter == nullptr) return;

	FVector Velocity = BlasterCharacter->GetVelocity();
	Velocity.Z = 0.f; // 不需要 Z 的分量
	Speed = Velocity.Size();
	bIsInAir = BlasterCharacter->GetCharacterMovement()->IsFalling(); // 如果正在落下，说明在空中
	bIsAcceleration = BlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
	bWeaponEquipped = BlasterCharacter->IsWeaponEquipped();
	bIsCrouched = BlasterCharacter->bIsCrouched;
	bAiming = BlasterCharacter->IsAiming();

}
