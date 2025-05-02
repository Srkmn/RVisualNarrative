#pragma once
#include "SGraphPin.h"

class SRVNGraphPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SRVNGraphPin)
		{
		}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin);

protected:
	virtual FSlateColor GetPinColor() const override;
	virtual TSharedRef<SWidget> GetDefaultValueWidget() override;

	const FSlateBrush* GetPinBorder() const;
};
