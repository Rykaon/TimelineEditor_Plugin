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
    
    // Construire et ajouter les widgets
    void AddTimeline();

    // Modifier et manager TimelineDuration, CurrentTime et TimeScale
    void SetTimelineDuration();
    void SetCurrentTime(float Value);
    FText GetCurrentTimeText() const;
    void OnCurrentTimeCommitted(const FText& NewText, ETextCommit::Type CommitType);
    void SetTimeScale(float Value);
    FText GetTimeScaleText() const;
    void OnTimeScaleCommitted(const FText& NewText, ETextCommit::Type CommitType);
    float GetTimeScaleSliderValue() const;
    void OnTimeScaleSliderValueChanged(float NewValue);

    FReply OnTimelineClicked(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
    float GetTimeFromClick(const FVector2D& ClickPosition) const;
    FText GetDurationText() const;

    FReply OnAddKeyFrame(/*FName VariableName, float Time*/);
    void OnModifyKeyFrame(FName VariableName, int32 KeyIndex, float NewValue);
    void OnDeleteKeyFrame(FName VariableName, int32 KeyIndex);

    class UTimelinePluginWidget* TimelinePluginWidget;
    TSharedPtr<SEditableTextBox> CurrentTimeTextBox;
    TSharedPtr<SEditableTextBox> TimeScaleTextBox;
    TSharedPtr<SSlider> TimeScaleSlider;
    
    TSharedPtr<SVerticalBox> VariablesField;
    TSharedPtr<SVerticalBox> TracksField;
    TSharedPtr<SScrollBox> TracksScrollBox;

    float TimelineDuration;
    float CurrentTime;
    float TimeScale;

    float SliderWidth;

    FSlateFontInfo SmallFont;
    FSlateBrush TimelineUnitTopBrush;
    FSlateBrush TimelineUnitBottomBrush;
    FSlateBrush TimelineTrackTopBrush;
    FSlateBrush TimelineTrackBrush;
    FSlateBrush TimelineTrackBottomBrush;
};