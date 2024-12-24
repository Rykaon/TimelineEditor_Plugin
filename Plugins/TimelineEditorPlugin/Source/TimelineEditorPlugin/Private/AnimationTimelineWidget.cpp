#include "AnimationTimelineWidget.h"
#include "Slate.h"
#include "TimelinePluginWidget.h"
#include "PaperSprite.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/Texture.h"
#include <string>
#include <sstream>
#include <iostream>

void SAnimationTimelineWidget::Construct(const FArguments& InArgs)
{
    TimelineDuration = 5.0f;
    CurrentTime = 0.0f;
    TimeScale = 100.0f;
    TrackFieldWidth = (TimelineDuration * 100.0f) * (100.0f / TimeScale);

    BrushImageSizeAttribute = TAttribute<FVector2D>::Create(TAttribute<FVector2D>::FGetter::CreateSP(this, &SAnimationTimelineWidget::GetBrushImageSize));
    TrackFieldWidthAttribute = TAttribute<FOptionalSize>::Create(TAttribute<FOptionalSize>::FGetter::CreateSP(this, &SAnimationTimelineWidget::GetTrackFieldWidth));

    TimelineUnitTopMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/TimelineEditorPlugin/Textures/M_TimelineUnitTop.M_TimelineUnitTop"));
    TimelineUnitBottomMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/TimelineEditorPlugin/Textures/M_TimelineUnitBottom.M_TimelineUnitBottom"));
    TimelineTrackTopMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/TimelineEditorPlugin/Textures/M_TimelineTrackTop.M_TimelineTrackTop"));
    TimelineTrackMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/TimelineEditorPlugin/Textures/M_TimelineTrack.M_TimelineTrack"));
    TimelineTrackBottomMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/TimelineEditorPlugin/Textures/M_TimelineTrackBottom.M_TimelineTrackBottom"));

    if (!TimelineUnitTopMaterial)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load material for TimelineUnitTop"));
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("Successfully loaded material for TimelineUnitTop"));
    }

    TimelineUnitTopMaterialDynamic = UMaterialInstanceDynamic::Create(TimelineUnitTopMaterial, nullptr);
    TimelineUnitBottomMaterialDynamic = UMaterialInstanceDynamic::Create(TimelineUnitBottomMaterial, nullptr);
    TimelineTrackTopMaterialDynamic = UMaterialInstanceDynamic::Create(TimelineTrackTopMaterial, nullptr);
    TimelineTrackMaterialDynamic = UMaterialInstanceDynamic::Create(TimelineTrackMaterial, nullptr);
    TimelineTrackBottomMaterialDynamic = UMaterialInstanceDynamic::Create(TimelineTrackBottomMaterial, nullptr);

    TimelineUnitTopBrushAttribute = TAttribute<const FSlateBrush*>::Create(TAttribute<const FSlateBrush*>::FGetter::CreateSP(this, &SAnimationTimelineWidget::GetTimelineUnitTopBrush));
    TimelineUnitBottomBrushAttribute = TAttribute<const FSlateBrush*>::Create(TAttribute<const FSlateBrush*>::FGetter::CreateSP(this, &SAnimationTimelineWidget::GetTimelineUnitBottomBrush));
    TimelineTrackTopBrushAttribute = TAttribute<const FSlateBrush*>::Create(TAttribute<const FSlateBrush*>::FGetter::CreateSP(this, &SAnimationTimelineWidget::GetTimelineTrackTopBrush));
    TimelineTrackBrushAttribute = TAttribute<const FSlateBrush*>::Create(TAttribute<const FSlateBrush*>::FGetter::CreateSP(this, &SAnimationTimelineWidget::GetTimelineTrackBrush));
    TimelineTrackBottomBrushAttribute = TAttribute<const FSlateBrush*>::Create(TAttribute<const FSlateBrush*>::FGetter::CreateSP(this, &SAnimationTimelineWidget::GetTimelineTrackBottomBrush));

    SmallFont = FCoreStyle::GetDefaultFontStyle("Regular", 8);

    //TimelineUnitTopBrush.SetResourceObject(LoadObject<UTexture2D>(nullptr, TEXT("/TimelineEditorPlugin/Textures/T_TimelineUnitTop")));
    TimelineUnitTopBrush.SetResourceObject(TimelineUnitTopMaterialDynamic);
    TimelineUnitTopBrush.Tiling = ESlateBrushTileType::Horizontal;

    //TimelineUnitBottomBrush.SetResourceObject(LoadObject<UTexture2D>(nullptr, TEXT("/TimelineEditorPlugin/Textures/T_TimelineUnitBottom")));
    TimelineUnitBottomBrush.SetResourceObject(TimelineUnitBottomMaterialDynamic);
    TimelineUnitBottomBrush.Tiling = ESlateBrushTileType::Horizontal;

    //TimelineTrackTopBrush.SetResourceObject(LoadObject<UTexture2D>(nullptr, TEXT("/TimelineEditorPlugin/Textures/T_TimelineTrackTop")));
    TimelineTrackTopBrush.SetResourceObject(TimelineTrackTopMaterialDynamic);
    TimelineTrackTopBrush.Tiling = ESlateBrushTileType::Horizontal;

    //TimelineTrackBrush.SetResourceObject(LoadObject<UTexture2D>(nullptr, TEXT("/TimelineEditorPlugin/Textures/T_TimelineTrack")));
    TimelineTrackBrush.SetResourceObject(TimelineTrackMaterialDynamic);
    TimelineTrackBrush.Tiling = ESlateBrushTileType::Horizontal;

    //TimelineTrackBottomBrush.SetResourceObject(LoadObject<UTexture2D>(nullptr, TEXT("/TimelineEditorPlugin/Textures/T_TimelineTrackBottom")));
    TimelineTrackBottomBrush.SetResourceObject(TimelineTrackBottomMaterialDynamic);
    TimelineTrackBottomBrush.Tiling = ESlateBrushTileType::Horizontal;

    SetBrushImageSize(TimeScale, 30.0f);
}

