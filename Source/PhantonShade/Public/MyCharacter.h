#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

UCLASS()
class PHANTONSHADE_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMyCharacter();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Properties")
	float Health = 100.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Properties")
	bool bIsDead = false;

	float TakeDamage(float DamageAmount);
	float GetHealth();
	void SetHealth(float NewHealth);
	float Heal(float HealAmount);

	bool IsDead();
};
