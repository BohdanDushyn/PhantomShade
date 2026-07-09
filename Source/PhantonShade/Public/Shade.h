#pragma once

#include "CoreMinimal.h"
#include "Engine/OverlapResult.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Actor.h"
#include "Materials/MaterialInterface.h"
#include "ProceduralMeshComponent.h"
#include "Shade.generated.h"

UCLASS()
class PHANTONSHADE_API AShade : public AActor
{
	GENERATED_BODY()
	
public:	
	AShade();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UProceduralMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* DefaultSceneRoot;

	bool bCollisionEnabled = true;

	TArray<TArray<FVector>> SectionsVerticesArray;

	TArray<TArray<int32>> SectionsTriangelsArray;

	UMaterialInterface* ShadowMateriall;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Parent Actor")
	AActor* ShadowParentActor;


protected:
	virtual void BeginPlay() override;

public:	
	TArray<AActor*> OverlappingActors;
	TArray<AActor*> ShellOverlappingActors;
	float moveDelta = 0.5f;

	void UpdateShadowActorMeshes(int32 SectionID, const TArray<FVector>& VerticesArray, const TArray<int32>& TriangelsArray);

	UFUNCTION(BlueprintCallable, Category = "ShadowUpdate")
	void SetMoveDelta(float NewMoveDelta) { moveDelta = NewMoveDelta; }

	UFUNCTION(BlueprintCallable, Category = "Overlaping")
	TArray<AActor*> GetAllOverlapingActors();
	
	FVector GetProceduralMeshLocationNotInterf();
	FRotator GetProceduralRotationNotInterf();

	void UpdateShadowActorMeshTransform(const FTransform& ObjTransform);

	int32 GetMeshNumSections() { return MeshComponent->GetNumSections(); }

	UFUNCTION(BlueprintCallable, Category = "ShadowUpdate")
	void SetParent(AActor* ParentActor);
	
	UFUNCTION(BlueprintCallable, Category = "ShadowOverlaping")
	TArray<AActor*> CheckSectionOverlapsWithShell();

	UFUNCTION(BlueprintCallable, Category = "ShadowOverlaping")
	FVector GetOverlapSectionCenter(AShade* OtherShade);
	
};