void SAnimationTimelineWidget::InvalidateWidget(TSharedPtr<SWidget> Widget)
{
    if (Widget.IsValid())
    {
        UE_LOG(LogTemp, Log, TEXT("TIMELINE PLUGIN : Plugin Invalidate!"));
        Widget->Invalidate(EInvalidateWidgetReason::Paint);
    }
}

FOptionalSize SAnimationTimelineWidget::GetTrackFieldWidth() const
{
    return FOptionalSize(TrackFieldWidth);
}

FVector2D SAnimationTimelineWidget::GetBrushImageSize() const
{
    return FVector2D(TimeScale, 30.0f);
}

void SAnimationTimelineWidget::SetBrushImageSize(float WidthValue, float HeightValue)
{
    TimelineUnitTopBrush.ImageSize = FVector2D(WidthValue, HeightValue);
    TimelineUnitBottomBrush.ImageSize = FVector2D(WidthValue, HeightValue);
    TimelineTrackTopBrush.ImageSize = FVector2D(WidthValue, HeightValue);
    TimelineTrackBrush.ImageSize = FVector2D(WidthValue, HeightValue);
    TimelineTrackBottomBrush.ImageSize = FVector2D(WidthValue, HeightValue);

    TimelineUnitTopMaterialDynamic->SetScalarParameterValue("HorizontalTiling", TimeScale / 200.0f);
    TimelineUnitBottomMaterialDynamic->SetScalarParameterValue("HorizontalTiling", TimeScale / 200.0f);
    TimelineTrackTopMaterialDynamic->SetScalarParameterValue("HorizontalTiling", TimeScale / 200.0f);
    TimelineTrackMaterialDynamic->SetScalarParameterValue("HorizontalTiling", TimeScale / 200.0f);
    TimelineTrackBottomMaterialDynamic->SetScalarParameterValue("HorizontalTiling", TimeScale / 200.0f);
}

const FSlateBrush* SAnimationTimelineWidget::GetTimelineUnitTopBrush() const
{
    return &TimelineUnitTopBrush;
}

const FSlateBrush* SAnimationTimelineWidget::GetTimelineUnitBottomBrush() const
{
    return &TimelineUnitBottomBrush;
}

const FSlateBrush* SAnimationTimelineWidget::GetTimelineTrackTopBrush() const
{
    return &TimelineTrackTopBrush;
}

const FSlateBrush* SAnimationTimelineWidget::GetTimelineTrackBrush() const
{
    return &TimelineTrackBrush;
}

