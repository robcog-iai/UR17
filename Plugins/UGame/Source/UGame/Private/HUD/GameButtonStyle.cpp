// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "GameButtonStyle.h"
#include "UGame.h"
#include "SlateGameResources.h" 

TSharedPtr<FSlateStyleSet> FGameButtonStyles::GameButtonStyleInstance = NULL;

void FGameButtonStyles::Initialize()
{
	if (!GameButtonStyleInstance.IsValid())
	{
		GameButtonStyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*GameButtonStyleInstance);
	}
}

void FGameButtonStyles::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*GameButtonStyleInstance);
	ensure(GameButtonStyleInstance.IsUnique());
	GameButtonStyleInstance.Reset();
}

FName FGameButtonStyles::GetStyleSetName()
{
	static FName StyleSetName(TEXT("GameBUttonStyles"));
	return StyleSetName;
}

TSharedRef<FSlateStyleSet> FGameButtonStyles::Create()
{
	TSharedRef<FSlateStyleSet> StyleRef = FSlateGameResources::New(FGameButtonStyles::GetStyleSetName(), "/Game/UI/Styles", "/Game/UI/Styles");
	return StyleRef;
}

const ISlateStyle& FGameButtonStyles::Get()
{
	return *GameButtonStyleInstance;
}