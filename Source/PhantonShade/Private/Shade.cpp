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

	ShadowMateriall = LoadObject<UMaterialInterface>(
		nullptr,
        TEXT("/Game/Materials/Shadow/M_ShadowMaterial.M_ShadowMaterial") 
    );
}

void AShade::BeginPlay()
{
	Super::BeginPlay();
	
}

void AShade::UpdateShadowActorMeshes(int32 SectionID, const TArray<FVector>& VerticesArray, const TArray<int32>& TriangelsArray)
{
	if (MeshComponent)
	{
		TArray<FVector2D> UVsArray;
		int UVsFlors = VerticesArray.Num() / 2 - 1;
		int std = 0;
		for (int i = 0; i <= UVsFlors; i++)
		{
			UVsArray.Add(FVector2D(0.0f, std));
			UVsArray.Add(FVector2D(1.0f, std));
			std += 1.0f / UVsFlors;
		}
		if (MeshComponent->GetProcMeshSection(SectionID) && MeshComponent->GetProcMeshSection(SectionID)->ProcVertexBuffer.Num() == VerticesArray.Num())
		{

			for (int32 i = 0; i < VerticesArray.Num(); i++)
			{
				if (MeshComponent->GetProcMeshSection(SectionID) && (MeshComponent->GetProcMeshSection(SectionID)->ProcVertexBuffer[i].Position - VerticesArray[i]).Length() > moveDelta)
				{
					while (!SectionsVerticesArray.IsValidIndex(SectionID) && !SectionsTriangelsArray.IsValidIndex(SectionID))
					{
						SectionsVerticesArray.Add(TArray<FVector>{});
						SectionsTriangelsArray.Add(TArray<int32>{});
					}
					SectionsVerticesArray[SectionID] = VerticesArray;
					SectionsTriangelsArray[SectionID] = TriangelsArray;

					FFunctionGraphTask::CreateAndDispatchWhenReady([this, SectionID, VerticesArray, TriangelsArray, UVsArray]() {
						MeshComponent->CreateMeshSection(SectionID, VerticesArray, TriangelsArray, TArray<FVector>{}, UVsArray, TArray<FColor>{}, TArray<FProcMeshTangent>{}, bCollisionEnabled);
						}, TStatId(), nullptr, ENamedThreads::GameThread);
					return;
				}
			}
		}
		else {
			FFunctionGraphTask::CreateAndDispatchWhenReady([this, SectionID, VerticesArray, TriangelsArray, UVsArray]() {
				MeshComponent->CreateMeshSection(SectionID, VerticesArray, TriangelsArray, TArray<FVector>{}, UVsArray, TArray<FColor>{}, TArray<FProcMeshTangent>{}, bCollisionEnabled);
				}, TStatId(), nullptr, ENamedThreads::GameThread);
			
			MeshComponent->SetMaterial(SectionID, ShadowMateriall);

			while (!SectionsVerticesArray.IsValidIndex(SectionID) && !SectionsTriangelsArray.IsValidIndex(SectionID))
			{
				SectionsVerticesArray.Add(TArray<FVector>{});
				SectionsTriangelsArray.Add(TArray<int32>{});
			}
			SectionsVerticesArray[SectionID] = VerticesArray;
			SectionsTriangelsArray[SectionID] = TriangelsArray;
		}

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MeshComponent is not initialized!"));
	}
}

TArray<AActor*> AShade::GetAllOverlapingActors()
{
	MeshComponent->GetOverlappingActors(OverlappingActors);
	return OverlappingActors;
}


