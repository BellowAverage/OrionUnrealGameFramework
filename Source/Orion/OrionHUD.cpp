﻿#include "OrionHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Font.h"
#include "Kismet/GameplayStatics.h"
#include "OrionChara.h"
#include "OrionPlayerController.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include <vector>

void AOrionHUD::BeginPlay()
{
    Super::BeginPlay();

    if (WB_DeveloperUIBase)
    {
        UUserWidget* DeveloperUIBase = CreateWidget<UUserWidget>(GetWorld(), WB_DeveloperUIBase);
        DeveloperUIBase->AddToViewport();
    }

}
void AOrionHUD::Tick(float DeltaTime)
{
    ListenChangeCharaSelection();
}

void AOrionHUD::ShowPlayerOperationMenu(float MouseX, float MouseY, FHitResult HitResult)
{
    TArray<FName> NamesToPass;
    NamesToPass.Add(FName(TEXT("SpawnCharaInstance")));
    NamesToPass.Add(FName(TEXT("Operation2")));
    NamesToPass.Add(FName(TEXT("Operation3")));

    if (WB_PlayerOperationMenu)
    {
        UUserWidget* PlayerOperationMenu = CreateWidget<UUserWidget>(GetWorld(), WB_PlayerOperationMenu);
        if (PlayerOperationMenu)
        {
			UFunction* SetFunction = PlayerOperationMenu->FindFunction(FName(TEXT("SetArrOperationAvailable")));
            if (SetFunction)
            {
                struct
                {
					TArray<FName> ArrOperationAvailable;
					FHitResult InHitResult;
				} Params;
				Params.ArrOperationAvailable = NamesToPass;
                Params.InHitResult = HitResult;
				PlayerOperationMenu->ProcessEvent(SetFunction, &Params);
            }

            PlayerOperationMenu->AddToViewport();
            PlayerOperationMenu->SetPositionInViewport(FVector2D(MouseX, MouseY));
        }
    }
}

std::vector<AOrionChara*> AOrionHUD::PreviousCharaSelection;

void AOrionHUD::ListenChangeCharaSelection()
{
    AOrionPlayerController* OrionPlayerController = Cast<AOrionPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
    if (!OrionPlayerController)
    {
        UE_LOG(LogTemp, Error, TEXT("OrionPlayerController is null"));
        return;
    }

    if (std::any_of(OrionPlayerController->OrionCharaSelection.begin(), OrionPlayerController->OrionCharaSelection.end(), [](AOrionChara* Chara) { return Chara == nullptr; }))
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid pointer in OrionCharaSelection"));
        return;
    }

    if (PreviousCharaSelection.size() != OrionPlayerController->OrionCharaSelection.size() ||
        !std::equal(OrionPlayerController->OrionCharaSelection.begin(), OrionPlayerController->OrionCharaSelection.end(), PreviousCharaSelection.begin()))
    {
        UE_LOG(LogTemp, Warning, TEXT("CharaSelection has changed!"));

        if (WB_CharaSelectionMenu)
        {
            // 检查是否已经存在
            if (ActiveCharaSelectionMenu)
            {
                // 从视口移除并销毁现有组件
                ActiveCharaSelectionMenu->RemoveFromParent();
                ActiveCharaSelectionMenu = nullptr;
            }

            UUserWidget* CharaSelectionMenu = CreateWidget<UUserWidget>(GetWorld(), WB_CharaSelectionMenu);
            if (CharaSelectionMenu)
            {
                // 存储当前实例
                ActiveCharaSelectionMenu = CharaSelectionMenu;

                UFunction* SetFunction = CharaSelectionMenu->FindFunction(FName(TEXT("FuncReloadWBDevelopmentUIBase")));
                if (SetFunction)
                {
                    struct
                    {
                        TArray<AOrionChara*> CharaSelection;
                    } Params;

                    for (auto& each : OrionPlayerController->OrionCharaSelection)
                    {
                        Params.CharaSelection.Add(each);
                    }

                    CharaSelectionMenu->ProcessEvent(SetFunction, &Params);
                }

                CharaSelectionMenu->AddToViewport();
                CharaSelectionMenu->SetPositionInViewport(FVector2D(40, 188));
            }
        }

    }

    // 更新快照
    PreviousCharaSelection = OrionPlayerController->OrionCharaSelection;
}



void AOrionHUD::DrawHUD()
{
    Super::DrawHUD();

    APlayerController* PC = GetOwningPlayerController();
    if (!PC || !Canvas) return;

    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOrionChara::StaticClass(), FoundActors);

    for (AActor* Actor : FoundActors)
    {
        AOrionChara* OrionChara = Cast<AOrionChara>(Actor);
        if (!OrionChara) continue;

        FString CurrObjectName = OrionChara->GetName();
        FString CurrObjectLocation = OrionChara->GetActorLocation().ToString();

        FString ActionQueueContent;
        for (const auto& Action : OrionChara->CharacterActionQueue.Actions)
        {
            ActionQueueContent += Action.Name + TEXT(" ");
        }
        FString CurrActionDebug = OrionChara->CurrentAction ? OrionChara->CurrentAction->Name : TEXT("None");

		FString CurrHealthDebug = FString::Printf(TEXT("Health: %f"), OrionChara->CurrHealth);

        FString CombinedText = FString::Printf(
            TEXT("Name: %s\nLoc: %s\nActionQueue: %s\nCurrentAction: %s\nCurrHealth: %s"),
            *CurrObjectName,
            *CurrObjectLocation,
            *ActionQueueContent,
            *CurrActionDebug,
            *CurrHealthDebug
        );

        // 2) 将OrionChara的世界坐标 转成 屏幕坐标
        FVector2D ScreenPos;
        bool bProjected = UGameplayStatics::ProjectWorldToScreen(
            PC,
            // 可稍微抬高一点，让文字显示在头顶
            OrionChara->GetActorLocation() + FVector(0.f, 0.f, 100.f),
            ScreenPos
        );

        // 3) 在Canvas上绘制文字
        if (bProjected)
        {
            // 选一个字体，UE 默认有 GetLargeFont() / GetMediumFont() 等等
            UFont* RenderFont = GEngine->GetMediumFont();

            // 先设置文本颜色
            Canvas->SetDrawColor(FColor::Yellow);

            // 如果需要阴影，可设置 FFontRenderInfo
            FFontRenderInfo RenderInfo;
            RenderInfo.bEnableShadow = true;

            // Canvas->DrawText(...) 在 UE5.5 中的原型：
            // float DrawText(const UFont*, const FString&, float X, float Y, float XScale=..., float YScale=..., const FFontRenderInfo& RenderInfo=...);

            Canvas->DrawText(
                RenderFont,
                CombinedText,
                ScreenPos.X,
                ScreenPos.Y,
                1.5f,                // XScale
                1.5f,                // YScale
                RenderInfo          // 字体渲染信息
            );

            // 如果需要再次换别的颜色，就再次 SetDrawColor(...) 后再绘制
        }
    }
}


