// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#include "TagStatics.h"


///////////////////////////////////////////////////////////////////////////
// Return the index where the tag type was found in the array
int32 FTagStatics::GetTagTypeIndex(const TArray<FName>& InTags, const FString& TagType)
{
	// Init tag index
	int32 TagIndex = INDEX_NONE;

	// Iterate all the tags, check for keyword TagType
	for (const auto& TagItr : InTags)
	{
		// Increment the position of the tag in the array
		++TagIndex;

		// Check if tag is of given type
		if (TagItr.ToString().StartsWith(TagType))
		{
			return TagIndex;
		}
	}
	// return INDEX_NONE if type was not found 
	return INDEX_NONE;
}

// Return the index where the tag type was found in the actor's array
int32 FTagStatics::GetTagTypeIndex(AActor* Actor, const FString& TagType)
{
	return GetTagTypeIndex(Actor->Tags, TagType);
}

// Return the index where the tag type was found in the component's array
int32 FTagStatics::GetTagTypeIndex(UActorComponent* Component, const FString& TagType)
{
	return GetTagTypeIndex(Component->ComponentTags, TagType);
}


///////////////////////////////////////////////////////////////////////////
// Check if key exists in tag
bool FTagStatics::HasKey(const FName& InTag, const FString& TagKey)
{
	// Check if key exist in tag
	if (InTag.ToString().Find(";" + TagKey) != INDEX_NONE)
	{
		return true;
	}
	// Key was not found, return false
	return false;
}

// Check if key exists tag array
bool FTagStatics::HasKey(const TArray<FName>& InTags, const FString& TagType, const FString& TagKey)
{
	// Check if type exists and return index of its location in the array
	int32 TagIndex = FTagStatics::GetTagTypeIndex(InTags, TagType);
	if (TagIndex != INDEX_NONE)
	{
		return FTagStatics::HasKey(InTags[TagIndex], TagKey);
	}
	// Type was not found, return false
	return false;
}

// Check if key exists from actor
bool FTagStatics::HasKey(AActor* Actor, const FString& TagType, const FString& TagKey)
{
	return FTagStatics::HasKey(Actor->Tags, TagType, TagKey);
}

// Check if key exists from component
bool FTagStatics::HasKey(UActorComponent* Component, const FString& TagType, const FString& TagKey)
{
	return FTagStatics::HasKey(Component->ComponentTags, TagType, TagKey);
}

///////////////////////////////////////////////////////////////////////////
// Check if key value pair exists in tag
bool FTagStatics::HasKeyValuePair(const FName& InTag, const FString& TagKey, const FString& TagValue)
{
	// Check if key exist in tag
	if (InTag.ToString().Find(";" + TagKey + "," + TagValue) != INDEX_NONE)
	{
		return true;
	}
	// Key was not found, return false
	return false;
}

// Check if key value pair exists in tag array
bool FTagStatics::HasKeyValuePair(const TArray<FName>& InTags, const FString& TagType, const FString& TagKey, const FString& TagValue)
{
	// Check if type exists and return index of its location in the array
	int32 TagIndex = FTagStatics::GetTagTypeIndex(InTags, TagType);
	if (TagIndex != INDEX_NONE)
	{
		return FTagStatics::HasKeyValuePair(InTags[TagIndex], TagKey, TagValue);
	}
	// Type was not found, return false
	return false;
}

// Check if key value pair exists in actor tags
bool FTagStatics::HasKeyValuePair(AActor* Actor, const FString& TagType, const FString& TagKey, const FString& TagValue)
{
	return FTagStatics::HasKeyValuePair(Actor->Tags, TagType, TagKey, TagValue);
}

// Check if key value pair exists in component tags
bool FTagStatics::HasKeyValuePair(UActorComponent* Component, const FString& TagType, const FString& TagKey, const FString& TagValue)
{
	return FTagStatics::HasKeyValuePair(Component->ComponentTags, TagType, TagKey, TagValue);
}

