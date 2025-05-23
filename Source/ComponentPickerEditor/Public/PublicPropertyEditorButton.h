// Copyright 2024 Gregor Sönnichsen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SButton.h"

/**
 * Standardized property editor button.
 * 
 * Copied from "PropertyCustomizationHelpers.cpp",
 * since the widget there is not accessible from the outside.
 */
class COMPONENTPICKEREDITOR_API SPublicPropertyEditorButton : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SPublicPropertyEditorButton) 
            : _Text()
            , _Image(FAppStyle::GetBrush("Default"))
            , _IsFocusable(true)
        {}
        SLATE_ATTRIBUTE( FText, Text )
        SLATE_ARGUMENT( const FSlateBrush*, Image )
        SLATE_EVENT( FSimpleDelegate, OnClickAction )

        /** Sometimes a button should only be mouse-clickable and never keyboard focusable. */
        SLATE_ARGUMENT( bool, IsFocusable )
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs)
    {
        OnClickAction = InArgs._OnClickAction;

        ChildSlot
        [
            SNew(SBox)
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Center)
            .WidthOverride(22.0f)
            .HeightOverride(22.0f)
            .ToolTipText(InArgs._Text)
            [
                SNew(SButton)
                .ButtonStyle( FAppStyle::Get(), "SimpleButton")
                .OnClicked( this, &SPublicPropertyEditorButton::OnClick)
                .ContentPadding(0.0f)
                .IsFocusable(InArgs._IsFocusable)
                [ 
                    SNew(SImage)
                    .Image(InArgs._Image)
                    .ColorAndOpacity(FSlateColor::UseForeground())
                ]
            ]
        ]; 
    }

private:
    FReply OnClick()
    {
        (void) OnClickAction.ExecuteIfBound();
        return FReply::Handled();
    }

    FSimpleDelegate OnClickAction;
};
