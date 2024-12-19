#include "AnimationTimelineWidget.h"
#include "Slate.h"
#include "TimelinePluginWidget.h"
#include <string>
#include <sstream>
#include <iostream>

void SAnimationTimelineWidget::Construct(const FArguments& InArgs)
{
    SmallFont = FCoreStyle::GetDefaultFontStyle("Regular", 8);
}

void SAnimationTimelineWidget::AddTimeline()
{
    SetTimelineDuration();

    CurrentTimeTextBox = SNew(SEditableTextBox)
        .Text(this, &SAnimationTimelineWidget::GetCurrentTimeText)
        .Font(SmallFont)
        .OnTextCommitted(this, &SAnimationTimelineWidget::OnCurrentTimeCommitted)
        .SelectAllTextWhenFocused(true)
        .MinDesiredWidth(200.0f);

    ChildSlot
        [
            SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    CurrentTimeTextBox.ToSharedRef()
                ]
                + SHorizontalBox::Slot()
                .FillWidth(1.0f)
                [
                    SNew(SBorder)
                        .BorderImage(FCoreStyle::Get().GetBrush("NoBorder"))
                        .Padding(0)
                        .Content()
                        [
                            SNew(SBox)
                                .WidthOverride(SliderWidth)
                                .HeightOverride(30.0f)
                                [
                                    SNew(SImage)
                                        .OnMouseButtonDown(this, &SAnimationTimelineWidget::OnTimelineClicked)
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