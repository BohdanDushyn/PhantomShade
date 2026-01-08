// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"



AMainCharacter::AMainCharacter() : Super()
{
	this->CharacterLevel = 1.0f;
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 600.0f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCameraComponent"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

}

void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) 
{
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(this->InputMappingContext, 0);
		}
	}

	if (UEnhancedInputComponent* Input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		Input->BindAction(this->TestAction, ETriggerEvent::Triggered, this, &AMainCharacter::TestActionPressed);

		Input->BindAction(this->MoveAction, ETriggerEvent::Triggered, this, &AMainCharacter::Move);
	}
}

void AMainCharacter::TestActionPressed()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Test Action Pressed"));
}

void AMainCharacter::Move(const FInputActionValue& InputValue)
{
	FVector2D InputVector = InputValue.Get<FVector2D>();

	if (IsValid(Controller) && bCanMove)
	{

		//const FRotator Rotation = Controller->GetControlRotation();
		//const FRotator YawRotation(0, Rotation.Yaw, 0);

		//const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		//const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		//
		//InputVector = InputVector.GetSafeNormal();

		////GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::SanitizeFloat(InputVector.Length()));;

		//AddMovementInput(ForwardDirection, InputVector.Length());

		

        InputVector = InputVector.GetSafeNormal();

        //FRotator SpringArmRotation = SpringArm->GetComponentRotation();
        FRotator SpringArmRotation = SpringArm->GetRelativeRotation();

        if (!InputVector.IsZero())
        {
            FVector TDVector(InputVector.X, InputVector.Y, 0.0f);
            FRotator Rotation(0, SpringArmRotation.Yaw + 90, 0);
            FVector Rotated3DVector = FRotationMatrix(Rotation).TransformVector(TDVector);
            InputVector = FVector2D(Rotated3DVector.X, Rotated3DVector.Y);

            float TargetYaw = FMath::RadiansToDegrees(FMath::Atan2(InputVector.Y, InputVector.X));

            FRotator TargetRotation = FRotator(0, TargetYaw, 0);

            /*
            FString RotationString = FString::Printf(TEXT("SpringArm Rotation: Pitch=%.2f, Yaw=%.2f, Roll=%.2f, TargetYaw=%.2f"),
                //SpringArmRotation.Pitch,
                //SpringArmRotation.Yaw,
                //SpringArmRotation.Roll,
                InputVector.X,
				InputVector.Y,
				TargetRotation.Pitch,
                SpringArmRotation.Yaw);
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, RotationString);
            */

            FRotator CurrentRotation = Controller->GetControlRotation();
            FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), 50.0f);
            Controller->SetControlRotation(NewRotation);

            const FVector ForwardDirection = FRotationMatrix(NewRotation).GetUnitAxis(EAxis::X);
            const FVector RightDirection = FRotationMatrix(NewRotation).GetUnitAxis(EAxis::Y);

			AddMovementInput(ForwardDirection, InputVector.Length());
        }

        //InputVector = InputVector.GetSafeNormal();

        //if (!InputVector.IsZero())
        //{
        //    // 1. Отримуємо ротацію SpringArm (лише Yaw)
        //    FRotator SpringArmRotation = SpringArm->GetComponentRotation();
        //    float SpringArmYaw = SpringArmRotation.Yaw;

        //    // 2. Обчислюємо кут руху відносно SpringArm
        //    float InputYaw = FMath::RadiansToDegrees(FMath::Atan2(InputVector.Y, InputVector.X));
        //    float TargetYaw = SpringArmYaw;

        //    // 3. Визначаємо напрямки руху (відносно камери)
        //    const FVector ForwardDirection = FRotationMatrix(SpringArmRotation).GetUnitAxis(EAxis::X);
        //    const FVector RightDirection = FRotationMatrix(SpringArmRotation).GetUnitAxis(EAxis::Y);

        //    // 4. Рухаємо персонажа
        //    AddMovementInput(ForwardDirection, InputVector.Y);
        //    AddMovementInput(RightDirection, InputVector.X);

        //    // 5. Плавний поворот персонажа
        //    FRotator TargetRotation(0, 0, TargetYaw);
        //    FRotator CurrentRotation = GetActorRotation();
        //    FRotator NewRotation = FMath::RInterpTo(
        //        CurrentRotation,
        //        TargetRotation,
        //        GetWorld()->GetDeltaSeconds(),
        //        15.0f // Швидкість повороту
        //    );

        //    SetActorRotation(FRotator(0, NewRotation.Yaw, 0));
        //}
        
	}
}
