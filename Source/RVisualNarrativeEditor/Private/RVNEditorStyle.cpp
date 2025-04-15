#include "RVNEditorStyle.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"

TSharedPtr<FSlateStyleSet> FRVNEditorStyle::StyleInstance = nullptr;

void FRVNEditorStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();

		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FRVNEditorStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);

	ensure(StyleInstance.IsUnique());

	StyleInstance.Reset();
}

FName FRVNEditorStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("RVNEditorStyle"));

	return StyleSetName;
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".ttf") ), __VA_ARGS__ )
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".otf") ), __VA_ARGS__ )

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon40x40(40.0f, 40.0f);

TSharedRef<FSlateStyleSet> FRVNEditorStyle::Create()
{
	TSharedRef<FSlateStyleSet> Style = MakeShareable(new FSlateStyleSet("RVNEditorStyle"));

	Style->SetContentRoot(IPluginManager::Get().FindPlugin("RVisualNarrative")->GetBaseDir() / TEXT("Resources"));

	Style->Set("RVNEditor.PluginAction", new IMAGE_BRUSH(TEXT("ButtonIcon_40x"), Icon40x40));

	Style->Set("ClassIcon.RVNBlackboardKeyType_Bool",
	           new FSlateRoundedBoxBrush(FLinearColor(0.300000f, 0.0f, 0.0f, 1.0f), 2.5f, FVector2D(16.f, 5.f)));

	Style->Set("ClassIcon.RVNBlackboardKeyType_Class",
	           new FSlateRoundedBoxBrush(FLinearColor(0.1f, 0.0f, 0.5f, 1.0f), 2.5f, FVector2D(16.f, 5.f)));

	Style->Set("ClassIcon.RVNBlackboardKeyType_Enum",
	           new FSlateRoundedBoxBrush(FLinearColor(0.0f, 0.160000f, 0.131270f, 1.0f), 2.5f, FVector2D(16.f, 5.f)));

	Style->Set("ClassIcon.RVNBlackboardKeyType_Float",
	           new FSlateRoundedBoxBrush(FLinearColor(0.357667f, 1.0f, 0.060000f, 1.0f), 2.5f, FVector2D(16.f, 5.f)));

	Style->Set("ClassIcon.RVNBlackboardKeyType_Int",
	           new FSlateRoundedBoxBrush(FLinearColor(0.013575f, 0.770000f, 0.429609f, 1.0f), 2.5f,
	                                     FVector2D(16.f, 5.f)));

	Style->Set("ClassIcon.RVNBlackboardKeyType_Name",
	           new FSlateRoundedBoxBrush(FLinearColor(0.607717f, 0.224984f, 1.0f, 1.0f), 2.5f, FVector2D(16.f, 5.f)));

	Style->Set("ClassIcon.RVNBlackboardKeyType_NativeEnum",
	           new FSlateRoundedBoxBrush(FLinearColor(0.0f, 0.160000f, 0.131270f, 1.0f), 2.5f, FVector2D(16.f, 5.f)));

	Style->Set("ClassIcon.RVNBlackboardKeyType_Object",
	           new FSlateRoundedBoxBrush(FLinearColor(0.0f, 0.4f, 0.910000f, 1.0f), 2.5f, FVector2D(16.f, 5.f)));

	Style->Set("ClassIcon.RVNBlackboardKeyType_Rotator",
	           new FSlateRoundedBoxBrush(FLinearColor(0.353393f, 0.454175f, 1.0f, 1.0f), 2.5f, FVector2D(16.f, 5.f)));

	Style->Set("ClassIcon.RVNBlackboardKeyType_String",
	           new FSlateRoundedBoxBrush(FLinearColor(1.0f, 0.0f, 0.660537f, 1.0f), 2.5f, FVector2D(16.f, 5.f)));

	Style->Set("ClassIcon.RVNBlackboardKeyType_Vector",
	           new FSlateRoundedBoxBrush(FLinearColor(1.0f, 0.591255f, 0.016512f, 1.0f), 2.5f, FVector2D(16.f, 5.f)));

	return Style;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT

void FRVNEditorStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FRVNEditorStyle::Get()
{
	return *StyleInstance;
}
