// Copyright 2024 Gregor Sönnichsen.
// Released under the MIT license https://opensource.org/license/MIT/

#include "ComponentPickerEditor.h"
#include "PropertyEditorModule.h"

#include "ComponentPickerTypeCustomization.h"

void FComponentPickerEditorModule::StartupModule()
{
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
    PropertyModule.RegisterCustomPropertyTypeLayout
    (
        TEXT("ComponentPicker"),
        FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FComponentPickerTypeCustomization::MakeInstance)
    );
}

void FComponentPickerEditorModule::ShutdownModule()
{
    if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
    {
        FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
        PropertyModule.UnregisterCustomPropertyTypeLayout(TEXT("ComponentPicker"));
    }
}
    
IMPLEMENT_MODULE(FComponentPickerEditorModule, ComponentPickerEditor)