TArray<AActor*> AShade::CheckSectionOverlapsWithShell()
{
    const float ShellHeightUp = 50.f;
    const float ShellHeightDown = 50.f;
    const float ShellInflate = 10.f;

    int32 SectionCount = MeshComponent->GetNumSections();

    for (int32 i = 0; i < SectionCount; i++)
    {
        FProcMeshSection* Section = MeshComponent->GetProcMeshSection(i);
        if (!Section || Section->ProcVertexBuffer.IsEmpty()) continue;

        TArray<FVector> WorldVerts;
        WorldVerts.Reserve(Section->ProcVertexBuffer.Num());

        for (const FProcMeshVertex& Vertex : Section->ProcVertexBuffer)
        {
            WorldVerts.Add(
                MeshComponent->GetComponentTransform()
                .TransformPosition(Vertex.Position)
            );
        }

        FVector Centroid = FVector::ZeroVector;
        for (const FVector& V : WorldVerts) Centroid += V;
        Centroid /= WorldVerts.Num();

        FVector PointA = WorldVerts[0];
        FVector PointB = WorldVerts[0];
        float MaxDist = 0.f;

        for (int32 j = 0; j < WorldVerts.Num(); j++)
        {
            for (int32 k = j + 1; k < WorldVerts.Num(); k++)
            {
                float Dist = FVector::DistSquared(WorldVerts[j], WorldVerts[k]);
                if (Dist > MaxDist)
                {
                    MaxDist = Dist;
                    PointA = WorldVerts[j];
                    PointB = WorldVerts[k];
                }
            }
        }

        FVector AxisX = (PointB - PointA).GetSafeNormal();

        FVector LocalNormal = Section->ProcVertexBuffer[0].Normal;
        FVector AxisZ = MeshComponent->GetComponentTransform()
            .TransformVector(LocalNormal)
            .GetSafeNormal();

        FVector AxisY = FVector::CrossProduct(AxisZ, AxisX).GetSafeNormal();

        FMatrix RotMatrix(AxisX, AxisY, AxisZ, FVector::ZeroVector);
        FQuat SectionQuat = RotMatrix.ToQuat();

        float ExtentX = 0.f; 
        float ExtentY = 0.f; 

        for (const FVector& V : WorldVerts)
        {
            FVector Local = V - Centroid;
            ExtentX = FMath::Max(ExtentX, FMath::Abs(FVector::DotProduct(Local, AxisX)));
            ExtentY = FMath::Max(ExtentY, FMath::Abs(FVector::DotProduct(Local, AxisY)));
        }

        FVector Extent = FVector(
            ExtentX + ShellInflate,
            ExtentY + ShellInflate,
            ShellHeightUp           
        );

        // DEBUG 

        //UKismetSystemLibrary::DrawDebugBox(
        //    GetWorld(),
        //    Centroid,
        //    Extent,
        //    FLinearColor::Yellow,
        //    SectionQuat.Rotator(),  
        //    0.f,
        //    2.f
        //);

        //UKismetSystemLibrary::DrawDebugArrow(
        //    GetWorld(),
        //    Centroid,
        //    Centroid + AxisZ * ShellHeightUp,
        //    20.f,
        //    FLinearColor::Blue,
        //    0.f,
        //    2.f
        //);

        TArray<FOverlapResult> Overlaps;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(GetOwner());

        GetWorld()->OverlapMultiByChannel(
            Overlaps,
            Centroid,
            SectionQuat,            
            ECC_WorldDynamic,
            FCollisionShape::MakeBox(Extent),
            Params
        );

        if (Overlaps.IsEmpty()) continue;

        //UKismetSystemLibrary::DrawDebugBox(
        //    GetWorld(),
        //    Centroid,
        //    Extent,
        //    FLinearColor::Red,
        //    SectionQuat.Rotator(),
        //    0.f,
        //    3.f
        //);


        for (FOverlapResult& Result : Overlaps)
        {
            AActor* OtherActor = Result.GetActor();

            if (!OtherActor) continue;

            ShellOverlappingActors.AddUnique(OtherActor);
        }
    }

	return ShellOverlappingActors;
}

FVector AShade::GetOverlapSectionCenter(AShade* OtherShade)
{
    if (!OtherShade || !OtherShade->MeshComponent || !MeshComponent)
        return FVector::ZeroVector;

    TArray<AActor*> Overlapping;
    MeshComponent->GetOverlappingActors(Overlapping);
    if (!Overlapping.Contains(OtherShade)) return FVector::ZeroVector;

    UProceduralMeshComponent* OtherMesh = OtherShade->MeshComponent;
    int32 SectionCount = OtherMesh->GetNumSections();

    for (int32 i = 0; i < SectionCount; i++)
    {
        FProcMeshSection* Section = OtherMesh->GetProcMeshSection(i);
        if (!Section || Section->ProcVertexBuffer.IsEmpty()) continue;

        for (const FProcMeshVertex& V : Section->ProcVertexBuffer)
        {
            FVector WorldPos = OtherMesh->GetComponentTransform()
                .TransformPosition(V.Position);

            if (MeshComponent->OverlapComponent(
                WorldPos,
                FQuat::Identity,
                FCollisionShape::MakeSphere(1.f)))
            {
                FVector SectionCentroid = FVector::ZeroVector;
                for (const FProcMeshVertex& CV : Section->ProcVertexBuffer)
                {
                    SectionCentroid += OtherMesh->GetComponentTransform()
                        .TransformPosition(CV.Position);
                }
                SectionCentroid /= Section->ProcVertexBuffer.Num();

                return SectionCentroid;
            }
        }
    }

    return FVector::ZeroVector;
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


void AShade::SetParent(AActor* ParentActor)
{
	if (!ParentActor) return;

	ShadowParentActor = ParentActor;
}

