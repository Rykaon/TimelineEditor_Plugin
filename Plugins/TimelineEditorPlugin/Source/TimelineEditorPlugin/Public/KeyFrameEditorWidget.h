#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "AnimationTimeline.h"
//#include "AnimationTimelineWidget.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Views/SListView.h"
#include "Slate.h"

class SAnimationTimelineWidget;

class TIMELINEPLUGIN_API SKeyFrameEditorWidget  : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SKeyFrameEditorWidget) {}
        SLATE_ARGUMENT(SAnimationTimelineWidget*, ParentWidget)
        SLATE_ARGUMENT(FAnimationTrack*, Track)
        SLATE_ARGUMENT(FAnimationKeyFrame*, KeyFrame)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);
	void SetParentWindow(TWeakPtr<SWindow> InParentWindow);
	void CloseWindow();

	void ConstructWidget();
	TSharedRef<SWidget> ConstrucValueInputFields();

	void OnTimeCommited(const FText& NewText, ETextCommit::Type CommitType);
	void OnValueTextBoxCommitted(const FText& NewText, ETextCommit::Type CommitType, int32 Index);
	FText GetKeyFrameValueAsText(int32 Index);

	// References
	SAnimationTimelineWidget* ParentWidget = nullptr;
    FAnimationTrack* Track = nullptr;
    FAnimationKeyFrame* KeyFrame = nullptr;
    TWeakPtr<SWindow> ParentWindow = nullptr;

	TArray<TSharedPtr<FString>> InterpolationOptions;
	TSharedPtr<FString> SelectedInterpolation;

	TSharedPtr<SEditableTextBox> TimeTextBox;
	TSharedPtr<SEditableTextBox> ValueXTextBox;
	TSharedPtr<SEditableTextBox> ValueYTextBox;
	TSharedPtr<SEditableTextBox> ValueZTextBox;
};