///////////////////////////////////////////////////////////////////////////
// Get tag key value from tag
FString FTagStatics::GetKeyValue(const FName& InTag, const FString& TagKey)
{
	// Copy of the current tag as FString
	FString CurrTag = InTag.ToString();

	// Check the position of the key string in the tag
	int32 KeyPos = CurrTag.Find(";" + TagKey);
	if (KeyPos != INDEX_NONE)
	{
		// Remove from tag with the cut length of: 
		// pos of the key + length of the semicolon char + length of the key + length of the comma char 
		CurrTag.RemoveAt(0, KeyPos + 1 + TagKey.Len() + 1);
		// Set the tag value as the left side of the string before the semicolon
		return CurrTag.Left(CurrTag.Find(";"));
	}
	// Return empty string if key was not found
	return FString();
}

// Get tag key value from tag array
FString FTagStatics::GetKeyValue(const TArray<FName>& InTags, const FString& TagType, const FString& TagKey)
{
	// Check if type exists and return index of its location in the array
	int32 TagIndex = FTagStatics::GetTagTypeIndex(InTags, TagType);
	if (TagIndex != INDEX_NONE)
	{
		return FTagStatics::GetKeyValue(InTags[TagIndex], TagKey);
	}

	// If type was not found return an empty string
	return FString();
}

// Get tag key value from actor
FString FTagStatics::GetKeyValue(AActor* Actor, const FString& TagType, const FString& TagKey)
{
	return FTagStatics::GetKeyValue(Actor->Tags, TagType, TagKey);
}

// Get tag key value from component
FString FTagStatics::GetKeyValue(UActorComponent* Component, const FString& TagType, const FString& TagKey)
{
	return FTagStatics::GetKeyValue(Component->ComponentTags, TagType, TagKey);
}


///////////////////////////////////////////////////////////////////////////
// Add tag key value from tags, if bReplaceExisting is true, replace existing value
bool FTagStatics::AddKeyValuePair(FName& InTag, const FString& TagKey, const FString& TagValue, bool bReplaceExisting)
{
	// Get the key value
	FString CurrVal = GetKeyValue(InTag, TagKey);
	if (CurrVal.IsEmpty())
	{
		// Key does not exist, add new one at the end
		InTag = FName(*InTag.ToString().Append(TagKey).Append(",").Append(TagValue).Append(";"));
		return true;
	}
	else if (bReplaceExisting)
	{
		// Key exist, replace
		InTag = FName(*InTag.ToString().Replace(*CurrVal, *TagValue));
		return true;
	}
	// Do not replace, return false
	return false;
}

// Add tag key value from tags, if bReplaceExisting is true, replace existing value
bool FTagStatics::AddKeyValuePair(TArray<FName>& InTags, const FString& TagType, const FString& TagKey, const FString& TagValue, bool bReplaceExisting)
{
	// Check if type exists and return index of its location in the array
	int32 TagIndex = FTagStatics::GetTagTypeIndex(InTags, TagType);
	if (TagIndex != INDEX_NONE)
	{
		return FTagStatics::AddKeyValuePair(InTags[TagIndex], TagKey, TagValue, bReplaceExisting);
	}
	else // Type was not found, create a new one
	{
		FString NewTag;
		InTags.Add(FName(*NewTag.Append(TagType).Append(":").Append(TagKey).Append(",").Append(TagValue).Append(";")));
		return true;
	}
	return false;
}

// Add tag key value to actor, if bReplaceExisting is true, replace existing value
bool FTagStatics::AddKeyValuePair(AActor* Actor, const FString& TagType, const FString& TagKey, const FString& TagValue, bool bReplaceExisting)
{
	return FTagStatics::AddKeyValuePair(Actor->Tags, TagType, TagKey, TagValue, bReplaceExisting);
}

// Add tag key value to component, if bReplaceExisting is true, replace existing value
bool FTagStatics::AddKeyValuePair(UActorComponent* Component, const FString& TagType, const FString& TagKey, const FString& TagValue, bool bReplaceExisting)
{
	return FTagStatics::AddKeyValuePair(Component->ComponentTags, TagType, TagKey, TagValue, bReplaceExisting);
}


