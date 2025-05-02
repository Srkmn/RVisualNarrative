#include "Decorator/Condition/RVNCondition.h"

#if WITH_EDITOR
FString URVNConditionBase::GetNodeIconName() const
{
	return TEXT("Icons.Help");
}

URVNDecorator* URVNConditionBase::PasteDecorator()
{
	return DuplicateObject<URVNConditionBase>(this, GetOuter());
}
#endif

bool URVNConditionBase::CheckCondition_Implementation(URVNComponent* OwnerComponent)
{
	return false;
}
