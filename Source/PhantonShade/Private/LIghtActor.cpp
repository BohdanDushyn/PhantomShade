#include "LIghtActor.h"


ALIghtActor::ALIghtActor()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ALIghtActor::BeginPlay()
{
	Super::BeginPlay();
}

void ALIghtActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector ALIghtActor::GetLightSourPosition()
{
	return GetComponentByClass<USphereComponent>()->GetComponentLocation();
}

float ALIghtActor::GetLightSourAttenuationRadius()
{
	return GetComponentByClass<USphereComponent>()->GetScaledSphereRadius();
}