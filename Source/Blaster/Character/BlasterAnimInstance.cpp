// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterAnimInstance.h"
#include "BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blaster/Weapon/Weapon.h"

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
	EquippedWeapon = BlasterCharacter->GetEquippedWeapon();
	bIsCrouched = BlasterCharacter->bIsCrouched;
	bAiming = BlasterCharacter->IsAiming();

	// 基本瞄准旋转是一个全局旋度
	// 如果直直的看着世界的 X 轴方向，它是 0 
	// 右旋转时增加，左旋旋转时减少
	// 旋转到 180° 时，继续旋转会变为 -180°，然后再递增
	FRotator AimRotation = BlasterCharacter->GetBaseAimRotation();
	
	// 获取运动的旋度。取一个方向向量，获取角色 X 方向的速度。
	// 这个旋转和 AimRotation 是同步的，可以获取它们之间的 delta
	// 按下 D 向右跑时，仍希望向左跑，就会有一个 offset
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(BlasterCharacter->GetVelocity());
	// Offset Yaw for Strafing
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	// 插值使左右动作切换更流畅
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 6.f);
	YawOffset = DeltaRotation.Yaw;

	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = BlasterCharacter->GetActorRotation();
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaTime;
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.f);
	Lean = FMath::Clamp(Interp, -90.f, 90.f);

	AO_Yaw = BlasterCharacter->GetAO_Yaw();
	AO_Pitch = BlasterCharacter->GetAO_Pitch();

	if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && BlasterCharacter->GetMesh()) {
		// 获取左手插槽的变换信息
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
		FVector OutPosition; // 存储输出的位置信息
		FRotator OutRotation; // 存储输出的旋转信息
		// 将 hand_r 的世界变换信息转换为骨骼的相对位置（相对与左手插槽）
		BlasterCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
		LeftHandTransform.SetLocation(OutPosition); // 设置左手插槽相对与右手的位置
		LeftHandTransform.SetRotation(FQuat(OutRotation)); // 设置左手插槽相对于右手的旋转
	}
}
