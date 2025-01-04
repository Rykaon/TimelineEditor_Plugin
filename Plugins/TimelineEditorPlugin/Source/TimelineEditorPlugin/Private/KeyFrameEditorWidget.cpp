#include "KeyFrameEditorWidget.h"
#include "KeyFrameEditorWidget.h"
#include "AnimationTimeline.h"
#include "AnimationTimelineWidget.h"
#include "Slate.h"

void SKeyFrameEditorWidget::Construct(const FArguments& InArgs)
{
    ParentWidget = InArgs._ParentWidget;
    Track = InArgs._Track;
    KeyFrame = InArgs._KeyFrame;

    InterpolationOptions.Add(MakeShared<FString>(TEXT("Linear")));
    InterpolationOptions.Add(MakeShared<FString>(TEXT("Quadratic In")));
    InterpolationOptions.Add(MakeShared<FString>(TEXT("Quadratic Out")));
    InterpolationOptions.Add(MakeShared<FString>(TEXT("Quadratic In-Out")));
    InterpolationOptions.Add(MakeShared<FString>(TEXT("Cubic In")));
    InterpolationOptions.Add(MakeShared<FString>(TEXT("Cubic Out")));
    InterpolationOptions.Add(MakeShared<FString>(TEXT("Cubic In-Out")));
    InterpolationOptions.Add(MakeShared<FString>(TEXT("Exponential In")));
    InterpolationOptions.Add(MakeShared<FString>(TEXT("Exponential Out")));
    InterpolationOptions.Add(MakeShared<FString>(TEXT("Exponential In-Out")));
    InterpolationOptions.Add(MakeShared<FString>(TEXT("Sine In")));
    InterpolationOptions.Add(MakeShared<FString>(TEXT("Sine Out")));
    InterpolationOptions.Add(MakeShared<FString>(TEXT("Sine In-Out")));

    SelectedInterpolation = InterpolationOptions[(uint8)KeyFrame->InterpolationType];

    ConstructWidget();
}

void SKeyFrameEditorWidget::SetParentWindow(TWeakPtr<SWindow> InParentWindow)
{
    ParentWindow = InParentWindow;
}

void SKeyFrameEditorWidget::CloseWindow()
{
    if (KeyFrame)
    {
        KeyFrame->IsOpen = false;
    }

    if (ParentWindow.IsValid())
    {
        FSlateApplication::Get().RequestDestroyWindow(ParentWindow.Pin().ToSharedRef());
    }
}

void SKeyFrameEditorWidget::ConstructWidget()
{
    ChildSlot
    [
        SNew(SVerticalBox)

        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(10.0f)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(STextBlock)
                .Text(FText::FromString("Set Time"))
            ]
            + SHorizontalBox::Slot()
            .FillWidth(1.0f)
            [
                SAssignNew(TimeTextBox, SEditableTextBox)
                .Text(FText::AsNumber(KeyFrame->Time))
                .OnTextCommitted_Lambda([this](const FText& NewText, ETextCommit::Type CommitType)
                {
                    OnTimeCommited(NewText, CommitType);
                })
            ]
        ]

        // "Value" Label and TextBoxes
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(10.0f)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(STextBlock)
                .Text(FText::FromString("Value"))
            ]
            + SHorizontalBox::Slot()
            .FillWidth(1.0f)
            [
                ConstrucValueInputFields()
            ]
        ]

        // "Interpolation Type" Label and Dropdown
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(10.0f)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(STextBlock)
                .Text(FText::FromString("Interpolation Type"))
            ]
            + SHorizontalBox::Slot()
            .FillWidth(1.0f)
            [
                SNew(SComboBox<TSharedPtr<FString>>)
                .OptionsSource(&InterpolationOptions)
                .OnGenerateWidget_Lambda([](TSharedPtr<FString> Item)
                {
                    return SNew(STextBlock).Text(FText::FromString(*Item));
                })
                .OnSelectionChanged_Lambda([this](TSharedPtr<FString> NewValue, ESelectInfo::Type)
                {
                    int32 Index = InterpolationOptions.Find(NewValue);
                    if (Index != INDEX_NONE)
                    {
                        KeyFrame->InterpolationType = static_cast<EInterpolationType>(Index);
                        SelectedInterpolation = InterpolationOptions[(uint8)KeyFrame->InterpolationType];
                    }
                })
                .InitiallySelectedItem(SelectedInterpolation)
                [
                    SNew(STextBlock)
                    .Text_Lambda([this]() { return FText::FromString(*SelectedInterpolation); })
                ]
            ]
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(10.0f)
        .HAlign(HAlign_Left)
        [
            SNew(SButton)
            .Text(FText::FromString("Delete KeyFrame"))
            .OnClicked_Lambda([this, AnimationTimeline = ParentWidget, KeyFrameToRemove = KeyFrame, TrackToUpdate = Track]() -> FReply
            {
                AnimationTimeline->RemoveKeyFrame(KeyFrameToRemove->KeyFrameID);
                AnimationTimeline->SetTrackValueAtTime(TrackToUpdate->TrackID, AnimationTimeline->CurrentTime);
                CloseWindow();
                return FReply::Handled();
            })
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(10.0f)
        .HAlign(HAlign_Right)
        [
            SNew(SButton)
            .Text(FText::FromString("Close"))
            .OnClicked_Lambda([this]() -> FReply
            {
                CloseWindow();
                return FReply::Handled();
            })
        ]
    ];
}

