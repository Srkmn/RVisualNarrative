#include "Graph/EdGraphSchema_RVNDialogue.h"

#include "Graph/EdGraphSchemaAction_RVNDialogue.h"
#include "Graph/GraphEditorCommands.h"
#include "Graph/RVNDialogueGraph.h"
#include "ToolMenu.h"
#include "Graph/Node/RVNStateNode.h"

#define LOCTEXT_NAMESPACE "DialogueGraphSchema"

void UEdGraphSchema_RVNDialogue::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	const FText Category = LOCTEXT("GetGraphContextActionsCategory_StateNode", "Dialogue Node");

	{
		const FText MenuDescGraph_Dialogue = LOCTEXT("GetGraphContextActionsDesc_StateNode", "New State Node");
		const FText ToolTipGraph_Dialogue = LOCTEXT("GetGraphContextActionsTooltip_StateNode",
		                                            "Add New State Node to Graph");

		const TSharedPtr<FEdGraphSchemaAction_Dialogue_State> NewDialogueNode1(
			new FEdGraphSchemaAction_Dialogue_State(Category, MenuDescGraph_Dialogue, ToolTipGraph_Dialogue, 0));

		ContextMenuBuilder.AddAction(NewDialogueNode1);
	}

	{
		const FText MenuDescGraph_Select = LOCTEXT("GetGraphContextActionsDesc_StateNode", "New Select Node");
		const FText ToolTipGraph_Select = LOCTEXT("GetGraphContextActionsTooltip_StateNode",
		                                          "Add New Select Node to Graph");

		const TSharedPtr<FEdGraphSchemaAction_Dialogue_Selector> NewDialogueNode2(
			new FEdGraphSchemaAction_Dialogue_Selector(Category, MenuDescGraph_Select, ToolTipGraph_Select, 0));

		ContextMenuBuilder.AddAction(NewDialogueNode2);
	}
}

void UEdGraphSchema_RVNDialogue::GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	if (!Context || !Context->Graph)
	{
		return;
	}

	if (Context->Node)
	{
		FToolMenuSection& Section = Menu->AddSection("RVNDialogueGraphContextMenuActions",
		                                             LOCTEXT("DialogueGraphContextMenuHeader", "Node Actions"));

		if (!Context->bIsDebugging)
		{
			Section.AddMenuEntry(FRVNGraphEditorCommands::Get().Delete);
			Section.AddMenuEntry(FRVNGraphEditorCommands::Get().Cut);
			Section.AddMenuEntry(FRVNGraphEditorCommands::Get().Copy);
			Section.AddMenuEntry(FRVNGraphEditorCommands::Get().Duplicate);
		}
	}

	Super::GetContextMenuActions(Menu, Context);
}

void UEdGraphSchema_RVNDialogue::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	URVNDialogueGraph* CurrentGraph = CastChecked<URVNDialogueGraph>(&Graph);

	if (CurrentGraph == nullptr)
	{
		return;
	}

	const auto Location = CurrentGraph->GetGoodPlaceForNewNode();
	const auto EntryNode = CurrentGraph->CreateStateNode(ENodeType::ENT_Entry, Location);

	EntryNode->NodePosX = Location.X;
	EntryNode->NodePosY = Location.Y;

	SetNodeMetaData(EntryNode, FNodeMetadata::DefaultGraphNode);

	CurrentGraph->Entry = EntryNode;
}

bool UEdGraphSchema_RVNDialogue::TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const
{
	if (A->GetOwningNode() == B->GetOwningNode())
	{
		return false;
	}

	if (B->Direction == A->Direction)
	{
		if (A->GetOwningNode()->IsA(URVNStateNode::StaticClass()) && B->GetOwningNode()->IsA(
			URVNStateNode::StaticClass()))
		{
			const auto StateA = Cast<URVNStateNode>(A->GetOwningNode());
			const auto StateB = Cast<URVNStateNode>(B->GetOwningNode());

			const auto EntryA = StateA->IsEntryNode();
			const auto EntryB = StateB->IsEntryNode();

			if (!EntryA && !EntryB)
			{
				if (A->Direction == EGPD_Input)
				{
					B = StateB->GetOutputPin();
				}
				else
				{
					B = StateB->GetInputPin();
				}

				return UEdGraphSchema::TryCreateConnection(A, B);
			}

			if (EntryB)
			{
				A = StateA->GetInputPin();
			}

			if (EntryA)
			{
				B = StateB->GetInputPin();
			}
		}
	}

	return UEdGraphSchema::TryCreateConnection(A, B);
}

const FPinConnectionResponse UEdGraphSchema_RVNDialogue::CanCreateConnection(
	const UEdGraphPin* A, const UEdGraphPin* B) const
{
	if (A->GetOwningNode() == B->GetOwningNode())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Cannot connect to self"));
	}

	const URVNStateNode* NodeA = Cast<URVNStateNode>(A->GetOwningNode());
	const URVNStateNode* NodeB = Cast<URVNStateNode>(B->GetOwningNode());

	if (!NodeA || !NodeB)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Invalid node type"));
	}

	const bool bIsEntryA = NodeA->IsEntryNode();
	const bool bIsDialogueA = NodeA->IsDialogueNode();
	const bool bIsSelectA = NodeA->IsSelectNode();

	const bool bIsEntryB = NodeB->IsEntryNode();
	const bool bIsDialogueB = NodeB->IsDialogueNode();
	const bool bIsSelectB = NodeB->IsSelectNode();

	if (bIsEntryA)
	{
		if (A->Direction == EGPD_Output && B->Direction == EGPD_Input && bIsDialogueB)
		{
			return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT("Entry can connect to Dialogue"));
		}

		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW,
		                              TEXT("Entry can only connect output to Dialogue input"));
	}

	if (bIsEntryB)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Cannot connect to Entry node"));
	}

	if (A->Direction == EGPD_Output)
	{
		if (A->HasAnyConnections())
		{
			bool bHasSelectConnection = false;
			bool bHasDialogueConnection = false;

			for (const UEdGraphPin* LinkedPin : A->LinkedTo)
			{
				if (const auto StateNode = Cast<URVNStateNode>(LinkedPin->GetOwningNode()))
				{
					if (StateNode->IsSelectNode() && !bHasSelectConnection)
					{
						bHasSelectConnection = true;
					}
					else if (StateNode->IsDialogueNode())
					{
						bHasDialogueConnection = true;
					}
				}
			}

			if (bHasSelectConnection && bIsDialogueB)
			{
				return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW,
				                              TEXT(
					                              "Next nodes must all be of the same type"));
			}

			if (bHasDialogueConnection && bIsSelectB)
			{
				return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW,
				                              TEXT(
					                              "Next nodes must all be of the same type"));
			}
		}

		if (B->Direction == EGPD_Input && (bIsDialogueB || bIsSelectB))
		{
			return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT("Valid Dialogue output connection"));
		}
	}

	return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Invalid connection"));
}

#undef LOCTEXT_NAMESPACE
