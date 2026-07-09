// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class PHANTONSHADE_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable, Category = "AI|Perception")
	void UpdateSightConfig(float NewSightRadius, float NewLoseSightRadius, float NewPeripheralVisionAngle);

	UFUNCTION(BlueprintCallable, Category = "AI|Perception")
	UAISenseConfig_Sight* GetSightSenseConfig();
};
