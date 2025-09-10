// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Shade.h" 
#include "LightActor.h"
#include "Components/ActorComponent.h"
#include "ProceduralMeshComponent.h"
#include "../ShadowEngine/LightSoursInterface.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AC_ShadowComponent.generated.h"


USTRUCT(BlueprintType)
struct FLineTraceResult
{
    GENERATED_BODY()

    //UPROPERTY(BlueprintReadOnly)
    //FVector StartPointResult;

    UPROPERTY(BlueprintReadOnly)
    FVector EndPointResult;

    UPROPERTY(BlueprintReadOnly)
    bool bIsTraced;
};

USTRUCT(BlueprintType)
struct FOffsetResultVector
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    float X;

    UPROPERTY(BlueprintReadOnly)
    float Z;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class PHANTONSHADE_API UAC_ShadowComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAC_ShadowComponent();

	TArray<ALIghtActor*> CastedLightActors;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shadow Actor Settings", meta = (AllowedClasses = "Actor"))
    TSubclassOf<AActor> ShadowActorClass;

protected:
    UWorld* WorldPtr = GetWorld();

    std::atomic<int32> ActiveTaskCount{ 0 };

    UClass* LightActorBlueprintClass;

	virtual void BeginPlay() override;

	FVector OwnerLocation;

	bool bInShadow = false;

    float timer1Value = 0;

    int32 timer1Counter = 0;

	FVector OwnerForwardVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TimerFunction")
    bool bTimerPaused = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShadowFunction")
    bool IsStarted = false;

    FTimerHandle MyTimerHandle;

    FVector MeshLocationVector;

	float offsetCobvexityCoefficient = 1.0f;

    FRotator MeshRotator;

	float TimerInterval = 1.0f;

    AActor* ParentActor;

    float lightLevel = 0;

    UPROPERTY(BlueprintReadOnly, Category = "ShadeActor")
    AShade* CastedShadeActor;

    float OffsetFromPlane = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShadowFunction")
    int AmountOfPieces = 2;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShadowFunction")
    TArray<FVector> MapOfShadow;

public:	 
	void SpawnShadowActor();

	//!!!!розробити зміщення для квадратних об'єктів

    bool AreAllTasksComplete() const
    {
        return ActiveTaskCount.load() == 0;
    }

    UFUNCTION(BlueprintCallable, Category = "Overlaping")
    TArray<AActor*> GetShadowOverlapingActors() { return CastedShadeActor->GetAllOverlapingActors(); }

    UFUNCTION(BlueprintCallable, Category = "Overlaping")
    void SetParentActor();

    UFUNCTION(BlueprintCallable, Category = "LightingTimer")
    void SetTimerInterval(float NewTimerInterval);

    UFUNCTION(BlueprintCallable, Category = "LightingTimer")
    void PauseTimer();

    UFUNCTION(BlueprintCallable, Category = "LightingTimer")
	void ResumeTimer();

    UFUNCTION(BlueprintCallable, Category = "Lighting")
    void AddLightActor(AActor* Actor);

    UFUNCTION(BlueprintCallable, Category = "Lighting")
    void SetLightActors(const TArray<AActor*>& Actors);

    UFUNCTION(BlueprintCallable, Category = "Lighting")
    void ClearLightActors();

    UFUNCTION(BlueprintCallable, Category = "Lighting")
    void RemoveLightActor(AActor* Actor);
		
    UFUNCTION(BlueprintCallable, Category = "Lighting")
    int32 GetLightSoursAmount();

    UFUNCTION(BlueprintCallable, Category = "ShadowFunction")
    void StartShadowCalculateWithParams(float TimerDelay, TArray<FVector> NewMapOfShadow, const TArray<AActor*>& NewLightActors, int AmountOfFloorPieces, float MinShadowMoveDelta);

    UFUNCTION(BlueprintCallable, Category = "ShadowFunction")
    void StartShadowCalculate();

    UFUNCTION(BlueprintCallable, Category = "ShadowFunction")
    void StartShadowCalculateWithSetTimer(float NewTimer);

    UFUNCTION(BlueprintCallable, Category = "ShadowFunction")
    void SetShadeActor(AActor* NewShadeActor);

    UFUNCTION(BlueprintCallable, Category = "ShadowFunction")
	void SetAmountOfPieces(int NewAmount) { AmountOfPieces = NewAmount; }

    UFUNCTION(BlueprintCallable, Category = "ShadowFunction")
    int GetAmountOfPieces() { return AmountOfPieces; }

    UFUNCTION(BlueprintCallable, Category = "ShadowFunction")
	void SetMapOfShadow(const TArray<FVector>& NewMapOfShadow) { MapOfShadow = NewMapOfShadow; }

    UFUNCTION(BlueprintCallable, Category = "ShadowFunction")
    FLineTraceResult LineTraceWithOffset(const FVector& LightStartLocation, FOffsetResultVector Offset, float RayMaxLength);

    UFUNCTION(BlueprintCallable, Category = "ShadowFunction")
    FOffsetResultVector MakeOffset(FVector OffsetValue, FVector LightPosition);

    UFUNCTION(BlueprintCallable, Category = "ShadowFunction")
    TArray<FVector> MakeShadowFloor(FVector OffsetValue, FVector LightStartLocation, float RayMaxLenght);

    UFUNCTION(BlueprintCallable, Category = "Lighting")
    void CreateShadow();

    void CreateOneShadow(ALIghtActor* LightActor, int32 id);

    //void SetLightActorsFromOverlapping();
};
