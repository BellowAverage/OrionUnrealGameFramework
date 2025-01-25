﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "OrionAIController.h"
#include "GameFramework/Actor.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Engine/Engine.h"

AOrionAIController::AOrionAIController()
{
    // 创建感知组件
    AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComp"));
    SetPerceptionComponent(*AIPerceptionComp);

    // 创建 Sight 配置
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

    // 配置视觉范围
    SightConfig->SightRadius = 1500.f;                 // 能看到的最大距离
    SightConfig->LoseSightRadius = 1600.f;             // 失去目标的距离
    SightConfig->PeripheralVisionAngleDegrees = 90.f;  // 左右视野(一侧)角度
    SightConfig->SetMaxAge(5.f);                       // 感知保留时长

    // 告诉感知组件可以感知敌对、中立、友军
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

    // 将Sight配置添加到感知组件
    AIPerceptionComp->ConfigureSense(*SightConfig);
    AIPerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());
}

void AOrionAIController::BeginPlay()
{
    Super::BeginPlay();

    // 绑定感知更新事件
    if (AIPerceptionComp)
    {
        AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AOrionAIController::OnTargetPerceptionUpdated);
    }
}

void AOrionAIController::Tick(float DeltaTime)
{

}



// 当检测到新的 Actor，或者感知状态改变时，会调用此函数
void AOrionAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    // Stimulus.IsActive() == true 表示当前还能“看见”该Actor
	FString DebugMessage = FString::Printf(TEXT("%s: %s sighted"), *GetControlledPawnName(), *Actor->GetName());
    if (Stimulus.IsActive())
    {
        //if (GEngine)
        //{
        //    GEngine->AddOnScreenDebugMessage(
        //        -1,
        //        3.0f,
        //        FColor::Green,
        //        DebugMessage
        //    );
        //}
    }
    else
    {
        // 一旦看不见了，通常会收到 Stimulus.IsActive() == false
        //FString DebugMessage2 = FString::Printf(TEXT("%s: Lost sight of %s"), *GetControlledPawnName(), *Actor->GetName());
        //if (GEngine)
        //{
        //    GEngine->AddOnScreenDebugMessage(
        //        -1,
        //        3.0f,
        //        FColor::Red,
        //        DebugMessage2
        //    );
        //}
    }
}

FString AOrionAIController::GetControlledPawnName() const
{
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn)
	{
		return ControlledPawn->GetName();
	}
    else
    {
		return "No Pawn is being controlled.";
    }
}

//void AOrionAIController::OnPossess(APawn* InPawn)
//{
//    Super::OnPossess(InPawn);
//    UE_LOG(LogTemp, Log, TEXT("OrionAIController has possessed: %s"), *GetNameSafe(InPawn));
//}
//
//void AOrionAIController::OnUnPossess()
//{
//    Super::OnUnPossess();
//    UE_LOG(LogTemp, Log, TEXT("OrionAIController has unpossessed its Pawn."));
//}

