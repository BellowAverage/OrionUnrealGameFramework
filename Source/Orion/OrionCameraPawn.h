// OrionCameraPawn.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "OrionCameraPawn.generated.h"

/**
 * 摄像机 Pawn，用于管理摄像机的移动和缩放
 */
UCLASS()
class ORION_API AOrionCameraPawn : public APawn
{
    GENERATED_BODY()

public:
    // 构造函数
    AOrionCameraPawn();

protected:
    // 初始化时调用
    virtual void BeginPlay() override;

public:
    // 每帧调用
    virtual void Tick(float DeltaTime) override;

    // 绑定输入
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
    // 弹簧臂组件，用于控制摄像机距离
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    USpringArmComponent* SpringArm;

    // 摄像机组件
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    UCameraComponent* Camera;

    // 移动组件
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
    UFloatingPawnMovement* MovementComponent;

    // 摄像机移动速度
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    float MoveSpeed;

    // 缩放限制
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    float MinZoomDistance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    float MaxZoomDistance;

    // 目标缩放距离
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    float DesiredArmLength;

    // 缩放速度（插值速度）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    float ZoomSpeed;

    // 输入处理函数
    void MoveForward(float Value);
    void MoveBackward(float Value);
    void MoveLeft(float Value);
    void MoveRight(float Value);
    void ZoomIn(float Value);   // 处理 ZoomUp
    void ZoomOut(float Value);  // 处理 ZoomDown
};
