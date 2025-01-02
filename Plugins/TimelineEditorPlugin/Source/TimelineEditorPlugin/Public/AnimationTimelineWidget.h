#pragma once

#include "CoreMinimal.h"
#include "Slate.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Widgets/Layout/SConstraintCanvas.h"
#include "AnimationTimeline.h"
#include "Engine/Texture.h"
#include <optional>
//#include "TimelinePluginWidget.h"
//#include "AnimationTimelineWidget.generated.h"

class SAnimationTimelineWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SAnimationTimelineWidget) {}
        SLATE_ARGUMENT(FAnimationTimeline*, AnimationTimeline)
    SLATE_END_ARGS()

    // Constructor
    void Construct(const FArguments& InArgs);

    // Construire et ajouter les widgets
    void AddTimeline();
    void InitializeTimelineTracks();
    void AddTrack(FAnimationTrack& Track);
    void InsertVariableSlotAt(int32 Index, TSharedRef<SWidget> NewWidget);
    void ConstructVariableField(TArray<TSharedRef<SWidget>> Widgets);
    void InsertTrackSlotAt(int32 Index, TSharedRef<SWidget> NewWidget);
    void ConstructTrackField(TArray<TSharedRef<SWidget>> Widgets);
    void AddKeyFrameSlot(int32 KeyFrameID);
    void ConstrucKeyFrameField(TArray<TSharedRef<SWidget>> Widgets);
    void ResetTracks();
    void RemoveTrack(int32 TrackID);
    void RemoveKeyFrame(int32 KeyFrameID);

    // Widgets UTilities
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
    TOptional<FSlateRenderTransform> GetCursorTransform() const;
    TOptional<FSlateRenderTransform> GetKeyFrameTransform(int32 KeyFrameID);

    // Manage Tracks
    FAnimationTrack* GetTrackOfID(int32 TrackID);
    FText GetTrackDefaultValueAsText(int32 TrackID, int32 Index);
    FText GetTrackValueAtTimeAsText(int32 TrackID, float Time, int32 Index);
    std::optional<bool> GetTrackBoolValueAtTime(int32 TrackID, float Time);
    std::optional<int32> GetTrackIntValueAtTime(int32 TrackID, float Time);
    float GetTrackFloatValueAtTime(int32 TrackID, float Time, int32 Index);
    double GetTrackDoubleValueAtTime(int32 TrackID, float Time);

    // Manage KeyFrames
    FAnimationKeyFrame* GetKeyFrameOfID(int32 KeyFrameID);
    void AddKeyFrameToTrack(int32 TrackID, float Time, bool SetValue, FText Value, int32 TextBoxIndex);
    FAnimationKeyFrame* GenerateKeyFrame(int32 TrackID, float Time, bool SetValue, FText Value, int32 TextBoxIndex);
    void OpenKeyFrame(int32 KeyFrameID);
    std::optional<bool> DoesTrackContainKeyFrameAtTime(int32 TrackID, float Time);
    int32 GetIndexOfKeyFrameAtTimeInTrack(int32 TrackID, float Time);
    void SetTracksValueAtTime(float Time);
    void SetKeyFrameValueAtTime(int32 KeyFrameID, float Time);

    // Utilities
    void LogMaps() const;
    void OnTextBoxCommittedForTrack(const FText& NewText, ETextCommit::Type CommitType, int32 TrackID, int32 TextBoxIndex);
    bool IsValidBoolean(const FString& InputString);
    bool IsValidInteger(const FString& InputString);
    bool IsValidFloat(const FString& InputString);
    float GetAlphaAtTime(float Start, float End, float Time);
    float InterpolateFloat(float StartValue, float EndValue, float Alpha, EInterpolationType InterpolationType);
    int32 InterpolateInteger(int32 StartValue, int32 EndValue, float Alpha, EInterpolationType InterpolationType);
    double InterpolateDouble(double StartValue, double EndValue, float Alpha, EInterpolationType InterpolationType);
    FVector InterpolateVector(FVector StartValue, FVector EndValue, float Alpha, EInterpolationType InterpolationType);
    FRotator InterpolateRotator(FRotator StartValue, FRotator EndValue, float Alpha, EInterpolationType InterpolationType);

    // Manage TimelineDuration, CurrentTime et TimeScale
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


    // Ref To TimelinePluginWidget (and TimelinePluginComponent)
    class UTimelinePluginWidget* TimelinePluginWidget;

    // Global Variables and their TextBox
    float CurrentTime;
    TSharedPtr<SEditableTextBox> CurrentTimeTextBox;
    float TimeScale;
    TSharedPtr<SEditableTextBox> TimeScaleTextBox;
    TSharedPtr<SSlider> TimeScaleSlider;
    float TrackFieldWidth;
    TAttribute<FOptionalSize> TrackFieldWidthAttribute;
    TAttribute<FOptionalSize> TrackFieldHeightAttribute;
    TAttribute<FVector2D> BrushImageSizeAttribute;
    
    // Slate Global Instance to construct the Widget
    TSharedPtr<SImage> TimelineUnitTopImage;
    TSharedPtr<SImage> TimelineUnitBottomImage;
    TSharedPtr<SVerticalBox> VariablesField;
    TSharedPtr<SVerticalBox> TracksField;
    TSharedPtr<SScrollBox> TracksScrollBox;
    TSharedPtr<SConstraintCanvas> KeyFramesCanvas;

    // Variables to update TrackedVariables
    FAnimationTimeline* AnimationTimeline;
    TMap<int32, FAnimationTrack*> TracksMap;
    TMap<int32, TSharedRef<SBox>> TracksFieldMap;
    TMap<int32, TSharedRef<SHorizontalBox>> VariableFieldMap;
    TMap<int32, TArray<TSharedPtr<SEditableTextBox>>> ValuesTextBoxMap;
    TMap<int32, TSharedRef<SBox>> KeyFramesMap;

    // Slate Brushes
    FSlateFontInfo SmallFont;
    FSlateBrush TimelineUnitTopBrush;
    FSlateBrush TimelineUnitBottomBrush;
    FSlateBrush TimelineTrackTopBrush;
    FSlateBrush TimelineTrackBrush;
    FSlateBrush TimelineTrackBottomBrush;
    FSlateBrush TimelineCursorBrush;
    FSlateBrush RemoveTrackBrush;
    FSlateBrush AddKeyFrameBrush;
    FSlateBrush KeyFrameDefaultBrush;
    FSlateBrush KeyFrameSelectedBrush;
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