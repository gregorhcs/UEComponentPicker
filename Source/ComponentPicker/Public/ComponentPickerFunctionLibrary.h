﻿// Copyright 2024 Gregor Sönnichsen.
// Released under the MIT license https://opensource.org/license/MIT/

#pragma once

#include "CoreMinimal.h"
#include "ComponentPickerStruct.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ComponentPickerFunctionLibrary.generated.h"

/**
 * Utility methods for @FComponentPicker.
 */
UCLASS(meta=(BlueprintThreadSafe, ScriptName = "ComponentPickerLibrary"), MinimalAPI)
class UComponentPickerFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /** Get the component selected in a @FComponentPicker. */
    UFUNCTION(
        BlueprintPure,
        meta=(DisplayName="Get (Component Picker)", ScriptMethod="GetPickedComponent", DeterminesOutputType="ComponentClass", DefaultToSelf="Target"),
        Category="Component Picker")
    static COMPONENTPICKER_API UActorComponent* GetPickedComponent(
        const UActorComponent* Target,
        const FComponentPicker& Picker,
        const TSubclassOf<UActorComponent> ComponentClass);
};
