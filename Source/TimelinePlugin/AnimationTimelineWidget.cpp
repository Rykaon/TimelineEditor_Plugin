#include "AnimationTimelineWidget.h"
#include "Slate.h"
#include "TimelinePluginWidget.h"

void SAnimationTimelineWidget::Construct(const FArguments& InArgs)
{
    SmallFont = FCoreStyle::GetDefaultFontStyle("Regular", 8);
}

void SAnimationTimelineWidget::AddTimeline()
{
    SetTimelineDuration();

    ChildSlot
        [
            SNew(SVerticalBox)
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(STextBlock)
                        .Text(FText::FromString("Animation Timeline"))
                ]
                + SVerticalBox::Slot()
                [
                    SNew(SHorizontalBox)
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            // Frise de temps (timeline)
                            SNew(SBorder)
                                .BorderImage(FCoreStyle::Get().GetBrush("NoBorder"))
                                .Padding(0)
                                .HAlign(HAlign_Fill)
                                .VAlign(VAlign_Center)
                                [
                                    SNew(SBox)
                                        .WidthOverride(SliderWidth)
                                        .HeightOverride(30.0f)
                                        [
                                            SNew(SHorizontalBox)
                                                + SHorizontalBox::Slot()
                                                .FillWidth(1.0f)
                                                [
                                                    // Fond de la frise
                                                    SNew(SBorder)
                                                        .BorderImage(FCoreStyle::Get().GetBrush("BlackBrush"))
                                                ]
                                                + SHorizontalBox::Slot()
                                                .AutoWidth()
                                                [
                                                    // Curseur encapsulé dans un SBox pour la taille
                                                    SNew(SBox)
                                                        .WidthOverride(10.0f)
                                                        .HeightOverride(30.0f)
                                                        [
                                                            SNew(SImage)
                                                                .Image(FCoreStyle::Get().GetBrush("WhiteBrush"))
                                                        ]
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