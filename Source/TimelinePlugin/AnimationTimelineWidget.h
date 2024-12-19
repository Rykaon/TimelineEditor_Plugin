#pragma once

#include "CoreMinimal.h"
#include "Slate.h"
//#include "TimelinePluginWidget.h"
//#include "AnimationTimelineWidget.generated.h"

class SAnimationTimelineWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SAnimationTimelineWidget) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);
    
    void AddTimeline();
    void SetTimelineDuration();
    void SetCurrentTime(float Value);
    FText GetCurrentTimeText() const;
    void OnCurrentTimeCommitted(const FText& NewText, ETextCommit::Type CommitType);

    FReply OnTimelineClicked(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
    float GetTimeFromClick(const FVector2D& ClickPosition) const;
    FText GetDurationText() const;

    FReply OnAddKeyFrame(/*FName VariableName, float Time*/);
    void OnModifyKeyFrame(FName VariableName, int32 KeyIndex, float NewValue);
    void OnDeleteKeyFrame(FName VariableName, int32 KeyIndex);

    class UTimelinePluginWidget* TimelinePluginWidget;
    TSharedPtr<SEditableTextBox> CurrentTimeTextBox;

    float TimelineDuration;

    float CurrentTime;

    float SliderWidth;

    FSlateFontInfo SmallFont;

    TSharedPtr<SScrollBox> TracksScrollBox;
    TSharedPtr<SSlider> TimeSlider;
};