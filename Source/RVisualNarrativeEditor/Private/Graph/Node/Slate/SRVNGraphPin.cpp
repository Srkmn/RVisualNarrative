#include "Graph/Node/Slate/SRVNGraphPin.h"
#include "SlateOptMacros.h"
#include "Styling/CoreStyle.h"
#include "UEVersion.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SRVNGraphPin::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
	this->SetCursor(EMouseCursor::Default);

	bShowLabel = true;

	GraphPinObj = InPin;

	SBorder::Construct(SBorder::FArguments()
	                   .BorderImage(this, &SRVNGraphPin::GetPinBorder)
	                   .BorderBackgroundColor(this, &SRVNGraphPin::GetPinColor)
	                   .OnMouseButtonDown(this, &SRVNGraphPin::OnPinMouseDown)
	                   .Cursor(this, &SRVNGraphPin::GetPinCursor)
	                   .Padding(FMargin(15.0f, 30.f))
	);
}

TSharedRef<SWidget> SRVNGraphPin::GetDefaultValueWidget()
{
	return SNew(STextBlock);
}

const FSlateBrush* SRVNGraphPin::GetPinBorder() const
{
	return
#if UE_APP_STYLE_GET_BRUSH
		FAppStyle::Get().GetBrush
#else
		FEditorStyle::GetBrush
#endif
		(TEXT("Graph.StateNode.Body"));
}

FSlateColor SRVNGraphPin::GetPinColor() const
{
	return FSlateColor(IsHovered() ? FLinearColor(0.4f, 0.2f, 0.2f, 1.0f) : FLinearColor(0.05f, 0.05f, 0.05f, 1.f));
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
