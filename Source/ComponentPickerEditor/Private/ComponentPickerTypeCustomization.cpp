// Copyright 2024 Gregor Sönnichsen.
// Released under the MIT license https://opensource.org/license/MIT/

#include "ComponentPickerTypeCustomization.h"

#include "ComponentPickerSCSEditorUICustomization.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "PublicPropertyEditorButton.h"
#include "SSubobjectBlueprintEditor.h"
#include "SSubobjectEditor.h"
#include "SubobjectDataBlueprintFunctionLibrary.h"
#include "Styling/SlateIconFinder.h"
#include "Toolkits/ToolkitManager.h"

#define LOCTEXT_NAMESPACE "FComponentPickerTypeCustomization"

void FComponentPickerTypeCustomization::CustomizeHeader(
    TSharedRef<IPropertyHandle> PropertyHandle,
     FDetailWidgetRow& HeaderRow,
     IPropertyTypeCustomizationUtils& CustomizationUtils)
{
    PropHandle = PropertyHandle;
    ComponentPropHandle = PropertyHandle->GetChildHandle("Component");
    AllowedClassPropHandle = PropertyHandle->GetChildHandle("AllowedClass");

    BlueprintToolkit = FetchBlueprintEditor(PropertyHandle);

    HeaderRow
    .NameContent()
    [
        PropertyHandle->CreatePropertyNameWidget()
    ]
    .ValueContent()
    .MaxDesiredWidth(FDetailWidgetRow::DefaultValueMaxWidth * 2)
    [
        BuildComponentPicker()
    ];
}

void FComponentPickerTypeCustomization::CustomizeChildren(
    TSharedRef<IPropertyHandle> PropertyHandle,
   IDetailChildrenBuilder& ChildBuilder,
   IPropertyTypeCustomizationUtils& CustomizationUtils)
{
    // no action needed here, but override is required
}

TSharedRef<SWidget> FComponentPickerTypeCustomization::BuildPopupContent()
{
    RebuildClassFilters();
    
    SubobjectEditor = SAssignNew(SubobjectEditor, SSubobjectBlueprintEditor)
        .ObjectContext(this, &FComponentPickerTypeCustomization::HandleGetSubObjectEditorObjectContext)
        .PreviewActor(this, &FComponentPickerTypeCustomization::HandleGetPreviewActor)
        .AllowEditing(false)
        .HideComponentClassCombo(false)
        .OnSelectionUpdated(this, &FComponentPickerTypeCustomization::HandleSelectionUpdated)
        .OnItemDoubleClicked(this, &FComponentPickerTypeCustomization::HandleComponentDoubleClicked);

    SubobjectEditor->SetUICustomization(FComponentPickerSCSEditorUICustomization::GetInstance());

    constexpr float MinPopupWidth = 250.0f;
    constexpr float MinPopupHeight = 200.0f;

    return SNew(SBorder)
        .BorderImage(FAppStyle::Get().GetBrush("Brushes.Secondary"))
        .Padding(2.f, 6.f)
        [
            SNew(SBorder)
            .BorderImage(FAppStyle::Get().GetBrush("Brushes.Recessed"))
            .Padding(4.0f)
            [
                SNew(SBox)
                .MinDesiredWidth(MinPopupWidth)
                .MinDesiredHeight(MinPopupHeight)
                [
                    SubobjectEditor.ToSharedRef()
                ]
            ]
        ];
}

void FComponentPickerTypeCustomization::RebuildClassFilters() const
{
    FComponentPickerSCSEditorUICustomization::GetInstance()
        ->SetComponentTypeFilter
        (
            ExtractAllowedComponentClass(AllowedClassPropHandle)
        );
}

FText FComponentPickerTypeCustomization::HandleGetCurrentComponentName() const
{
    const UActorComponent* ComponentTemplate = ExtractCurrentlyPickedComponent(ComponentPropHandle);
    if (!ComponentTemplate)
    {
        return FText::FromString("None");
    }

    if (auto* SubObjectDataSubsystem = USubobjectDataSubsystem::Get())
    {
        AActor* ActorCDO = FetchActorCDOForProperty(PropHandle);
        
        TArray<FSubobjectDataHandle> SubObjectDataArray;
        SubObjectDataSubsystem->GatherSubobjectData(ActorCDO, SubObjectDataArray);
        
        for (FSubobjectDataHandle& Handle : SubObjectDataArray)
        {
            const FSubobjectData* SubObjectData = Handle.GetData();
            if (SubObjectData->GetComponentTemplate() == ComponentTemplate)
            {
                return FText::FromString(SubObjectData->GetDisplayString(false));
            }
        }
    }

    return FText::FromString(ComponentTemplate->GetName());
}

