#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "RVNConditionFactory.generated.h"

class URVNConditionBase;

UCLASS()
class RVISUALNARRATIVEEDITOR_API URVNConditionFactory : public UFactory
{
	GENERATED_BODY()

public:
	URVNConditionFactory();

	UPROPERTY(EditAnywhere, Category=GameplayAbilitiesBlueprintFactory)
	TEnumAsByte<EBlueprintType> BlueprintType;

	UPROPERTY(EditAnywhere, Category=GameplayAbilitiesBlueprintFactory)
	TSubclassOf<URVNConditionBase> ParentClass;

	//~ Begin UFactory Interface
	virtual bool ConfigureProperties() override;

	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags,
	                                  UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;

	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags,
	                                  UObject* Context, FFeedbackContext* Warn) override;
	//~ Begin UFactory Interface
};