TSharedRef<SWidget> SKeyFrameEditorWidget::ConstrucValueInputFields()
{
    if (Track->VariableType == "Vector" || Track->VariableType == "Rotator")
    {
        return SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .FillWidth(1.0f)
            [
                SAssignNew(ValueXTextBox, SEditableTextBox)
                .Text(GetKeyFrameValueAsText(0))
                .OnTextCommitted_Lambda([this](const FText& NewText, ETextCommit::Type CommitType)
                {
                    OnValueTextBoxCommitted(NewText, CommitType, 0);
                })
            ]
            + SHorizontalBox::Slot()
            .FillWidth(1.0f)
            [
                SAssignNew(ValueYTextBox, SEditableTextBox)
                .Text(GetKeyFrameValueAsText(1))
                .OnTextCommitted_Lambda([this](const FText& NewText, ETextCommit::Type CommitType)
                {
                    OnValueTextBoxCommitted(NewText, CommitType, 1);
                })
            ]
            + SHorizontalBox::Slot()
            .FillWidth(1.0f)
            [
                SAssignNew(ValueZTextBox, SEditableTextBox)
                .Text(GetKeyFrameValueAsText(2))
                .OnTextCommitted_Lambda([this](const FText& NewText, ETextCommit::Type CommitType)
                {
                    OnValueTextBoxCommitted(NewText, CommitType, 2);
                })
            ];
    }
    else
    {
        return SAssignNew(ValueXTextBox, SEditableTextBox)
            .Text(GetKeyFrameValueAsText(0))
            .OnTextCommitted_Lambda([this](const FText& NewText, ETextCommit::Type CommitType)
            {
                OnValueTextBoxCommitted(NewText, CommitType, 0);
            });
    }
}

void SKeyFrameEditorWidget::OnTimeCommited(const FText& NewText, ETextCommit::Type CommitType)
{
    if (CommitType == ETextCommit::OnEnter || CommitType == ETextCommit::OnUserMovedFocus)
    {
        FString TypingFieldContent = NewText.ToString();

        if (ParentWidget->IsValidFloat(TypingFieldContent))
        {
            KeyFrame->Time = FCString::Atof(*TypingFieldContent);
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("TIMELINE PLUGIN : Value not set as valid Float!"));
        }

        TimeTextBox->SetText(FText::AsNumber(KeyFrame->Time));
        ParentWidget->SetTrackValueAtTime(Track->TrackID, ParentWidget->CurrentTime);
    }
}

void SKeyFrameEditorWidget::OnValueTextBoxCommitted(const FText& NewText, ETextCommit::Type CommitType, int32 Index)
{
    if (Track->VariableType == "Boolean")
    {
        if (ParentWidget->IsValidBoolean(NewText.ToString()))
        {
            if (NewText.ToString() == "true")
            {
                KeyFrame->BoolValue = true;
            }
            else
            {
                KeyFrame->BoolValue = false;
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("NewText not a valid Boolean"));
        }
    }
    else if (Track->VariableType == "Integer")
    {
        if (ParentWidget->IsValidInteger(NewText.ToString()))
        {
            KeyFrame->IntValue = FCString::Atoi(*NewText.ToString());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("NewText not a valid Integer"));
        }
    }
    else if (Track->VariableType == "Float" || Track->VariableType == "Double")
    {
        if (ParentWidget->IsValidFloat(NewText.ToString()))
        {
            if (Track->VariableType == "Float")
            {
                KeyFrame->FloatValueX = FCString::Atof(*NewText.ToString());
            }
            else
            {
                KeyFrame->DoubleValue = FCString::Atof(*NewText.ToString());
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("NewText not a valid Float"));
        }
    }
    else if (Track->VariableType == "Vector" || Track->VariableType == "Rotator")
    {
        if (ParentWidget->IsValidFloat(NewText.ToString()))
        {
            float Value = FCString::Atof(*NewText.ToString());

            switch (Index)
            {
                case 0: 
                    KeyFrame->FloatValueX = Value;
                    break;
                case 1:
                    KeyFrame->FloatValueY = Value;
                    break;
                case 2:
                    KeyFrame->FloatValueZ = Value;
                    break;
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("NewText not a valid Float"));
        }
    }

    switch (Index)
    {
        case 0:
            ValueXTextBox->SetText(GetKeyFrameValueAsText(Index));
            break;
        case 1:
            ValueYTextBox->SetText(GetKeyFrameValueAsText(Index));
            break;
        case 2:
            ValueZTextBox->SetText(GetKeyFrameValueAsText(Index));
            break;
        default:
            break;
    }

    ParentWidget->SetTrackValueAtTime(Track->TrackID, ParentWidget->CurrentTime);
}

FText SKeyFrameEditorWidget::GetKeyFrameValueAsText(int32 Index)
{
    if (Track->VariableType == "Boolean")
    {
        if (KeyFrame->BoolValue)
        {
            return FText::FromString("true");
        }
        else
        {
            return FText::FromString("false");
        }
    }
    else if (Track->VariableType == "Integer")
    {
        return FText::FromString(FString::FromInt(KeyFrame->IntValue));
    }
    else if (Track->VariableType == "Float")
    {
        return FText::FromString(FString::SanitizeFloat(KeyFrame->FloatValueX));
    }
    else if (Track->VariableType == "Double")
    {
        return FText::FromString(FString::Printf(TEXT("%.15g"), KeyFrame->DoubleValue));
    }
    else if (Track->VariableType == "Vector" || Track->VariableType == "Rotator")
    {
        if (Index == 0)
        {
            return FText::FromString(FString::SanitizeFloat(KeyFrame->FloatValueX));
        }
        else if (Index == 1)
        {
            return FText::FromString(FString::SanitizeFloat(KeyFrame->FloatValueY));
        }
        else if (Index == 2)
        {
            return FText::FromString(FString::SanitizeFloat(KeyFrame->FloatValueZ));
        }
    }

    return FText::FromString("null");
}