///////////////////////////////////////////////////////////////////////////
// Remove tag key value from tag
bool FTagStatics::RemoveKeyValuePair(FName& InTag, const FString& TagKey)
{
	// Copy of the current tag as FString
	FString CurrTag = InTag.ToString();
	const FString ToRemove = TagKey + TEXT(",") + GetKeyValue(InTag, TagKey) + TEXT(";");
	int32 FindPos = CurrTag.Find(ToRemove, ESearchCase::CaseSensitive);
	if (FindPos != INDEX_NONE)
	{
		CurrTag.RemoveAt(FindPos, ToRemove.Len());
		InTag = FName(*CurrTag);
	}
	// "TagKey,TagValue;" combo could not be found
	return false;
}

// Remove tag key value from tags
bool FTagStatics::RemoveKeyValuePair(TArray<FName>& InTags, const FString& TagType, const FString& TagKey)
{
	// Check if type exists and return index of its location in the array
	int32 TagIndex = FTagStatics::GetTagTypeIndex(InTags, TagType);
	if (TagIndex != INDEX_NONE)
	{
		return FTagStatics::RemoveKeyValuePair(InTags[TagIndex], TagKey);
	}
	// Tag type not found, nothing to remove
	return false;
}

// Remove tag key value from actor
bool FTagStatics::RemoveKeyValuePair(AActor* Actor, const FString& TagType, const FString& TagKey)
{
	return FTagStatics::RemoveKeyValuePair(Actor->Tags, TagType, TagKey);;
}

// Remove tag key value from component
bool FTagStatics::RemoveKeyValuePair(UActorComponent* Component, const FString& TagType, const FString& TagKey)
{
	return FTagStatics::RemoveKeyValuePair(Component->ComponentTags, TagType, TagKey);
}

// Remove all tag key values from world
bool FTagStatics::RemoveKeyValuePairs(UWorld* World, const FString& TagType, const FString& TagKey)
{
	// Iterate actors from world
	for (TActorIterator<AActor> ActorItr(World); ActorItr; ++ActorItr)
	{
		FTagStatics::RemoveKeyValuePair(*ActorItr, TagType, TagKey);

		// Iterate components of the actor
		for (const auto& CompItr : ActorItr->GetComponents())
		{
			FTagStatics::RemoveKeyValuePair(CompItr, TagType, TagKey);
		}
	}
	return true;
}


///////////////////////////////////////////////////////////////////////////
// Get tag key value pairs from tag array
TMap<FString, FString> FTagStatics::GetKeyValuePairs(const TArray<FName>& InTags, const FString& TagType)
{
	// Map of the tag properties
	TMap<FString, FString> TagProperties;

	// Iterate all the tags, check for keyword TagType
	for (const auto& TagItr : InTags)
	{
		// Copy of the current tag as FString
		FString CurrTag = TagItr.ToString();

		// Check if tag is related to the TagType
		if (CurrTag.RemoveFromStart(TagType))
		{
			// Split on semicolon
			FString CurrPair;
			while (CurrTag.Split(TEXT(";"), &CurrPair, &CurrTag))
			{
				// Split on comma
				FString CurrKey, CurrValue;
				if (CurrPair.Split(TEXT(","), &CurrKey, &CurrValue))
				{
					if (!CurrKey.IsEmpty() && !CurrValue.IsEmpty())
					{
						TagProperties.Emplace(CurrKey, CurrValue);
					}
				}
			}
		}
	}
	return TagProperties;
}

// Get tag key value pairs from actor
TMap<FString, FString> FTagStatics::GetKeyValuePairs(AActor* Actor, const FString& TagType)
{
	return FTagStatics::GetKeyValuePairs(Actor->Tags, TagType);
}

// Get tag key value pairs from component
TMap<FString, FString> FTagStatics::GetKeyValuePairs(UActorComponent* Component, const FString& TagType)
{
	return FTagStatics::GetKeyValuePairs(Component->ComponentTags, TagType);
}


