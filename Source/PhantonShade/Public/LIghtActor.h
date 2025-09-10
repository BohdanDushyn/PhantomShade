#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../ShadowEngine/LightSoursInterface.h"
#include "Components/PointLightComponent.h" 
#include "Components/SphereComponent.h"
#include "LIghtActor.generated.h"


UCLASS()
class PHANTONSHADE_API ALIghtActor : public AActor, public ILightSoursInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALIghtActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light Param", meta = (AllowedClasses = "float"))
	float LightLevel = 1.0f;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool bIsStatic = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	

	FVector GetLightSourPosition();
	float GetLightSourAttenuationRadius();

	//FVector GetLightSourPosition_Implementation() override;
	//float GetLightSourAttenuationRadius_Implementation() override;
};
