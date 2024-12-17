#pragma once

#include "CoreMinimal.h"
#include "Slate.h"
//#include "AnimationTimelineWidget.generated.h"

class SAnimationTimelineWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SAnimationTimelineWidget) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    TSharedPtr<SScrollBox> TracksScrollBox;
    TSharedPtr<SSlider> TimeSlider;

    void OnAddKeyframe(FName VariableName, float Time);
    void OnModifyKeyframe(FName VariableName, int32 KeyIndex, float NewValue);
    void OnDeleteKeyframe(FName VariableName, int32 KeyIndex);
};