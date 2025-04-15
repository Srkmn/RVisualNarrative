#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "RVNComponentFactory.generated.h"

UCLASS()
class RVISUALNARRATIVEEDITOR_API URVNComponentFactory : public UFactory
{
	GENERATED_BODY()

public:
	URVNComponentFactory();

	virtual bool ConfigureProperties() override;

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
	                                  UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
	                                  UObject* Context, FFeedbackContext* Warn) override;

public:
	UClass* ParentClass;
};
