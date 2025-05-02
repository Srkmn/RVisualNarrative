#include "Graph/Node/Slate/SRVNEntryPin.h"
#include "UEVersion.h"

void SRVNEntryPin::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
	SGraphPin::Construct(SGraphPin::FArguments(), InPin);
}

const FSlateBrush* SRVNEntryPin::GetPinIcon() const
{
	return
#if UE_APP_STYLE_GET_BRUSH
		FAppStyle::Get().GetBrush
#else
		FEditorStyle::GetBrush
#endif
		("Icons.Play");
}

FSlateColor SRVNEntryPin::GetPinTextColor() const
{
	return FLinearColor(0.8f, 0.8f, 0.8f, 0.f);
}

FSlateColor SRVNEntryPin::GetPinColor() const
{
	return FSlateColor(FLinearColor::White);
}
