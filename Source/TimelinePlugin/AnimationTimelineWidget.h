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

    FReply OnAddKeyFrame(/*FName VariableName, float Time*/);
    void OnModifyKeyFrame(FName VariableName, int32 KeyIndex, float NewValue);
    void OnDeleteKeyFrame(FName VariableName, int32 KeyIndex);

    class UTimelinePluginWidget* TimelinePluginWidget;

    float TimelineDuration;

    float CurrentTime;

    float SliderWidth;

    FSlateFontInfo SmallFont;

    TSharedPtr<SScrollBox> TracksScrollBox;
    TSharedPtr<SSlider> TimeSlider;
};