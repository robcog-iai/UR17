// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#include "ForceBaseIndicatorInterface.h"


// Add default functionality here for any IForceBaseIndicatorInterface functions that are not pure virtual.

FColor IForceBaseIndicatorInterface::LinearColorInterpolate(FColor LowColor, FColor HightColor, float Alpha)
{
	//Calculate a color between two other colors (linear)
	FColor Result = FLinearColor::LerpUsingHSV(LowColor.ReinterpretAsLinear(), HightColor.ReinterpretAsLinear(), Alpha).ToRGBE();
	return Result;
}

float IForceBaseIndicatorInterface::GetForce(FVector Location, FVector Force)
{
	//Claculate the distance between two points
	FVector EndPoint = Location + Force;
	return FVector::Dist(Location, EndPoint);
}
