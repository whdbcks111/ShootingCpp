// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameHudWidget.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTINGCPP_API UGameHudWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void UpdateSpawnState(int aliveCount, int maxCount, int killCount);
	
	UFUNCTION(BlueprintCallable)
	void UpdateBossHp(float progress);
	
	UFUNCTION(BlueprintCallable)
	void UpdateBossBarVisible(bool show);
	
	UFUNCTION(BlueprintCallable)
	void UpdatePlayerHp(float progress);
	
	UFUNCTION(BlueprintCallable)
	void UpdateGameCleared(bool cleared);
	
	UFUNCTION(BlueprintCallable)
	void UpdateGameOverred(bool overred);
	
protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* EnemyCountText;
	
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* BossHpBar;
	
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* PlayerHpBar;
	
	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* BossBarPanel;
	
	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* GameOverPanel;
	
	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* GameClearPanel;
};
