#include "Decorator/RVNDecorator.h"

#if WITH_EDITOR
FString URVNDecorator::GetNodeIconName() const
{
	return TEXT("");
}

URVNDecorator* URVNDecorator::PasteDecorator()
{
	check(0);

	return nullptr;
}
#endif
