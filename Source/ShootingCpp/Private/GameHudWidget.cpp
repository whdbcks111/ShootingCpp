// Fill out your copyright notice in the Description page of Project Settings.


#include "GameHudWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UGameHudWidget::UpdateSpawnState(int aliveCount, int maxCount, int killCount)
{
	if (EnemyCountText != nullptr)
	{
		EnemyCountText->SetText(FText::FromString(FString::Printf(TEXT("%d"), maxCount - killCount)));
	}
}

void UGameHudWidget::UpdateBossHp(float progress)
{
	if (BossHpBar != nullptr)
	{
		BossHpBar->SetPercent(progress);
	}
}

void UGameHudWidget::UpdateBossBarVisible(bool show)
{
	if (BossBarPanel != nullptr)
	{
		BossBarPanel->SetVisibility(show ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UGameHudWidget::UpdatePlayerHp(float progress)
{
	if (PlayerHpBar != nullptr)
	{
		PlayerHpBar->SetPercent(progress);
	}
}

void UGameHudWidget::UpdateGameCleared(bool cleared)
{
	GameClearPanel->SetVisibility(cleared ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void UGameHudWidget::UpdateGameOverred(bool overred)
{
	GameOverPanel->SetVisibility(overred ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}