///////////////////////////////////////////////////////////////////////////
// Get all objects (actor and actor components) to tag key value pairs from world
TMap<UObject*, TMap<FString, FString>> FTagStatics::GetObjectsToKeyValuePairs(UWorld* World, const FString& TagType)
{
	// Map of actors to their tag properties
	TMap<UObject*, TMap<FString, FString>> ObjectToTagProperties;
	// Iterate all actors
	for (TActorIterator<AActor> ActorItr(World); ActorItr; ++ActorItr)
	{
		const TMap<FString, FString> ActorTagProperties =
			FTagStatics::GetKeyValuePairs(ActorItr->Tags, TagType);
		// If actor has tag type and at least one property
		if (ActorTagProperties.Num() > 0)
		{
			ObjectToTagProperties.Emplace(*ActorItr, ActorTagProperties);
		}

		// Iterate components of the actor
		for (const auto& CompItr : ActorItr->GetComponents())
		{
			const TMap<FString, FString> CompTagProperties =
				FTagStatics::GetKeyValuePairs(CompItr->ComponentTags, TagType);
			// If tag type has at least one property
			if (CompTagProperties.Num() > 0)
			{
				ObjectToTagProperties.Emplace(CompItr, CompTagProperties);
			}
		}
	}
	return ObjectToTagProperties;
}

// Get all actors to tag key value pairs from world
TMap<AActor*, TMap<FString, FString>> FTagStatics::GetActorsToKeyValuePairs(UWorld* World, const FString& TagType)
{
	// Map of actors to their tag properties
	TMap<AActor*, TMap<FString, FString>> ActorToTagProperties;
	// Iterate all actors
	for (TActorIterator<AActor> ActorItr(World); ActorItr; ++ActorItr)
	{
		const TMap<FString, FString> TagProperties = FTagStatics::GetKeyValuePairs(ActorItr->Tags, TagType);
		// If actor has tag type and at least one property
		if (TagProperties.Num() > 0)
		{
			ActorToTagProperties.Emplace(*ActorItr, TagProperties);
		}
	}
	return ActorToTagProperties;
}

// Get all components to tag key value pairs from world
TMap<UActorComponent*, TMap<FString, FString>> FTagStatics::GetComponentsToKeyValuePairs(UWorld* World, const FString& TagType)
{
	// Map of actors to their tag properties
	TMap<UActorComponent*, TMap<FString, FString>> ComponentToTagProperties;
	// Iterate all actors
	for (TActorIterator<AActor> ActorItr(World); ActorItr; ++ActorItr)
	{
		// Iterate components of the actor
		for (const auto& CompItr : ActorItr->GetComponents())
		{
			const TMap<FString, FString> TagProperties = FTagStatics::GetKeyValuePairs(CompItr->ComponentTags, TagType);
			// If tag type has at least one property
			if (TagProperties.Num() > 0)
			{
				ComponentToTagProperties.Emplace(CompItr, TagProperties);
			}
		}
	}
	return ComponentToTagProperties;
}

/////////////////////////////////////////////////////////////////////////
// Get all objects (actor and actor components) to tag key value
TMap<UObject*, FString> FTagStatics::GetObjectsToKeyValue(UWorld* World, const FString& TagType, const FString& TagKey)
{
	// Map of actors to their tag properties
	TMap<UObject*, FString> ObjectsToKeyValue;
	// Iterate all actors
	for (TActorIterator<AActor> ActorItr(World); ActorItr; ++ActorItr)
	{
		// Add to map if key is found in the actor
		FString Value = FTagStatics::GetKeyValue(*ActorItr, TagType, TagKey);
		if (!Value.IsEmpty())
		{
			ObjectsToKeyValue.Emplace(*ActorItr, Value);
		}

		// Iterate components of the actor
		for (const auto& CompItr : ActorItr->GetComponents())
		{
			// Add to map if key is found in the actor
			FString Value = FTagStatics::GetKeyValue(CompItr, TagType, TagKey);
			if (!Value.IsEmpty())
			{
				ObjectsToKeyValue.Emplace(CompItr, Value);
			}
		}
	}
	return ObjectsToKeyValue;
}

