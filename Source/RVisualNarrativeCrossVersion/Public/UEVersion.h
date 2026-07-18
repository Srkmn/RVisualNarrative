#pragma once

#include "Misc/EngineVersionComparison.h"

#define UE_VERSION_START(MajorVersion, MinorVersion, PatchVersion) \
UE_GREATER_SORT(ENGINE_MAJOR_VERSION, MajorVersion, UE_GREATER_SORT(ENGINE_MINOR_VERSION, MinorVersion, UE_GREATER_SORT(ENGINE_PATCH_VERSION, PatchVersion, true)))

#define UE_G_EDITOR_TRANS UE_VERSION_START(5, 1, 0)

#define UE_APP_STYLE_GET_BRUSH UE_VERSION_START(5, 1, 0)

#define UE_APP_STYLE_GET_MARGIN UE_VERSION_START(5, 1, 0)

#define UE_APP_STYLE_GET_FLOAT UE_VERSION_START(5, 1, 0)

#define UE_APP_STYLE_GET_FONT_STYLE UE_VERSION_START(5, 1, 0)

#define UE_APP_STYLE_GET_STYLE_SET_NAME UE_VERSION_START(5, 1, 0)

#define UE_ASSET_DATA_GET_ASSET_CLASS_STRING UE_VERSION_START(5, 1, 0)

#define UE_ASSET_DATA_GET_SOFT_OBJECT_PATH_STRING UE_VERSION_START(5, 1, 0)

#define UE_ANY_PACKAGE UE_VERSION_START(5, 1, 0)

// SGraphNode::MoveTo switched to FVector2f in UE 5.8.
#define UE_S_GRAPH_NODE_MOVE_TO_FLOAT UE_VERSION_START(5, 8, 0)

// The direct OnPostEngineInit delegate was replaced by an accessor in UE 5.8.
#define UE_CORE_DELEGATES_GET_ON_POST_ENGINE_INIT UE_VERSION_START(5, 8, 0)

// SNodePanel no longer consumes ContentScale in UE 5.8.
#define UE_S_NODE_CONTENT_SCALE_DEPRECATED UE_VERSION_START(5, 8, 0)

// Application mode tab factories are registered internally in UE 5.8.
#define UE_APPLICATION_MODE_REGISTER_TAB_FACTORIES_PRIVATE UE_VERSION_START(5, 8, 0)
