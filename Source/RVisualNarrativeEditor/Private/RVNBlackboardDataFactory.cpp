#include "RVNBlackboardDataFactory.h"
#include "RVNBlackboardData.h"

URVNBlackboardDataFactory::URVNBlackboardDataFactory()
{
	SupportedClass = URVNBlackboardData::StaticClass();

	bCreateNew = true;

	bEditAfterNew = true;
}

UObject* URVNBlackboardDataFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags,
                                                     UObject* Context, FFeedbackContext* Warn)
{
	check(Class->IsChildOf(URVNBlackboardData::StaticClass()));

	return NewObject<URVNBlackboardData>(InParent, Class, Name, Flags);
}

bool URVNBlackboardDataFactory::CanCreateNew() const
{
	return true;
}