const FSlateBrush* FComponentPickerTypeCustomization::HandleGetCurrentComponentIcon() const
{
    const UActorComponent* ComponentTemplate = ExtractCurrentlyPickedComponent(ComponentPropHandle);
    return !ComponentTemplate ? nullptr : FSlateIconFinder::FindIconBrushForClass(ComponentTemplate->GetClass(), TEXT("SCS.Component"));
}

TSharedRef<SWidget> FComponentPickerTypeCustomization::BuildComponentPicker()
{
    return SNew(SVerticalBox)
        // component picker
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            .IsEnabled_Lambda([this](){ return BlueprintToolkit != nullptr; })
            .ToolTipText(LOCTEXT("ComponentPickerToolTipText",
                "Pick the component to be accessed later on. Only available in the blueprint actor editor."))
            + SHorizontalBox::Slot()
            .FillWidth(1.f)
            [
                SAssignNew(ComponentListComboButton, SComboButton)
                .ButtonStyle(FAppStyle::Get(), "PropertyEditor.AssetComboStyle")
                .ForegroundColor(FAppStyle::GetColor("PropertyEditor.AssetName.ColorAndOpacity"))
                .OnGetMenuContent(this, &FComponentPickerTypeCustomization::BuildPopupContent)
                .ContentPadding(FMargin(3.0f, 3.0f, 2.0f, 1.0f))
                .ButtonContent()
                [
                    BuildComponentPickerLabel()
                ]
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SPublicPropertyEditorButton)
                .Text(LOCTEXT("ComponentPickerClearButtonText", "Clear"))
                .Image(FAppStyle::GetBrush("Icons.X"))
                .OnClickAction(FSimpleDelegate::CreateLambda([this]
                {
                    TrySetComponent(nullptr);
                }))
                .IsFocusable(false)
            ]
        ]
        // spacer
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SSpacer)
            .Size(FVector2D(0.f, 5.f))
        ]
        // allowed class selection
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            BuildComponentPickerAllowedClassPicker()
        ];
}

TSharedRef<SWidget> FComponentPickerTypeCustomization::BuildComponentPickerAllowedClassPicker() const
{
    if (AllowedClassPropHandle == nullptr)
        return SNew(SBox);
    
    return SNew(SBox)
            .IsEnabled_Lambda([this](){ return BlueprintToolkit == nullptr; })
            .ToolTipText(LOCTEXT("AllowedComponentToolTipText",
                "Choose the component class that is allowed to be picked. Only available in the blueprint component editor."))
            [
                AllowedClassPropHandle->CreatePropertyValueWidget()
            ];
}

TSharedRef<SWidget> FComponentPickerTypeCustomization::BuildComponentPickerLabel()
{
    return SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SSpacer)
            .Size(FVector2D(3.f, 0.f))
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SImage)
            .Image(this, &FComponentPickerTypeCustomization::HandleGetCurrentComponentIcon)
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SSpacer)
            .Size(FVector2D(5.f, 0.f))
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        .VAlign(VAlign_Center)
        [
            SNew(STextBlock)
            .Text(this, &FComponentPickerTypeCustomization::HandleGetCurrentComponentName)
            .Font(IDetailLayoutBuilder::GetDetailFont())
        ]
        + SHorizontalBox::Slot()
        .FillWidth(1.f)
        [
            SNew(SSpacer)
            .Size(FVector2D(1.f, 0.f))
        ];
}

UObject* FComponentPickerTypeCustomization::HandleGetSubObjectEditorObjectContext() const
{
    return BlueprintToolkit->GetSubobjectEditorObjectContext();
}

AActor* FComponentPickerTypeCustomization::HandleGetPreviewActor() const
{
    return BlueprintToolkit->GetPreviewActor();
}

void FComponentPickerTypeCustomization::HandleSelectionUpdated(const TArray<TSharedPtr<FSubobjectEditorTreeNode>>& SelectedNodes)
{
    UActorComponent* EditableComponent = ExtractComponentFromSubobjectNode(SelectedNodes[0]);
    TrySetComponent(EditableComponent);

    ComponentListComboButton->SetIsOpen(false);
}

