#include "EnemyAIController.h"

void AEnemyAIController::UpdateSightConfig(float NewSightRadius, float NewLoseSightRadius, float NewPeripheralVisionAngle)
{
	UAIPerceptionComponent* PerceptionComp = GetPerceptionComponent();
	if (!PerceptionComp)
	{
		return;
	}

	UAISenseConfig_Sight* SightConfig = GetSightSenseConfig();
	if (!SightConfig)
	{
		return;
	}

	SightConfig->SightRadius = NewSightRadius;
	SightConfig->LoseSightRadius = NewLoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = NewPeripheralVisionAngle;

	PerceptionComp->ConfigureSense(*SightConfig);
}

UAISenseConfig_Sight* AEnemyAIController::GetSightSenseConfig()
{
	UAIPerceptionComponent* PerceptionComp = GetPerceptionComponent();
	if (!PerceptionComp)
	{
		return nullptr;
	}

	FAISenseID SightSenseID = UAISense::GetSenseID(UAISense_Sight::StaticClass());
	UAISenseConfig* BaseConfig = PerceptionComp->GetSenseConfig(SightSenseID);
	return Cast<UAISenseConfig_Sight>(BaseConfig);
}