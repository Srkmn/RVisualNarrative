#pragma once

#include "CoreMinimal.h"
#include "SGraphPin.h"

class SRVNEntryPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SRVNEntryPin)
		{
		}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin);

	virtual const FSlateBrush* GetPinIcon() const override;
	virtual FSlateColor GetPinTextColor() const override;

protected:
	virtual FSlateColor GetPinColor() const override;
};
