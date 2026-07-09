#pragma once

#include "CoreMinimal.h"
#include "MyCharacter.h"
#include "InputActionValue.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "MainCharacter.generated.h"

UCLASS()
class PHANTONSHADE_API AMainCharacter : public AMyCharacter
{
	GENERATED_BODY()
	
public:
	AMainCharacter();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Properties")
	float CharacterLevel = 1.0f;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	bool bCanMove = true;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> OverlapingWithShadowActors; 


	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Enhanched Input")
	class UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, Category = "Enhanched Input")
	class UInputAction* TestAction;

    UPROPERTY(EditAnywhere, Category = "Enhanched Input")
    class UInputAction* MoveAction;

	void TestActionPressed();

	void Move(const FInputActionValue& InputValue);

};
