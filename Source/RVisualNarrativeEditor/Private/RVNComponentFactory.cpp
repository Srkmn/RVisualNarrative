#include "RVNComponentFactory.h"
#include "Blueprint/RVNAssetBlueprint.h"
#include "RVNComponent.h"

URVNComponentFactory::URVNComponentFactory()
{
	bCreateNew = true;

	bEditAfterNew = true;

	SupportedClass = URVNAssetBlueprint::StaticClass();

	ParentClass = UActorComponent::StaticClass();
}

bool URVNComponentFactory::ConfigureProperties()
{
	return true;
}

UObject* URVNComponentFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
                                                UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	return FactoryCreateNew(InClass, InParent, InName, Flags, Context, Warn);
}

UObject* URVNComponentFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
                                                UObject* Context, FFeedbackContext* Warn)
{
	check(InClass->IsChildOf(URVNAssetBlueprint::StaticClass()));

	const auto NewBP = NewObject<URVNAssetBlueprint>(InParent, InClass, InName, Flags);

	NewBP->ParentClass = URVNComponent::StaticClass();
	NewBP->BlueprintType = BPTYPE_Normal;
	NewBP->bIsNewlyCreated = true;
	NewBP->bLegacyNeedToPurgeSkelRefs = false;
	NewBP->GenerateNewGuid();

	return NewBP;
}
