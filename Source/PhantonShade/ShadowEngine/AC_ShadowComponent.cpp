// Fill out your copyright notice in the Description page of Project Settings.

#include "AC_ShadowComponent.h"

// Sets default values for this component's properties
UAC_ShadowComponent::UAC_ShadowComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

}


// Called when the game starts
void UAC_ShadowComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UAC_ShadowComponent::StartShadowCalculate()
{
	if (!IsValid(this) || !GetOwner() || !GetWorld())
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid component state in StartShadowCalculate"));
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(
		MyTimerHandle,
		this,
		&UAC_ShadowComponent::CreateShadow,
		TimerInterval,
		true,
		FMath::FRand()
	);
}

void UAC_ShadowComponent::StartShadowCalculateWithSetTimer(float NewTimer)
{
	//UE_LOG(LogTemp, Warning, TEXT("StartShadowCalculateWithSetTimer"));
	IsStarted = true;
	SetTimerInterval(NewTimer);
	GetWorld()->GetTimerManager().SetTimer(
		MyTimerHandle,
		this,
		&UAC_ShadowComponent::CreateShadow,
		TimerInterval,
		true,
		0.0f
	);
}

void UAC_ShadowComponent::SetShadeActor(AActor* NewShadeActor)
{
	CastedShadeActor = Cast<AShade>(NewShadeActor);
}

void UAC_ShadowComponent::SpawnShadowActor()
{
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		UE_LOG(LogTemp, Error, TEXT("Shadow Component: No owner actor found"));
		return;
	}

	FTransform OwnerTransform = OwnerActor->GetActorTransform();

	if (ShadowActorClass)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = OwnerActor; 
			SpawnParams.Instigator = OwnerActor->GetInstigator();
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			AActor* SpawnedActor = World->SpawnActor<AActor>(
				ShadowActorClass,
				OwnerTransform.GetLocation(),
				OwnerTransform.GetRotation().Rotator(),
				SpawnParams
			);

			if (SpawnedActor)
			{
				SetShadeActor(SpawnedActor);

				UE_LOG(LogTemp, Warning, TEXT("Shadow Component: Successfully spawned shadow actor"));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Shadow Component: Failed to spawn shadow actor"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Shadow Component: World is null"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Shadow Component: ShadowActorClass is not set! Please assign it in the editor."));
	}
}

bool UAC_ShadowComponent::AreAllTasksComplete() const
{
	return ActiveTaskCount.load() == 0;
}

void UAC_ShadowComponent::SetParentActor()
{
	//if (!NewParent) return;

	//ParentActor = NewParent;

	//if (!IsValid(GetOwner())) return;

	ParentActor = GetOwner();
}

void UAC_ShadowComponent::SetShadowCollision(bool bEnableCollision)
{
	CastedShadeActor->bCollisionEnabled = bEnableCollision;	
}

void UAC_ShadowComponent::SetTimerInterval(float NewTimerInterval)
{
	TimerInterval = NewTimerInterval;
	if (GetWorld()->GetTimerManager().IsTimerActive(MyTimerHandle))
	{
		GetWorld()->GetTimerManager().SetTimer(
			MyTimerHandle,
			this,
			&UAC_ShadowComponent::CreateShadow,
			TimerInterval,
			true,
			0.0f
		);
	}

}

void UAC_ShadowComponent::PauseTimer()
{
	if (MyTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().PauseTimer(MyTimerHandle);
		bTimerPaused = true;
	}
}

void UAC_ShadowComponent::ResumeTimer()
{
	if (MyTimerHandle.IsValid() && bTimerPaused)
	{
		GetWorld()->GetTimerManager().UnPauseTimer(MyTimerHandle);
		bTimerPaused = false;
	}
}

void UAC_ShadowComponent::AddLightActor(AActor* Actor)
{
	if (Actor)
	{
		CastedLightActors.AddUnique(Cast<ALIghtActor>(Actor));
	}
}

void UAC_ShadowComponent::SetLightActors(const TArray<AActor*>& Actors)
{
	CastedLightActors.Empty();

	for (AActor* Actor : Actors)
	{
		if (Actor)
		{
			CastedLightActors.AddUnique(Cast<ALIghtActor>(Actor));
		}
	}
}

void UAC_ShadowComponent::ClearLightActors()
{
	CastedLightActors.Empty();
}

void UAC_ShadowComponent::RemoveLightActor(AActor* Actor)
{
	if (Actor)
	{
		CastedLightActors.Remove(Cast<ALIghtActor>(Actor));
	}

	CastedShadeActor->MeshComponent->ClearAllMeshSections();
}

