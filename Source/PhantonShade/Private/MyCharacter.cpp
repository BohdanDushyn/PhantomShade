#include "MyCharacter.h"
#include "Components/CapsuleComponent.h"

AMyCharacter::AMyCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	this->Health = 100.0f;
	this->bIsDead = false;
}


float AMyCharacter::TakeDamage(float DamageAmount)
{
	this->Health -= DamageAmount;
	IsDead();
	return this->Health;
}

float AMyCharacter::GetHealth()
{
	return this->Health;
}

void AMyCharacter::SetHealth(float NewHealth)
{
	this->Health = NewHealth;
}

float AMyCharacter::Heal(float HealAmount)
{
	this->Health += HealAmount;
	return this->Health;
}

bool AMyCharacter::IsDead()
{
	if (this->Health <= 0)
	{
		bIsDead = true;

		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	return bIsDead;
}
