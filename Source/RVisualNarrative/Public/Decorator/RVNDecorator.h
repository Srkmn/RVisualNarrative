#pragma once

#include "CoreMinimal.h"
#include "RVNDecorator.generated.h"

class URVNComponent;

UCLASS(abstract)
class RVISUALNARRATIVE_API URVNDecorator : public UObject
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	virtual FString GetNodeIconName() const;

	virtual URVNDecorator* PasteDecorator();
#endif
};
