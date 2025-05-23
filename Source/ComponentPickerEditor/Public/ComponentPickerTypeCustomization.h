// Copyright 2024 Gregor Sönnichsen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BlueprintEditor.h"
#include "SSubobjectEditor.h"
#include "IPropertyTypeCustomization.h"

/**
 * Editor property customization for @FComponentPicker.
 */
class FComponentPickerTypeCustomization final : public IPropertyTypeCustomization
{
public:
    /** Makes a new instance of this detail layout class for a specific detail view requesting it. */
    static TSharedRef<IPropertyTypeCustomization> MakeInstance()
    {
        return MakeShareable(new FComponentPickerTypeCustomization());
    }

    // - IPropertyTypeCustomization
    virtual void CustomizeHeader(
        TSharedRef<IPropertyHandle> PropertyHandle,
        FDetailWidgetRow& HeaderRow,
        IPropertyTypeCustomizationUtils& CustomizationUtils
    ) override;
    virtual void CustomizeChildren(
        TSharedRef<IPropertyHandle> PropertyHandle,
        IDetailChildrenBuilder& ChildBuilder,
        IPropertyTypeCustomizationUtils& CustomizationUtils
    ) override;
    // --
    
protected:
    /** Set the component selected by the picker. */
    void TrySetComponent(UActorComponent* Component) const;

    //////////////////////////////
    /// Widget Builders

    /** Get the to be displayed name of the selected component. */
    FText HandleGetCurrentComponentName() const;

    /** Get the to be displayed icon of the selected component. */
    const FSlateBrush* HandleGetCurrentComponentIcon() const;
    
    /** Build the component picker widget. */
    TSharedRef<SWidget> BuildComponentPicker();
    
    /** Build the allowed class picker widget. */
    TSharedRef<SWidget> BuildComponentPickerAllowedClassPicker() const;
    
    /** Build the content of the component combo box label. */
    TSharedRef<SWidget> BuildComponentPickerLabel();
    
    /** Build the content of the component selector pop-up. */
    TSharedRef<SWidget> BuildPopupContent();

    /** Fill the @ClassFilters array. */
    void RebuildClassFilters() const;

    //////////////////////////////
    /// BP component tree view delegate handlers

    UObject* HandleGetSubObjectEditorObjectContext() const;
    AActor* HandleGetPreviewActor() const;
    void HandleSelectionUpdated(const TArray<TSharedPtr<FSubobjectEditorTreeNode>>& SelectedNodes);
    void HandleComponentDoubleClicked(TSharedPtr<FSubobjectEditorTreeNode> Node);

    //////////////////////////////
    /// Property Handle Extractors
    
    /** Tries to extract a component class value from the given property handle. */
    static UClass* ExtractAllowedComponentClass(const TSharedPtr<IPropertyHandle>& PropHandle);

    /** Tries to extract a component value from the given property handle. */
    static UActorComponent* ExtractCurrentlyPickedComponent(const TSharedPtr<IPropertyHandle>& PropHandle);

    /** Tries to extract the component CDO from the subobject editor node. */
    static UActorComponent* ExtractComponentFromSubobjectNode(const FSubobjectEditorTreeNodePtrType& SubobjectNodePtr);

    /** Walks up the outer chain and fetches the first actor object, if any. */
    static AActor* FetchActorCDOForProperty(const TSharedPtr<IPropertyHandle>& PropertyHandle);
    
    /** Uses the toolkit manager to try and find the blueprint editor with which the actor CDO to the property is edited. */
    static FBlueprintEditor* FetchBlueprintEditor(const TSharedPtr<IPropertyHandle>& PropertyHandle);

private:
    // owning toolkit editor
    FBlueprintEditor* BlueprintToolkit = nullptr;

    // widgets
    TSharedPtr<SBox> AllowedClassBox;
    TSharedPtr<SComboButton> ComponentListComboButton;
    TSharedPtr<SListView<TWeakObjectPtr<UActorComponent>>> ComponentListView;
    TSharedPtr<SSearchBox> SearchBox;
    TSharedPtr<SSubobjectEditor> SubobjectEditor;

    // property handles
    TSharedPtr<IPropertyHandle> PropHandle;
    TSharedPtr<IPropertyHandle> AllowedClassPropHandle;
    TSharedPtr<IPropertyHandle> ComponentPropHandle;
};
