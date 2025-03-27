#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FRVisualNarrativeModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;

	virtual void ShutdownModule() override;

	static FRVisualNarrativeModule& Get();
};
