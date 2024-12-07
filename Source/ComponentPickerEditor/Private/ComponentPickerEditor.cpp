﻿// Copyright 2024 Gregor Sönnichsen. All Rights Reserved.

#include "ComponentPickerEditor.h"

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