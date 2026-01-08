// Fill out your copyright notice in the Description page of Project Settings.


#include "Shade.h"

AShade::AShade() : Super()
{
	PrimaryActorTick.bCanEverTick = false;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RootComponent = DefaultSceneRoot;

	MeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProcShadow"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->bUseAsyncCooking = true;
	MeshComponent->bUseComplexAsSimpleCollision = true;
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MeshComponent->SetSimulatePhysics(false);
}

void AShade::BeginPlay()
{
	Super::BeginPlay();
	
}

void AShade::UpdateShadowActorMeshes(int32 SectionID, const TArray<FVector>& VerticesArray, const TArray<int32>& TriangelsArray)
{
	if (MeshComponent)
	{

		if (MeshComponent->GetProcMeshSection(SectionID) && MeshComponent->GetProcMeshSection(SectionID)->ProcVertexBuffer.Num() == VerticesArray.Num())
		{
			for (int32 i = 0; i < VerticesArray.Num(); i++)
			{
				if (MeshComponent->GetProcMeshSection(SectionID) && (MeshComponent->GetProcMeshSection(SectionID)->ProcVertexBuffer[i].Position - VerticesArray[i]).Length() > moveDelta)
				{
					FFunctionGraphTask::CreateAndDispatchWhenReady([this, SectionID, VerticesArray, TriangelsArray]() {
						MeshComponent->CreateMeshSection(SectionID, VerticesArray, TriangelsArray, TArray<FVector>{}, TArray<FVector2D>{}, TArray<FColor>{}, TArray<FProcMeshTangent>{}, bCollisionEnabled);
						}, TStatId(), nullptr, ENamedThreads::GameThread);
					return;
				}
			}
		}
		else {
			FFunctionGraphTask::CreateAndDispatchWhenReady([this, SectionID, VerticesArray, TriangelsArray]() {
				MeshComponent->CreateMeshSection(SectionID, VerticesArray, TriangelsArray, TArray<FVector>{}, TArray<FVector2D>{}, TArray<FColor>{}, TArray<FProcMeshTangent>{}, bCollisionEnabled);
				}, TStatId(), nullptr, ENamedThreads::GameThread);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MeshComponent is not initialized!"));
	}
}

/*
void AShade::RemoveMeschSections()
{
	MeshComponent->ClearAllMeshSections();
}*/

TArray<AActor*> AShade::GetAllOverlapingActors()
{
	MeshComponent->GetOverlappingActors(OverlappingActors);
	return OverlappingActors;
}


FVector AShade::GetProceduralMeshLocationNotInterf()
{
	return MeshComponent->GetComponentLocation();
}

FRotator AShade::GetProceduralRotationNotInterf()
{
	return MeshComponent->GetComponentRotation();
}

void AShade::UpdateShadowActorMeshTransform(const FTransform& ObjTransform)
{
	SetActorTransform(ObjTransform);
}

