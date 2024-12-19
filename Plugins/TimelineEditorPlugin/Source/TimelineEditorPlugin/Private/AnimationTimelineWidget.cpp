#include "AnimationTimelineWidget.h"
#include "Slate.h"
#include "TimelinePluginWidget.h"
#include "PaperSprite.h"
#include <string>
#include <sstream>
#include <iostream>

void SAnimationTimelineWidget::Construct(const FArguments& InArgs)
{
    TimelineDuration = 5.0f;
    CurrentTime = 0.0f;
    TimeScale = 100.0f;

    SmallFont = FCoreStyle::GetDefaultFontStyle("Regular", 8);

    TimelineUnitTopBrush.SetResourceObject(LoadObject<UPaperSprite>(nullptr, TEXT("/TimelineEditorPlugin/Textures/SPR_TimelineUnitTop")));
    TimelineUnitTopBrush.ImageSize = FVector2D(100.0f, 30.0f);
    TimelineUnitTopBrush.Tiling = ESlateBrushTileType::Horizontal;

    TimelineUnitBottomBrush.SetResourceObject(LoadObject<UPaperSprite>(nullptr, TEXT("/TimelineEditorPlugin/Textures/SPR_TimelineUnitBottom")));
    TimelineUnitBottomBrush.ImageSize = FVector2D(100.0f, 30.0f);
    TimelineUnitBottomBrush.Tiling = ESlateBrushTileType::Horizontal;

    TimelineTrackTopBrush.SetResourceObject(LoadObject<UPaperSprite>(nullptr, TEXT("/TimelineEditorPlugin/Textures/SPR_TimelineTrackTop")));
    TimelineTrackTopBrush.ImageSize = FVector2D(100.0f, 30.0f);
    TimelineTrackTopBrush.Tiling = ESlateBrushTileType::Horizontal;

    TimelineTrackBrush.SetResourceObject(LoadObject<UPaperSprite>(nullptr, TEXT("/TimelineEditorPlugin/Textures/SPR_TimelineTrack")));
    TimelineTrackBrush.ImageSize = FVector2D(100.0f, 30.0f);
    TimelineTrackBrush.Tiling = ESlateBrushTileType::Horizontal;

    TimelineTrackBottomBrush.SetResourceObject(LoadObject<UPaperSprite>(nullptr, TEXT("/TimelineEditorPlugin/Textures/SPR_TimelineTrackBottom")));
    TimelineTrackBottomBrush.ImageSize = FVector2D(100.0f, 30.0f);
    TimelineTrackBottomBrush.Tiling = ESlateBrushTileType::Horizontal;
}

void SAnimationTimelineWidget::AddTimeline()
{
    SetTimelineDuration();

    CurrentTimeTextBox = SNew(SEditableTextBox)
        .Text(this, &SAnimationTimelineWidget::GetCurrentTimeText)
        .Font(SmallFont)
        .OnTextCommitted(this, &SAnimationTimelineWidget::OnCurrentTimeCommitted)
        .SelectAllTextWhenFocused(true);
        //.MinDesiredWidth(100.0f);

    TimeScaleTextBox = SNew(SEditableTextBox)
        .Text(this, &SAnimationTimelineWidget::GetTimeScaleText)
        .Font(SmallFont)
        .OnTextCommitted(this, &SAnimationTimelineWidget::OnTimeScaleCommitted)
        .SelectAllTextWhenFocused(true);
        //.MinDesiredWidth(100.0f);

    ChildSlot
        [
            SNew(SVerticalBox)
                // Slot pour TimeScaleTextBox et TimeScaleSlider
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SHorizontalBox)
                        // Slot pour TimeScaleTextBox
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SNew(SBox)
                                .WidthOverride(50.0f) // Largeur fixe pour le texte
                                .HeightOverride(30.0f) // Hauteur fixe pour le texte
                                [
                                    TimeScaleTextBox.ToSharedRef()
                                ]
                        ]

                        // Slot pour TimeScaleSlider 
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SNew(SBox)
                                .MinDesiredWidth(200.0f) // Largeur fixe pour le texte
                                .HeightOverride(30.0f) // Hauteur fixe pour le texte
                                [
                                    SAssignNew(TimeScaleSlider, SSlider)
                                        .Value(this, &SAnimationTimelineWidget::GetTimeScaleSliderValue)
                                        .OnValueChanged(this, &SAnimationTimelineWidget::OnTimeScaleSliderValueChanged)
                                        .SliderBarColor(FLinearColor::White)
                                        .Style(&FCoreStyle::Get().GetWidgetStyle<FSliderStyle>("Slider"))
                                ]
                        ]
                ]

                // Slot pour la frise totale
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SHorizontalBox)
                        // Slot pour CurrentTimeTextBox encapsulé dans une SVerticalBox
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SAssignNew(VariablesField, SVerticalBox) // Assigne la SVerticalBox à VariablesField
                                + SVerticalBox::Slot()
                                .AutoHeight()
                                [
                                    SNew(SBox)
                                        .WidthOverride(50.0f) // Largeur fixe pour le texte
                                        .HeightOverride(30.0f) // Hauteur fixe pour le texte
                                        [
                                            CurrentTimeTextBox.ToSharedRef()
                                        ]
                                ]
                        ]

                        // Slot pour la frise scrollable encapsulé dans une SVerticalBox
                        + SHorizontalBox::Slot()
                        .FillWidth(1.0f)
                        [
                            SAssignNew(TracksScrollBox, SScrollBox)
                                .Orientation(Orient_Horizontal) // Scroll horizontal uniquement
                                + SScrollBox::Slot()
                                [
                                    SAssignNew(TracksField, SVerticalBox) // Assigne la SVerticalBox à KeyFramesField
                                        + SVerticalBox::Slot()
                                        .AutoHeight()
                                        [
                                            SNew(SBox)
                                                .MinDesiredWidth(500.0f) // Largeur minimale de la frise
                                                .HeightOverride(30.0f)   // Hauteur fixe de la frise
                                                [
                                                    SNew(SImage)
                                                        .Image(&TimelineUnitTopBrush)
                                                        .OnMouseButtonDown(this, &SAnimationTimelineWidget::OnTimelineClicked)
                                                ]
                                        ]
                                        + SVerticalBox::Slot()
                                        .AutoHeight()
                                        [
                                            SNew(SBox)
                                                .MinDesiredWidth(500.0f) // Largeur minimale de la frise
                                                .HeightOverride(30.0f)   // Hauteur fixe de la frise
                                                [
                                                    SNew(SImage)
                                                        .Image(&TimelineUnitBottomBrush)
                                                        .OnMouseButtonDown(this, &SAnimationTimelineWidget::OnTimelineClicked)
                                                ]
                                        ]

                                ]
                        ]
                ]
        ];
}

