#include "AnimationTimelineWidget.h"
#include "Slate.h"

void SAnimationTimelineWidget::Construct(const FArguments& InArgs)
{
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
                    SAssignNew(TracksScrollBox, SScrollBox)
                        + SScrollBox::Slot()
                        [
                            // Exemple d'une track
                            SNew(SHorizontalBox)
                                + SHorizontalBox::Slot()
                                [
                                    SNew(STextBlock).Text(FText::FromString("Track Name"))
                                ]
                                + SHorizontalBox::Slot()
                                [
                                    SNew(SButton)
                                        .Text(FText::FromString("Keyframe"))
                                        .OnClicked(this, &SAnimationTimelineWidget::OnAddKeyFrame)
                                ]
                        ]
                ]
        ];
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