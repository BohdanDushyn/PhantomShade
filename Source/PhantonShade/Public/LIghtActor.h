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
	ALIghtActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light Param", meta = (AllowedClasses = "float"))
	float LightLevel = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light Param")
	bool IsActive = true;
protected:
	virtual void BeginPlay() override;

	bool bIsStatic = false;

public:	
	virtual void Tick(float DeltaTime) override;

	

	FVector GetLightSourPosition();
	float GetLightSourAttenuationRadius();

};
