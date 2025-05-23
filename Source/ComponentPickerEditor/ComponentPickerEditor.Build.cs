// Copyright 2024 Gregor Sönnichsen.
// Released under the MIT license https://opensource.org/license/MIT/

using UnrealBuildTool;

public class ComponentPickerEditor : ModuleRules
{
    public ComponentPickerEditor(ReadOnlyTargetRules target) : base(target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new[]
            {
                "Core", 
                "Kismet",
                "DetailCustomizations",
                "PropertyEditor",
                "SubobjectEditor",
                "EditorFramework",
                "SubobjectDataInterface", 
                "ComponentPicker", 
                "UnrealEd"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "PropertyEditor"
            }
        );
    }
}