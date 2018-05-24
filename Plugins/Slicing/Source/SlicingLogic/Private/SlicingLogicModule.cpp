// Copyright 2018, Institute for Artificial Intelligence

#include "SlicingLogicModule.h"

#include "StaticMeshResources.h"
#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"
#include "RawMesh.h"
#include "PhysicsEngine/BodySetup.h"
#include "PhysicsEngine/ConvexElem.h"

#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"

#define LOCTEXT_NAMESPACE "FSlicingLogicModule"

void FSlicingLogicModule::StartupModule()
{
	// Empty as of right now
}

void FSlicingLogicModule::ShutdownModule()
{
	// Empty as of right now
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSlicingLogicModule, Slicing)