// Get all actors to tag key value
TMap<AActor*, FString> FTagStatics::GetActorsToKeyValue(UWorld* World, const FString& TagType, const FString& TagKey)
{
	// Map of actors to their tag properties
	TMap<AActor*, FString> ActorsToKeyValue;
	// Iterate all actors
	for (TActorIterator<AActor> ActorItr(World); ActorItr; ++ActorItr)
	{
		// Add to map if key is found in the actor
		FString Value = FTagStatics::GetKeyValue(*ActorItr, TagType, TagKey);
		if (!Value.IsEmpty())
		{
			ActorsToKeyValue.Emplace(*ActorItr, Value);
		}
	}
	return ActorsToKeyValue;
}

// Get all components to tag key value
TMap<UActorComponent*, FString> FTagStatics::GetComponentsToKeyValue(UWorld* World, const FString& TagType, const FString& TagKey)
{
	// Map of actors to their tag properties
	TMap<UActorComponent*, FString> ComponentsToKeyValue;
	// Iterate all actors
	for (TActorIterator<AActor> ActorItr(World); ActorItr; ++ActorItr)
	{
		// Iterate components of the actor
		for (const auto& CompItr : ActorItr->GetComponents())
		{
			// Add to map if key is found in the actor
			FString Value = FTagStatics::GetKeyValue(CompItr, TagType, TagKey);
			if (!Value.IsEmpty())
			{
				ComponentsToKeyValue.Emplace(CompItr, Value);
			}
		}
	}
	return ComponentsToKeyValue;
}

/////////////////////////////////////////////////////////////////////////
// Get key values to objects (actor and actor components)
TMap<FString, UObject*> FTagStatics::GetKeyValuesToObject(UWorld* World, const FString& TagType, const FString& TagKey)
{
	// Map of actors to their tag properties
	TMap<FString, UObject*> KeyValuesToObjects;
	// Iterate all actors
	for (TActorIterator<AActor> ActorItr(World); ActorItr; ++ActorItr)
	{
		// Add to map if key is found in the actor
		FString Value = FTagStatics::GetKeyValue(*ActorItr, TagType, TagKey);
		if (!Value.IsEmpty())
		{
			KeyValuesToObjects.Emplace(Value, *ActorItr);
		}

		// Iterate components of the actor
		for (const auto& CompItr : ActorItr->GetComponents())
		{
			// Add to map if key is found in the actor
			FString Value = FTagStatics::GetKeyValue(CompItr, TagType, TagKey);
			if (!Value.IsEmpty())
			{
				KeyValuesToObjects.Emplace(Value, CompItr);
			}
		}
	}
	return KeyValuesToObjects;
}

// Get tag key values to actors
TMap<FString, AActor*> FTagStatics::GetKeyValuesToActor(UWorld* World, const FString& TagType, const FString& TagKey)
{
	// Map of actors to their tag properties
	TMap<FString, AActor*> KeyValuesToActor;
	// Iterate all actors
	for (TActorIterator<AActor> ActorItr(World); ActorItr; ++ActorItr)
	{
		// Add to map if key is found in the actor
		FString Value = FTagStatics::GetKeyValue(*ActorItr, TagType, TagKey);
		if (!Value.IsEmpty())
		{
			KeyValuesToActor.Emplace(Value, *ActorItr);
		}
	}
	return KeyValuesToActor;
}

// Get tag key values to components
TMap<FString, UActorComponent*> FTagStatics::GetKeyValuesToComponents(UWorld* World, const FString& TagType, const FString& TagKey)
{
	// Map of actors to their tag properties
	TMap<FString, UActorComponent*> KeyValuesToComponents;
	// Iterate all actors
	for (TActorIterator<AActor> ActorItr(World); ActorItr; ++ActorItr)
	{
		// Iterate components of the actor
		for (const auto& CompItr : ActorItr->GetComponents())
		{
			// Add to map if key is found in the actor
			FString Value = FTagStatics::GetKeyValue(CompItr, TagType, TagKey);
			if (!Value.IsEmpty())
			{
				KeyValuesToComponents.Emplace(Value, CompItr);
			}
		}
	}
	return KeyValuesToComponents;
}


