// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "GameGlobalButtonStyle.h" 
#include "UGame.h" 

void FGlobalStyle::GetResources(TArray<const FSlateBrush*>& OutBrushes) const
{
}

const FName FGlobalStyle::TypeName = TEXT("FGlobalStyle");

const FName FGlobalStyle::GetTypeName() const
{
	static const FName TypeName = TEXT("FGlobalStyle");
	return TypeName;
}

const FGlobalStyle& FGlobalStyle::GetDefault()
{
	static FGlobalStyle Default;
	return Default;
}

UGameGlobalButtonStyle::UGameGlobalButtonStyle(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}