int32 UAC_ShadowComponent::GetLightSoursAmount()
{
	return CastedLightActors.Num();
}

void UAC_ShadowComponent::StartShadowCalculateWithParams(float TimerDelay, TArray<FVector> NewMapOfShadow, const TArray<AActor*>& NewLightActors, int AmountOfFloorPieces, float MinShadowMoveDelta)
{
	//UE_LOG(LogTemp, Warning, TEXT("StartShadowCalculateWithParams"));
	SetMapOfShadow(NewMapOfShadow);
	SetParentActor();
	SetAmountOfPieces(AmountOfFloorPieces);
	SpawnShadowActor();
	SetLightActors(NewLightActors);
	StartShadowCalculateWithSetTimer(TimerDelay);
	if (CastedShadeActor)
	{
		CastedShadeActor->SetMoveDelta(MinShadowMoveDelta);
	}
}

FLineTraceResult UAC_ShadowComponent::LineTraceWithOffset(const FVector& LightStartLocation, FOffsetResultVector Offset, float RayMaxLength)
{
	FLineTraceResult Result;
	Result.bIsTraced = false;
	FVector CalculatedXOffset = (OwnerLocation - LightStartLocation).GetSafeNormal().Cross(FVector(0, 0, 1)) * Offset.X;
	CalculatedXOffset += OwnerLocation - LightStartLocation;
	CalculatedXOffset += FVector(0, 0, Offset.Z);
	FVector EndPoint = LightStartLocation + (CalculatedXOffset.GetSafeNormal() * RayMaxLength);
	
	TArray<FHitResult> HitResults;
	
	bool bHit = WorldPtr->LineTraceMultiByChannel(
		HitResults,
		LightStartLocation,
		EndPoint,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionQueryParams{}
	);


	/*
	auto TraceFunction = [this](const FVector& FuncStartPoint, const FVector& FuncEndPoint) {
		TArray<FHitResult> FuncHitResults;
		if (!GetWorld())
		{
			return FuncHitResults;
		}
		bool bHit = GetWorld()->LineTraceMultiByChannel(
			FuncHitResults,
			FuncStartPoint,
			FuncEndPoint,
			ECollisionChannel::ECC_GameTraceChannel2,
			FCollisionQueryParams{}
		);

		return FuncHitResults;
		};
	
	TSharedPtr<TArray<FHitResult>> SharedResults = MakeShared<TArray<FHitResult>>();

	FFunctionGraphTask::CreateAndDispatchWhenReady([SharedResults, &TraceFunction, LightStartLocation, EndPoint]() {
		*SharedResults = TraceFunction(LightStartLocation, EndPoint);
		}, TStatId(), nullptr, ENamedThreads::GameThread)->Wait();

	HitResults = *SharedResults;
	*/
	

	//UE_LOG(LogTemp, Warning, TEXT("HitResults %d %d"), Offset, HitResults.Num());
	int8 LastHit = HitResults.Num() - 1;
	if (HitResults.Num() >= 2)
	{
		//UE_LOG(LogTemp, Warning, TEXT("HitResults %s"), *HitResults[0].GetComponent()->GetName());
		if (HitResults[0].GetComponent() == GetOwner()->FindComponentByClass<UCapsuleComponent>() && HitResults[LastHit].GetActor() != GetOwner()) {
			//UE_LOG(LogTemp, Warning, TEXT("HitResults %s"), *HitResults[LastHit].GetComponent()->GetName());
			//Result.StartPoint = HitResults[0].ImpactPoint;
			Result.EndPointResult = HitResults[LastHit].ImpactPoint;
			Result.bIsTraced = true;
		}
	}
	
	return Result;
}

FOffsetResultVector UAC_ShadowComponent::MakeOffset(FVector OffsetValue, FVector LightPosition)
{
	float Dot = FVector::DotProduct((LightPosition - OwnerLocation).GetSafeNormal(), OwnerForwardVector.GetSafeNormal());
	float angeleCorection = 1 - (FMath::Abs(0.5f - FMath::Abs(Dot)) * 2);
	angeleCorection = 1 + (0.4142f * offsetCobvexityCoefficient * angeleCorection);
	return FOffsetResultVector(((OffsetValue * FVector(FMath::Abs(Dot), 1 - FMath::Abs(Dot), 0)).Length() * angeleCorection), OffsetValue.Z);
}