void FComponentPickerTypeCustomization::HandleComponentDoubleClicked(TSharedPtr<FSubobjectEditorTreeNode> Node)
{
    UActorComponent* EditableComponent = ExtractComponentFromSubobjectNode(Node);
    TrySetComponent(EditableComponent);

    ComponentListComboButton->SetIsOpen(false);
}

UClass* FComponentPickerTypeCustomization::ExtractAllowedComponentClass(
    const TSharedPtr<IPropertyHandle>& PropHandle)
{
    switch (UObject* Value; PropHandle->GetValue(Value))
    {
    case FPropertyAccess::Success:
        return Cast<UClass>(Value);
    default:
        return nullptr;
    }
}

UActorComponent* FComponentPickerTypeCustomization::ExtractCurrentlyPickedComponent(const TSharedPtr<IPropertyHandle>& PropHandle)
{
    switch (UObject* Value; PropHandle->GetValue(Value))
    {
    case FPropertyAccess::Success:
        return Cast<UActorComponent>(Value);
    default:
        return nullptr;
    }
}

UActorComponent* FComponentPickerTypeCustomization::ExtractComponentFromSubobjectNode(
    const FSubobjectEditorTreeNodePtrType& SubobjectNodePtr)
{
    if (!SubobjectNodePtr)
        return nullptr;
    
    const FSubobjectData* NodeData = SubobjectNodePtr->GetDataSource();
    if (!NodeData)
        return nullptr;
    
    const UActorComponent* TmpComponent = NodeData->GetObject<UActorComponent>();
    if (!TmpComponent)
        return nullptr;
    
   return const_cast<UActorComponent*>(TmpComponent);
}

void FComponentPickerTypeCustomization::TrySetComponent(UActorComponent* Component) const
{
    const UClass* AllowedClass = ExtractAllowedComponentClass(AllowedClassPropHandle);
    if (Component != nullptr && AllowedClass != nullptr)
    {
        if (!Component->IsA(AllowedClass)) return;
    }
    
    AActor* ActorCDO = FetchActorCDOForProperty(PropHandle);
    if (!ActorCDO)
        return;

    // Actual transaction starts here
    const FScopedTransaction Transaction(FText::Format(
        LOCTEXT("SetComponentPickerComponentProperty", "Set {0}"),
        ComponentPropHandle->GetPropertyDisplayName()));

    ActorCDO->SetFlags(RF_Transactional);
    ActorCDO->Modify();
    
    ComponentPropHandle->SetValue(Component);
}

AActor* FComponentPickerTypeCustomization::FetchActorCDOForProperty(
    const TSharedPtr<IPropertyHandle>& PropertyHandle)
{
    TArray<UObject*> ObjectArray;
    PropertyHandle->GetOuterObjects(ObjectArray);
    for (UObject* Object : ObjectArray)
    {
        UObject* Outer;
        UObject* NextOuter = Object;

        do
        {
            Outer = NextOuter;

            if (!IsValid(Outer))
                break;

            if (const auto Actor = Cast<AActor>(Outer))
                return Actor;

            for (UObject* EditedAsset : GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->GetAllEditedAssets())
            {
                if (const UBlueprint* EditedBlueprintAsset = Cast<UBlueprint>(EditedAsset))
                {
                    if (EditedBlueprintAsset->GeneratedClass == Outer)
                    {
                        if (UObject* CDO = Cast<UClass>(Outer)->GetDefaultObject())
                        {
                            if (AActor* ActorCDO = Cast<AActor>(CDO))
                            {
                                return ActorCDO;
                            }
                        }
                    }
                }
            }

            NextOuter = Outer->GetOuter();
        }
        while (Outer != NextOuter);
    }
    return nullptr;
}

FBlueprintEditor* FComponentPickerTypeCustomization::FetchBlueprintEditor(
    const TSharedPtr<IPropertyHandle>& PropertyHandle)
{
    const AActor* EditedActor = FetchActorCDOForProperty(PropertyHandle);
    if (!IsValid(EditedActor))
        return nullptr;
    
    const UClass* Class = EditedActor->GetClass();
    if (!IsValid(Class))
        return nullptr;
    
    const UObject* Blueprint = Class->ClassGeneratedBy;
    if (!IsValid(Blueprint))
        return nullptr;
    
    const TSharedPtr<IToolkit> Toolkit = FToolkitManager::Get().FindEditorForAsset(Blueprint);
    if (!Toolkit.IsValid())
        return nullptr;
    
    return StaticCast<FBlueprintEditor*>(Toolkit.Get());
}

#undef LOCTEXT_NAMESPACE
