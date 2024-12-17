#pragma once

#include "CoreMinimal.h"
#include "Slate.h"
#include "Widgets/SCompoundWidget.h"
//#include "TimelineTrackWidget.generated.h"

class STimelineTrackWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(STimelineTrackWidget) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    TSharedPtr<SScrollBox> TracksScrollBox;
    TSharedPtr<SSlider> TimeSlider; // Pour ajuster la durée de l'animation

    void OnAddKeyframe(FName VariableName, float Time);
    void OnModifyKeyframe(FName VariableName, int32 KeyIndex, float NewValue);
    void OnDeleteKeyframe(FName VariableName, int32 KeyIndex);
};