const FSlateBrush* SAnimationTimelineWidget::GetTimelineTrackBottomBrush() const
{
    return &TimelineTrackBottomBrush;
}

void SAnimationTimelineWidget::AddTimeline()
{
    CurrentTimeTextBox = SNew(SEditableTextBox)
        .Text(this, &SAnimationTimelineWidget::GetCurrentTimeText)
        .Font(SmallFont)
        .OnTextCommitted(this, &SAnimationTimelineWidget::OnCurrentTimeCommitted)
        .SelectAllTextWhenFocused(true);
    //.MinDesiredWidth(100.0f);

    TimeScaleTextBox = SNew(SEditableTextBox)
        .Text(this, &SAnimationTimelineWidget::GetTimeScaleText)
        .Font(SmallFont)
        .OnTextCommitted(this, &SAnimationTimelineWidget::OnTimeScaleCommitted)
        .SelectAllTextWhenFocused(true);
    //.MinDesiredWidth(100.0f);

    ChildSlot
        [
            SNew(SVerticalBox)
                // Slot pour TimeScaleTextBox et TimeScaleSlider
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SHorizontalBox)
                        // Slot pour TimeScaleTextBox
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SNew(SBox)
                                .WidthOverride(50.0f) // Largeur fixe pour le texte
                                .HeightOverride(30.0f) // Hauteur fixe pour le texte
                                [
                                    TimeScaleTextBox.ToSharedRef()
                                ]
                        ]

                        // Slot pour TimeScaleSlider 
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SNew(SBox)
                                .MinDesiredWidth(200.0f) // Largeur fixe pour le texte
                                .HeightOverride(30.0f) // Hauteur fixe pour le texte
                                [
                                    SAssignNew(TimeScaleSlider, SSlider)
                                        .Value(this, &SAnimationTimelineWidget::GetTimeScaleSliderValue)
                                        .OnValueChanged(this, &SAnimationTimelineWidget::OnTimeScaleSliderValueChanged)
                                        .SliderBarColor(FLinearColor::White)
                                        .Style(&FCoreStyle::Get().GetWidgetStyle<FSliderStyle>("Slider"))
                                ]
                        ]
                ]

                // Slot pour la frise totale
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SHorizontalBox)
                        // Slot pour CurrentTimeTextBox encapsul� dans une SVerticalBox
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SAssignNew(VariablesField, SVerticalBox) // Assigne la SVerticalBox � VariablesField
                                + SVerticalBox::Slot()
                                .AutoHeight()
                                [
                                    SNew(SBox)
                                        .WidthOverride(50.0f) // Largeur fixe pour le texte
                                        .HeightOverride(30.0f) // Hauteur fixe pour le texte
                                        [
                                            CurrentTimeTextBox.ToSharedRef()
                                        ]
                                ]
                        ]

                        // Slot pour la frise scrollable encapsul� dans une SVerticalBox
                        + SHorizontalBox::Slot()
                        .FillWidth(1.0f)
                        [
                            SAssignNew(TracksScrollBox, SScrollBox)
                                .Orientation(Orient_Horizontal) // Scroll horizontal uniquement
                                + SScrollBox::Slot()
                                [
                                    SAssignNew(TracksField, SVerticalBox) // Assigne la SVerticalBox � KeyFramesField
                                        + SVerticalBox::Slot()
                                        .AutoHeight()
                                        [
                                            SNew(SBox)
                                                .MinDesiredWidth(500.0f) // Largeur minimale de la frise
                                                .WidthOverride(TrackFieldWidthAttribute)
                                                .HeightOverride(30.0f)   // Hauteur fixe de la frise
                                                [
                                                    SAssignNew(TimelineUnitTopImage, SImage)
                                                        .Image(TimelineUnitTopBrushAttribute)
                                                        .OnMouseButtonDown(this, &SAnimationTimelineWidget::OnTimelineClicked)
                                                ]
                                        ]
                                        + SVerticalBox::Slot()
                                        .AutoHeight()
                                        [
                                            SNew(SBox)
                                                .MinDesiredWidth(500.0f) // Largeur minimale de la frise
                                                .WidthOverride(TrackFieldWidthAttribute)
                                                .HeightOverride(30.0f)   // Hauteur fixe de la frise
                                                [
                                                    SAssignNew(TimelineUnitBottomImage, SImage)
                                                        .Image(TimelineUnitBottomBrushAttribute)
                                                        .OnMouseButtonDown(this, &SAnimationTimelineWidget::OnTimelineClicked)
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
        TrackFieldWidth = (TimelineDuration * 100.0f) * (100.0f / TimeScale);
    }
    else
    {
        TrackFieldWidth = (5 * 100.0f) * (100.0f / TimeScale);
    }

    InvalidateWidget(TracksField);
}

void SAnimationTimelineWidget::SetCurrentTime(float Value)
{
    CurrentTime = Value;
}

void SAnimationTimelineWidget::SetTimeScale(float Value)
{
    TimeScale = Value;
    TimeScaleTextBox->SetText(GetTimeScaleText());

    SetBrushImageSize(TimeScale, 30.0f);

    UE_LOG(LogTemp, Log, TEXT("TIMELINE PLUGIN : TimeScale set!"));
    SetTimelineDuration();
}

FText SAnimationTimelineWidget::GetCurrentTimeText() const
{
    return FText::AsNumber(CurrentTime);
}

void SAnimationTimelineWidget::OnCurrentTimeCommitted(const FText& NewText, ETextCommit::Type CommitType)
{
    if (CommitType == ETextCommit::OnEnter || CommitType == ETextCommit::OnUserMovedFocus)
    {
        FString TypingFieldContent = NewText.ToString();

        const std::string& str = TCHAR_TO_UTF8(*TypingFieldContent);
        std::istringstream iss(str);
        float value;
        iss >> value;

        if (!iss.fail() && iss.eof())
        {
            if (value >= 0)
            {
                SetCurrentTime(value);
            }
            else
            {
                UE_LOG(LogTemp, Log, TEXT("TIMELINE PLUGIN : CurrentTime must be a positive number!"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("TIMELINE PLUGIN : CurrentTime not set as valid Float!"));
        }
    }
}

FText SAnimationTimelineWidget::GetTimeScaleText() const
{
    return FText::AsNumber(TimeScale);
}

void SAnimationTimelineWidget::OnTimeScaleCommitted(const FText& NewText, ETextCommit::Type CommitType)
{
    if (CommitType == ETextCommit::OnEnter || CommitType == ETextCommit::OnUserMovedFocus)
    {
        FString TypingFieldContent = NewText.ToString();

        const std::string& str = TCHAR_TO_UTF8(*TypingFieldContent);
        std::istringstream iss(str);
        float value;
        iss >> value;

        if (!iss.fail() && iss.eof())
        {
            if (value >= 10.0f && value <= 200.0f)
            {
                SetTimeScale(value);
                InvalidateWidget(TimeScaleSlider);
            }
            else
            {
                UE_LOG(LogTemp, Log, TEXT("TIMELINE PLUGIN : TimeScale must be a number between 10 and 200!"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("TIMELINE PLUGIN : TimeScale not set as valid Float!"));
        }

        TimeScaleTextBox->SetText(GetTimeScaleText());
    }
}

float SAnimationTimelineWidget::GetTimeScaleSliderValue() const
{
    return (TimeScale - 10.0f) / 190.0f;
}

void SAnimationTimelineWidget::OnTimeScaleSliderValueChanged(float NewValue)
{
    SetTimeScale(FMath::Lerp(10.0f, 200.0f, NewValue));
}

FReply SAnimationTimelineWidget::OnTimelineClicked(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    FVector2D LocalClickPosition = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
    CurrentTime = GetTimeFromClick(LocalClickPosition);
    return FReply::Handled();
}

float SAnimationTimelineWidget::GetTimeFromClick(const FVector2D& ClickPosition) const
{
    float ClickX = ClickPosition.X;
    return FMath::Clamp((ClickX / TrackFieldWidth) * TimelineDuration, 0.0f, TimelineDuration);
}

/*FText SAnimationTimelineWidget::GetCurrentTimeText() const
{
    return FText::Format(NSLOCTEXT("AnimationTimelineWidget", "CurrentTime", "Current Time: {0} s"), FText::AsNumber(CurrentTime));
}*/

FText SAnimationTimelineWidget::GetDurationText() const
{
    return FText::Format(NSLOCTEXT("AnimationTimelineWidget", "Duration", "Total Duration: {0} s"), FText::AsNumber(TimelineDuration));
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