TArray<FVector> UAC_ShadowComponent::MakeShadowFloor(FVector OffsetValue, FVector LightStartLocation, float RayMaxLenght)
{
	
	TArray<FVector> ShadowPointMap;

	FOffsetResultVector Offset = MakeOffset(OffsetValue, LightStartLocation);
	int8 index = 0;
	if (AmountOfPieces < 2) {
		AmountOfPieces = 2;
	}
	
	
	for (int8 i = 0; i < AmountOfPieces; i++)
	{
		FLineTraceResult Result = LineTraceWithOffset(LightStartLocation, FOffsetResultVector(Offset.X - ((Offset.X / 0.5 / AmountOfPieces) * i), Offset.Z), RayMaxLenght);
		if (Result.bIsTraced) {
			ShadowPointMap.Add(Result.EndPointResult);
			index = i;
			break;
		}
	}
	
	
	if (ShadowPointMap.Num() == 1)
	{
		for (int8 i = AmountOfPieces; i > index; i--)
		{
			//FLineTraceResult Result = LineTraceWithOffset(LightStartLocation, FOffsetResultVector(Offset.X - ((Offset.X / 0.5 / AmountOfPieces - 1) * i - AmountOfPieces - 1), Offset.Z), RayMaxLenght);
			FLineTraceResult Result = LineTraceWithOffset(LightStartLocation, FOffsetResultVector(Offset.X - ((Offset.X / 0.5 / AmountOfPieces) * i), Offset.Z), RayMaxLenght);
			if (Result.bIsTraced) {
				ShadowPointMap.Add(Result.EndPointResult);
				return ShadowPointMap;
			}
		}
	}
	
	return TArray<FVector>{};
}

void UAC_ShadowComponent::CreateShadow()
{
	//uint64 StartCycles = FPlatformTime::Cycles64();

	AllShadowsVerticesArray.Empty();

	if (!AreAllTasksComplete())
	{
		UE_LOG(LogTemp, Warning, TEXT("Previous tasks are still running. Count: %d"), ActiveTaskCount.load());
		return;
	}
	AllShadowsVerticesArray.Append(temporaryVerticesArray);
	temporaryVerticesArray.Empty();

	if (CastedLightActors.Num() == 0) return;

	OwnerLocation = GetOwner()->GetActorLocation();
	OwnerForwardVector = GetOwner()->GetActorForwardVector();

	if (!CastedShadeActor || !IsValid(CastedShadeActor))
	{
		UE_LOG(LogTemp, Error, TEXT("ShadeActor is not valid in CreateShadow"));
		return;
	}

	if (bInShadow || CastedLightActors.Num() < CastedShadeActor->MeshComponent->GetNumSections())
	{
		CastedShadeActor->MeshComponent->ClearAllMeshSections();
	}
	bInShadow = true;
	lightLevel = 0;

	WorldPtr = GetWorld();

	CastedShadeActor->UpdateShadowActorMeshTransform(GetOwner()->GetActorTransform());
	MeshLocationVector = CastedShadeActor->GetActorLocation();
	MeshRotator = CastedShadeActor->GetActorRotation() * -1;

	TWeakObjectPtr<UAC_ShadowComponent> WeakThis(this);

	TArray<ALIghtActor*> CastedLightActorsCopy = CastedLightActors;

	ActiveTaskCount.store(CastedLightActorsCopy.Num());
	CastedShadeActor->bCollisionEnabled = false;
	FFunctionGraphTask::CreateAndDispatchWhenReady(
		[WeakThis, CastedLightActorsCopy]() {
			//FGraphEventArray Tasks;
			//Tasks.Reserve(CastedLightActorsCopy.Num());
			for (int32 i = 0; i < CastedLightActorsCopy.Num(); i++)
			{
				
				if (i == CastedLightActorsCopy.Num() - 1) WeakThis->SetShadowCollision(true);

				ALIghtActor* LightActor = CastedLightActorsCopy[i];

				FGraphEventRef Task = FFunctionGraphTask::CreateAndDispatchWhenReady(
					[WeakThis, LightActor, i]()
					{
						if (UAC_ShadowComponent* ValidComponent = WeakThis.Get())
						{
							if (IsValid(ValidComponent))
							{
								ValidComponent->CreateOneShadow(LightActor, i);
							}
						}

						if (UAC_ShadowComponent* ValidComponent = WeakThis.Get())
						{
							ValidComponent->ActiveTaskCount.fetch_sub(1);
						}
					},
					TStatId(),
					nullptr,
					ENamedThreads::AnyHiPriThreadHiPriTask
				);

				if (i == CastedLightActorsCopy.Num() - 2) Task->Wait();
				//Tasks.Add(Task);
			}
			//FTaskGraphInterface::Get().WaitUntilTasksComplete(Tasks);
		},
		TStatId(),
		nullptr,
		ENamedThreads::AnyHiPriThreadHiPriTask
	)->Wait();
	//uint64 EndCycles = FPlatformTime::Cycles64();
	//timer1Value += FPlatformTime::ToSeconds64(EndCycles - StartCycles);

	//timer1Counter += 1;
	
	//UE_LOG(LogTemp, Warning, TEXT("Section %d"), CastedShadeActor->MeshComponent->GetNumSections());
	//UE_LOG(LogTemp, Warning, TEXT("Sadow %d"), CastedLightActors.Num());
	/*
	if (timer1Counter >= 100)
	{
		UE_LOG(LogTemp, Warning, TEXT("High precision time: %f ms"), timer1Value * 1000.0 / timer1Counter);
		UE_LOG(LogTemp, Warning, TEXT("Shadow casters: %d"), CastedLightActors.Num());
		timer1Value = 0;
		timer1Counter = 0;
	}*/
}