void SAnimationTimelineWidget::SetTimelineDuration() 
{
    TimelineDuration = TimelinePluginWidget->TimelineComponent->AnimationTimelineDuration;

    if (TimelineDuration > 1) 
    {
        SliderWidth = TimelineDuration * 50;
    }
    else 
    {
        SliderWidth = 50;
    }
}

void SAnimationTimelineWidget::SetCurrentTime(float Value)
{
    CurrentTime = Value;
}

FText SAnimationTimelineWidget::GetCurrentTimeText() const
{
    return FText::AsNumber(CurrentTime);
}

void SAnimationTimelineWidget::OnCurrentTimeCommitted(const FText& NewText, ETextCommit::Type CommitType)
{
    if (CommitType == ETextCommit::OnEnter || CommitType == ETextCommit::OnUserMovedFocus)
    {
        FString TypingFieldContent = NewText.ToString();

        const std::string& str = TCHAR_TO_UTF8(*TypingFieldContent);
        std::istringstream iss(str);
        float value;
        iss >> value;

        if (!iss.fail() && iss.eof())
        {
            if (value >= 0)
            {
                SetCurrentTime(value);
            }
            else
            {
                UE_LOG(LogTemp, Log, TEXT("TIMELINE PLUGIN : CurrentTime must be a positive number!"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("TIMELINE PLUGIN : CurrentTime not set as valid Float!"));
        }
    }
}

void SAnimationTimelineWidget::SetTimeScale(float Value)
{
    TimeScale = Value;
}

FText SAnimationTimelineWidget::GetTimeScaleText() const
{
    return FText::AsNumber(TimeScale);
}

void SAnimationTimelineWidget::OnTimeScaleCommitted(const FText& NewText, ETextCommit::Type CommitType)
{
    if (CommitType == ETextCommit::OnEnter || CommitType == ETextCommit::OnUserMovedFocus)
    {
        FString TypingFieldContent = NewText.ToString();

        const std::string& str = TCHAR_TO_UTF8(*TypingFieldContent);
        std::istringstream iss(str);
        float value;
        iss >> value;

        if (!iss.fail() && iss.eof())
        {
            if (value >= 10.0f && value <= 200.0f)
            {
                SetTimeScale(value);
            }
            else
            {
                UE_LOG(LogTemp, Log, TEXT("TIMELINE PLUGIN : TimeScale must be a number between 10 and 200!"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("TIMELINE PLUGIN : TimeScale not set as valid Float!"));
        }
    }
}

float SAnimationTimelineWidget::GetTimeScaleSliderValue() const
{
    return (TimeScale - 10.0f) / 190.0f;
}

void SAnimationTimelineWidget::OnTimeScaleSliderValueChanged(float NewValue)
{
    SetTimeScale(FMath::Lerp(10.0f, 200.0f, NewValue));
}

FReply SAnimationTimelineWidget::OnTimelineClicked(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    FVector2D LocalClickPosition = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
    CurrentTime = GetTimeFromClick(LocalClickPosition);
    return FReply::Handled();
}

float SAnimationTimelineWidget::GetTimeFromClick(const FVector2D& ClickPosition) const
{
    float ClickX = ClickPosition.X;
    return FMath::Clamp((ClickX / SliderWidth) * TimelineDuration, 0.0f, TimelineDuration);
}

/*FText SAnimationTimelineWidget::GetCurrentTimeText() const
{
    return FText::Format(NSLOCTEXT("AnimationTimelineWidget", "CurrentTime", "Current Time: {0} s"), FText::AsNumber(CurrentTime));
}*/

FText SAnimationTimelineWidget::GetDurationText() const
{
    return FText::Format(NSLOCTEXT("AnimationTimelineWidget", "Duration", "Total Duration: {0} s"), FText::AsNumber(TimelineDuration));
}

FReply SAnimationTimelineWidget::OnAddKeyFrame(/*FName VariableName, float Time*/)
{
    return FReply::Handled();
}

void SAnimationTimelineWidget::OnModifyKeyFrame(FName VariableName, int32 KeyIndex, float NewValue)
{

}

void SAnimationTimelineWidget::OnDeleteKeyFrame(FName VariableName, int32 KeyIndex)
{

}