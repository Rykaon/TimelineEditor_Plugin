#pragma once

#include "CoreMinimal.h"
#include "Slate.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/Texture.h"
//#include "TimelinePluginWidget.h"
//#include "AnimationTimelineWidget.generated.h"

class SAnimationTimelineWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SAnimationTimelineWidget) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);
    void InvalidateWidget(TSharedPtr<SWidget> Widget);
    FOptionalSize GetTrackFieldWidth() const;
    FOptionalSize GetTracksFieldHeight() const;
    FVector2D GetBrushImageSize() const;
    const FSlateBrush* GetTimelineUnitTopBrush() const;
    const FSlateBrush* GetTimelineUnitBottomBrush() const;
    const FSlateBrush* GetTimelineTrackTopBrush() const;
    const FSlateBrush* GetTimelineTrackBrush() const;
    const FSlateBrush* GetTimelineTrackBottomBrush() const;
    const FSlateBrush* GetTimelineCursorBrush() const;
    void SetBrushImageSize(float WidthValue, float HeightValue);

    
    // Construire et ajouter les widgets
    void AddTimeline();

    // Modifier et manager TimelineDuration, CurrentTime et TimeScale
    void SetTimelineDuration();
    void SetCurrentTime(float Value);
    void SetTimeScale(float Value);
    FText GetCurrentTimeText() const;
    void OnCurrentTimeCommitted(const FText& NewText, ETextCommit::Type CommitType);
    FText GetTimeScaleText() const;
    void OnTimeScaleCommitted(const FText& NewText, ETextCommit::Type CommitType);
    float GetTimeScaleSliderValue() const;
    void OnTimeScaleSliderValueChanged(float NewValue);

    FReply OnTimelineClicked(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
    float GetTimeFromClick(const FVector2D& ClickPosition) const;
    TOptional<FSlateRenderTransform> GetCursorTransform() const;
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

    TSharedPtr<SImage> TimelineUnitTopImage;
    TSharedPtr<SImage> TimelineUnitBottomImage;

    float TimelineDuration;
    float CurrentTime;
    float TimeScale;
    TAttribute<FVector2D> BrushImageSizeAttribute;
    float TrackFieldWidth;
    TAttribute<FOptionalSize> TrackFieldWidthAttribute;
    TAttribute<FOptionalSize> TrackFieldHeightAttribute;

    FSlateFontInfo SmallFont;
    FSlateBrush TimelineUnitTopBrush;
    FSlateBrush TimelineUnitBottomBrush;
    FSlateBrush TimelineTrackTopBrush;
    FSlateBrush TimelineTrackBrush;
    FSlateBrush TimelineTrackBottomBrush;
    FSlateBrush TimelineCursorBrush;
    UMaterialInterface* TimelineUnitTopMaterial;
    UMaterialInterface* TimelineUnitBottomMaterial;
    UMaterialInterface* TimelineTrackTopMaterial;
    UMaterialInterface* TimelineTrackMaterial;
    UMaterialInterface* TimelineTrackBottomMaterial;
    UMaterialInstanceDynamic* TimelineUnitTopMaterialDynamic;
    UMaterialInstanceDynamic* TimelineUnitBottomMaterialDynamic;
    UMaterialInstanceDynamic* TimelineTrackTopMaterialDynamic;
    UMaterialInstanceDynamic* TimelineTrackMaterialDynamic;
    UMaterialInstanceDynamic* TimelineTrackBottomMaterialDynamic;
    TAttribute<const FSlateBrush*> TimelineUnitTopBrushAttribute;
    TAttribute<const FSlateBrush*> TimelineUnitBottomBrushAttribute;
    TAttribute<const FSlateBrush*> TimelineTrackTopBrushAttribute;
    TAttribute<const FSlateBrush*> TimelineTrackBrushAttribute;
    TAttribute<const FSlateBrush*> TimelineTrackBottomBrushAttribute;
    TAttribute<const FSlateBrush*> TimelineCursorBrushAttribute;
};