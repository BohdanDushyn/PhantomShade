// Fill out your copyright notice in the Description page of Project Settings.


#include "LIghtActor.h"


// Sets default values
ALIghtActor::ALIghtActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALIghtActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
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

/*
FVector ALIghtActor::GetLightSourPosition_Implementation()
{
	return GetComponentByClass<UPointLightComponent>()->GetComponentLocation();
}

float ALIghtActor::GetLightSourAttenuationRadius_Implementation()
{
	return GetComponentByClass<UPointLightComponent>()->AttenuationRadius;
}

*/