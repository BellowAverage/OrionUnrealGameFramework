// OrionCameraPawn.cpp

#include "OrionCameraPawn.h"
#include "Components/InputComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/Engine.h" // 确保包含此头文件以使用 GEngine

AOrionCameraPawn::AOrionCameraPawn()
{
    PrimaryActorTick.bCanEverTick = true;

    // 创建弹簧臂组件并设置为根组件
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    RootComponent = SpringArm;
    SpringArm->TargetArmLength = 1000.f; // 初始距离
    SpringArm->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f)); // 斜向下俯视
    SpringArm->bUsePawnControlRotation = false; // 不随 Pawn 旋转

    // 创建摄像机组件
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    Camera->bUsePawnControlRotation = false; // 不随 Pawn 旋转

    // 创建移动组件
    MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));
    MovementComponent->MaxSpeed = 1000.f;

    // 设置默认移动速度
    MoveSpeed = 200.f;

    // 设置缩放限制
    MinZoomDistance = -2000.f;
    MaxZoomDistance = 4000.f;

    // 初始化目标缩放距离为当前弹簧臂长度
    DesiredArmLength = SpringArm->TargetArmLength;

    // 设置缩放速度
    ZoomSpeed = 40.f; // 可以根据需要调整
}

void AOrionCameraPawn::BeginPlay()
{
    Super::BeginPlay();

    // 设置 Pawn 的初始位置
    FVector InitialPawnLocation(-2000.0f, 0.0f, 2000.0f); // 例如，世界中心
    SetActorLocation(InitialPawnLocation);

    // 设置弹簧臂的旋转，使摄像机俯视
    SpringArm->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));

    // 如果需要，调整弹簧臂长度
    SpringArm->TargetArmLength = 1000.f;

    // 初始化目标缩放距离
    DesiredArmLength = SpringArm->TargetArmLength;
}

void AOrionCameraPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (SpringArm->TargetArmLength != DesiredArmLength)
    {
        // 使用插值函数平滑过渡
        SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, DesiredArmLength, DeltaTime, ZoomSpeed);
    }
}

void AOrionCameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (PlayerInputComponent)
    {
        // 绑定 WASD 控制摄像机移动
        PlayerInputComponent->BindAxis("MoveForward", this, &AOrionCameraPawn::MoveForward);
        PlayerInputComponent->BindAxis("MoveBackward", this, &AOrionCameraPawn::MoveBackward);
        PlayerInputComponent->BindAxis("MoveLeft", this, &AOrionCameraPawn::MoveLeft);
        PlayerInputComponent->BindAxis("MoveRight", this, &AOrionCameraPawn::MoveRight);

        // 绑定鼠标滚轮控制摄像机缩放
        PlayerInputComponent->BindAxis("ZoomUp", this, &AOrionCameraPawn::ZoomIn);
        PlayerInputComponent->BindAxis("ZoomDown", this, &AOrionCameraPawn::ZoomOut);
    }
}

void AOrionCameraPawn::MoveForward(float Value)
{
    if (Value != 0.f)
    {
        // 向前移动（沿世界的 X 轴）
        if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("MoveForward Called."));
        AddMovementInput(FVector::ForwardVector, Value * MoveSpeed * GetWorld()->GetDeltaSeconds());
    }
}

void AOrionCameraPawn::MoveBackward(float Value)
{
    if (Value != 0.f)
    {
        // 向后移动（沿世界的 -X 轴）
        if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("MoveBackward Called."));
        AddMovementInput(-FVector::ForwardVector, Value * MoveSpeed * GetWorld()->GetDeltaSeconds());
    }
}

void AOrionCameraPawn::MoveLeft(float Value)
{
    if (Value != 0.f)
    {
        // 向左移动（沿世界的 -Y 轴）
        if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("MoveLeft Called."));
        AddMovementInput(-FVector::RightVector, Value * MoveSpeed * GetWorld()->GetDeltaSeconds());
    }
}

void AOrionCameraPawn::MoveRight(float Value)
{
    if (Value != 0.f)
    {
        // 向右移动（沿世界的 Y 轴）
        if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("MoveRight Called."));
        AddMovementInput(FVector::RightVector, Value * MoveSpeed * GetWorld()->GetDeltaSeconds());
    }
}

void AOrionCameraPawn::ZoomIn(float Value)
{
    if (Value != 0.f && SpringArm)
    {
        // 添加调试消息
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("ZoomIn Called with Value: %f"), Value));
        }

        float ZoomAmount = Value * 25.f; // 缩放量
        DesiredArmLength = FMath::Clamp(SpringArm->TargetArmLength - ZoomAmount, MinZoomDistance, MaxZoomDistance);
    }
}

void AOrionCameraPawn::ZoomOut(float Value)
{
    if (Value != 0.f && SpringArm)
    {
        // 添加调试消息
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("ZoomOut Called with Value: %f"), Value));
        }

        float ZoomAmount = Value * 25.f; // 缩放量
        DesiredArmLength = FMath::Clamp(SpringArm->TargetArmLength + ZoomAmount, MinZoomDistance, MaxZoomDistance);
    }
}
