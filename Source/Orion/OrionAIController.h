// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "OrionAIController.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API AOrionAIController : public AAIController
{
    GENERATED_BODY()

public:
    AOrionAIController();

protected:
    //virtual void OnPossess(APawn* InPawn) override;
    //virtual void OnUnPossess() override;

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // 感知组件
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    UAIPerceptionComponent* AIPerceptionComp;

    // 用于配置视觉参数
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    UAISenseConfig_Sight* SightConfig;

    // 感知更新回调函数：当感知状态变化时会被调用
    UFUNCTION()
    void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	// AIController Utiltiy Functions
	UFUNCTION(BlueprintCallable, Category = "AIController Utiltiy")
	FString GetControlledPawnName() const;
};