///////////////////////////////////////////////////////////////////////////
// Get all actors with the key value pair as array
TArray<AActor*> FTagStatics::GetActorsWithKeyValuePair(UWorld* World, const FString& TagType, const FString& TagKey, const FString& TagValue)
{
	// Array of actors
	TArray<AActor*> ActorsWithKeyValuePair;
	// Iterate all actors in the world
	for (TActorIterator<AActor> ActorItr(World); ActorItr; ++ActorItr)
	{
		// Add actor to array if it has tag type with key and value pair
		if (FTagStatics::HasKeyValuePair(ActorItr->Tags, TagType, TagKey, TagValue))
		{
			ActorsWithKeyValuePair.Emplace(*ActorItr);
		}
	}
	return ActorsWithKeyValuePair;
}

// Get all actors with the key value pair as set
TSet<AActor*> FTagStatics::GetActorsWithKeyValuePairAsSet(UWorld* World, const FString& TagType, const FString& TagKey, const FString& TagValue)
{
	// Set of actors
	TSet<AActor*> ActorsWithKeyValuePair;
	// Iterate all actors in the world
	for (TActorIterator<AActor> ActorItr(World); ActorItr; ++ActorItr)
	{
		// Add actor to array if it has tag type with key and value pair
		if (FTagStatics::HasKeyValuePair(ActorItr->Tags, TagType, TagKey, TagValue))
		{
			ActorsWithKeyValuePair.Emplace(*ActorItr);
		}
	}
	return ActorsWithKeyValuePair;
}

// Get all components with the key value pair as array
TArray<UActorComponent*> FTagStatics::GetComponentsWithKeyValuePair(UWorld* World, const FString& TagType, const FString& TagKey, const FString& TagValue)
{
	// Array of components
	TArray<UActorComponent*> ComponentsWithKeyValuePair;
	// Iterate all actors in the world
	for (TActorIterator<AActor> ActorItr(World); ActorItr; ++ActorItr)
	{
		// Iterate actor's components
		for (const auto& CompItr : ActorItr->GetComponents())
		{
			// Add component to container if it has tag type with key and value pair
			if (FTagStatics::HasKeyValuePair(CompItr->ComponentTags, TagType, TagKey, TagValue))
			{
				ComponentsWithKeyValuePair.Emplace(CompItr);
			}
		}
	}
	return ComponentsWithKeyValuePair;
}

// Get all components with the key value pair as set
TSet<UActorComponent*> FTagStatics::GetComponentsWithKeyValuePairAsSet(UWorld* World, const FString& TagType, const FString& TagKey, const FString& TagValue)
{
	// Set of components
	TSet<UActorComponent*> ComponentsWithKeyValuePair;
	// Iterate all actors in the world
	for (TActorIterator<AActor> ActorItr(World); ActorItr; ++ActorItr)
	{
		// Iterate actor's components
		for (const auto& CompItr : ActorItr->GetComponents())
		{
			// Add component to container if it has tag type with key and value pair
			if (FTagStatics::HasKeyValuePair(CompItr->ComponentTags, TagType, TagKey, TagValue))
			{
				ComponentsWithKeyValuePair.Emplace(CompItr);
			}
		}
	}
	return ComponentsWithKeyValuePair;
}


///////////////////////////////////////////////////////////
// Get all actors with the key value pair	
TSet<AActor*> FTagStatics::GetActorSetWithKeyValuePair(UWorld* World, const FString& TagType, const FString& TagKey, const FString& TagValue)
{
	return TSet<AActor*>(GetActorsWithKeyValuePair(World, TagType, TagKey, TagValue));
}

// Gets all components with the key value pair
TSet<UActorComponent*> FTagStatics::GetComponentSetWithKeyValuePair(UWorld* World, const FString& TagType, const FString& TagKey, const FString& TagValue)
{
	TSet<UActorComponent*> ComponentsWithTag;

	for (TActorIterator<AActor> ActorItr(World); ActorItr; ++ActorItr)
	{
		for (auto& Component : ActorItr->GetComponents())
		{
			if (FTagStatics::HasKeyValuePair(Component->ComponentTags, TagType, TagKey, TagValue))
			{
				ComponentsWithTag.Add(Component);
			}
		}
	}
	return ComponentsWithTag;
}
