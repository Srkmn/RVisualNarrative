#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Factories/Factory.h"
#include "RVNBlackboardDataFactory.generated.h"

UCLASS()
class URVNBlackboardDataFactory : public UFactory
{
	GENERATED_BODY()

public:
	URVNBlackboardDataFactory();

	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags,
	                                  UObject* Context, FFeedbackContext* Warn) override;

	virtual bool CanCreateNew() const override;
};
