# Realistic Walking  
* The realistic walking for the character is implemented in [GMovment.h](https://github.com/wzeitler/UR17/blob/ur17-p4/Source/UGame/Private/Character/Components/GMovement.h) 
and [GMovment.cpp](https://github.com/wzeitler/UR17/blob/ur17-p4/Source/UGame/Private/Character/Components/GMovement.cpp). GMovment is the movment component which handles the movment with the keyboard button W, A, S and D.

* For the realisitc movment the curved function [PT1-Glied](https://de.wikipedia.org/wiki/PT1-Glied) was used, 
which increasses the movment speed until the maximum threshold is reached. [The TickComponent checks if the character is still moving and if not resets the speed up.](https://github.com/wzeitler/UR17/blob/ur17-p4/Source/UGame/Private/Character/Components/GMovement.cpp#L69-L80)

```
// Called every frame
void UGMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    // Check if the character is still moving. If not reset the CurrentSpeed and SpeedUpTime. 
    if (Character->GetVelocity() == FVector::ZeroVector && bIsMoving) {
        CurrentSpeed = 0;
        SpeedUpTime = 0.05;
        bIsMoving = false;
        UE_LOG(LogTemp, Warning, TEXT("Reset Speed!"));
    }
}
```

[The actuel speed up is handled in the speed up function, which increassed the SpeedUpTime. For the calculation of the current speed the PT1-Glied function, in CalculateNewSpeed, computes the new speed. The new speed has to be adjusted to the correct direction and if necessary needs to be inverted.](https://github.com/wzeitler/UR17/blob/ur17-p4/Source/UGame/Private/Character/Components/GMovement.cpp#L102-142)

```
float UGMovement::CalculateNewSpeed(float TimeStep)
{
    // The variables are fixed and change the curve of the function, if changed
    int TVariable = 1;

    // Variables to hold the values from the function
    float Tmp = -(TimeStep / TVariable);
    float BracePart = (1 - FMath::Exp(Tmp));
    float NewSpeed = MaxMovementSpeed * BracePart;

    return NewSpeed;
}

void UGMovement::SpeedUp(const FVector Direction, const float Val)
{
    // Recalculate the current speed (by Waldemar Zeitler)
    if (!bIsMoving) {
        bIsMoving = true;
    }

    // Rising speed up
    if (SpeedUpTime < 2) {
        SpeedUpTime += GetWorld()->GetDeltaSeconds();
    }

    // If the max speed is not reached calculate the new and faster speed
    if (CurrentSpeed <= MaxMovementSpeed)
    {
        CurrentSpeed = CalculateNewSpeed(SpeedUpTime) + MinMovementSpeed;
        CurrentSpeed = CurrentSpeed * Val;
        UE_LOG(LogTemp, Warning, TEXT("Current Speed: %f"), CurrentSpeed);
    }

    // If the max speed is reached the movment for a negative/direction has to be switched accordingly
    if ((CurrentSpeed < 0 && Val > 0) || (CurrentSpeed > 0 && Val < 0))
    {
        CurrentSpeed = -CurrentSpeed;
    }

    Character->AddMovementInput(Direction, CurrentSpeed);
}
```