void UAC_ShadowComponent::CreateOneShadow(ALIghtActor* LightActor, int32 id)
{
	//UE_LOG(LogTemp, Warning, TEXT("OK!"));

	bool IsPreviousFloorEnebel = false;
	TArray<FVector> VerticesArray;
	TArray<int32> TriangelsArray;
	for (const FVector Offset : MapOfShadow)
	{
		if (!LightActor)
		{
			UE_LOG(LogTemp, Warning, TEXT("LightActor is not valid in CreateOneShadow"));
			return;
		}

		TArray<FVector> ShadowFloor = MakeShadowFloor(Offset, LightActor->GetComponentByClass<USphereComponent>()->GetComponentLocation(), LightActor->GetComponentByClass<USphereComponent>()->GetScaledSphereRadius());
		if (ShadowFloor.Num() != 0)
		{
			for (const FVector FloorPoints : ShadowFloor) {
				temporaryVerticesArray.Add(FloorPoints);
				FVector LocationVector = MeshRotator.RotateVector(FloorPoints - MeshLocationVector);
				VerticesArray.Add(LocationVector.GetSafeNormal() * (LocationVector.Length() - OffsetFromPlane));
			}
			if (ShadowFloor.Num() >= 2) temporaryVerticesArray.Add(FMath::Lerp(ShadowFloor[0], ShadowFloor[1], 0.5f));
			if (IsPreviousFloorEnebel)
			{
				for (int i = VerticesArray.Num() - 4; i <= VerticesArray.Num() - 3; i++)
				{
					for (int j = i; j <= i+2; j++)
					{
						//UE_LOG(LogTemp, Warning, TEXT("j: %d"), j);
						TriangelsArray.Add(j);
					}
				}
			}
			else {
				IsPreviousFloorEnebel = true;
			}
		}
		else {
			IsPreviousFloorEnebel = false;
		}
		
		
		
	}
	if (VerticesArray.Num() == 0 || TriangelsArray.Num() == 0)
	{
		FFunctionGraphTask::CreateAndDispatchWhenReady([this, id]() {
			CastedShadeActor->MeshComponent->ClearMeshSection(id);
			}, TStatId(), nullptr, ENamedThreads::GameThread);

		//UE_LOG(LogTemp, Warning, TEXT("VerticesArray or TriangelsArray is empty!"));
		return;
	}
	else {
		//UE_LOG(LogTemp, Warning, TEXT("VerticesArray or TriangelsArray is NOT empty!"));
		bInShadow = false;
		lightLevel += LightActor->LightLevel;
		CastedShadeActor->UpdateShadowActorMeshes(id, VerticesArray, TriangelsArray);
	}

	/*
	if (AActor* Actor = SoftActor.Get())
	{
		// Перевіряємо чи актор реалізує інтерфейс
		if (Actor->Implements<ULightSoursInterface>())
		{
			// Викликаємо інтерфейс через Execute
			FVector Position = ILightSoursInterface::Execute_GetLightSourPosition(Actor);
			//UE_LOG(LogTemp, Warning, TEXT("Actor [%d] %s Position: %s"), id, *Actor->GetName(), *Position.ToString());
		}
		else
		{
			// Fallback - стандартна позиція актора
			FVector Position = Actor->GetActorLocation();
			//UE_LOG(LogTemp, Warning, TEXT("Actor [%d] %s Position (fallback): %s"), id, *Actor->GetName(), *Position.ToString());
		}
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("Actor [%d]: NULL or not loaded"), id);
	}
	*/
}

