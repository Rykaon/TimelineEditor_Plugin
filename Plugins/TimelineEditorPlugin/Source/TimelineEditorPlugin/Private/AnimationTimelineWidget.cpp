#include "AnimationTimelineWidget.h"
#include "Slate.h"
#include "TimelinePluginWidget.h"
#include "PaperSprite.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/Texture.h"
#include "Widgets/Layout/SConstraintCanvas.h"
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
    TrackFieldHeightAttribute = TAttribute<FOptionalSize>::Create(TAttribute<FOptionalSize>::FGetter::CreateSP(this, &SAnimationTimelineWidget::GetTracksFieldHeight));

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
    TimelineCursorBrushAttribute = TAttribute<const FSlateBrush*>::Create(TAttribute<const FSlateBrush*>::FGetter::CreateSP(this, &SAnimationTimelineWidget::GetTimelineCursorBrush));

    SmallFont = FCoreStyle::GetDefaultFontStyle("Regular", 8);

    TimelineUnitTopBrush.SetResourceObject(TimelineUnitTopMaterialDynamic);
    TimelineUnitTopBrush.Tiling = ESlateBrushTileType::Horizontal;

    TimelineUnitBottomBrush.SetResourceObject(TimelineUnitBottomMaterialDynamic);
    TimelineUnitBottomBrush.Tiling = ESlateBrushTileType::Horizontal;

    TimelineTrackTopBrush.SetResourceObject(TimelineTrackTopMaterialDynamic);
    TimelineTrackTopBrush.Tiling = ESlateBrushTileType::Horizontal;

    TimelineTrackBrush.SetResourceObject(TimelineTrackMaterialDynamic);
    TimelineTrackBrush.Tiling = ESlateBrushTileType::Horizontal;

    TimelineTrackBottomBrush.SetResourceObject(TimelineTrackBottomMaterialDynamic);
    TimelineTrackBottomBrush.Tiling = ESlateBrushTileType::Horizontal;

    TimelineCursorBrush.SetResourceObject(LoadObject<UPaperSprite>(nullptr, TEXT("/TimelineEditorPlugin/Textures/SPR_TimelineCursor")));


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

    TimelineUnitTopMaterialDynamic->SetScalarParameterValue("HorizontalTiling", TimeScale / 100.0f);
    TimelineUnitBottomMaterialDynamic->SetScalarParameterValue("HorizontalTiling", TimeScale / 100.0f);
    TimelineTrackTopMaterialDynamic->SetScalarParameterValue("HorizontalTiling", TimeScale / 100.0f);
    TimelineTrackMaterialDynamic->SetScalarParameterValue("HorizontalTiling", TimeScale / 100.0f);
    TimelineTrackBottomMaterialDynamic->SetScalarParameterValue("HorizontalTiling", TimeScale / 100.0f);
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

const FSlateBrush* SAnimationTimelineWidget::GetTimelineCursorBrush() const
{
    return &TimelineCursorBrush;
}

FOptionalSize SAnimationTimelineWidget::GetTracksFieldHeight() const
{
    if (TracksField.IsValid())
    {
        return FOptionalSize(TracksField->GetDesiredSize().Y);
    }
    return FOptionalSize(0.0f); // Par défaut, retourne 0 si non valide
}

void SAnimationTimelineWidget::AddTimeline()
{
    CurrentTimeTextBox = SNew(SEditableTextBox)
        .Text(this, &SAnimationTimelineWidget::GetCurrentTimeText)
        .Font(SmallFont)
        .OnTextCommitted(this, &SAnimationTimelineWidget::OnCurrentTimeCommitted)
        .SelectAllTextWhenFocused(true);

    TimeScaleTextBox = SNew(SEditableTextBox)
        .Text(this, &SAnimationTimelineWidget::GetTimeScaleText)
        .Font(SmallFont)
        .OnTextCommitted(this, &SAnimationTimelineWidget::OnTimeScaleCommitted)
        .SelectAllTextWhenFocused(true);

    ChildSlot
    [
        SNew(SVerticalBox)

        // Contrôle de TimeScale
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SBox)
                .WidthOverride(50.0f)
                .HeightOverride(30.0f)
                [
                    TimeScaleTextBox.ToSharedRef()
                ]
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SBox)
                .MinDesiredWidth(200.0f)
                .HeightOverride(30.0f)
                [
                    SAssignNew(TimeScaleSlider, SSlider)
                    .Value(this, &SAnimationTimelineWidget::GetTimeScaleSliderValue)
                    .OnValueChanged(this, &SAnimationTimelineWidget::OnTimeScaleSliderValueChanged)
                    .SliderBarColor(FLinearColor::White)
                    .Style(&FCoreStyle::Get().GetWidgetStyle<FSliderStyle>("Slider"))
                ]
            ]
        ]

        // Frise et curseur
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            // Variables & Tracks Field
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                // Variables Field
                SAssignNew(VariablesField, SVerticalBox)
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    // Current Time
                    SNew(SBox)
                    .WidthOverride(50.0f)
                    .HeightOverride(30.0f)
                    [
                        CurrentTimeTextBox.ToSharedRef()
                    ]
                ]
            ]

            + SHorizontalBox::Slot()
            .FillWidth(1.0f)
            [
                // TracksField
                SAssignNew(TracksScrollBox, SScrollBox)
                .Orientation(Orient_Horizontal)
                + SScrollBox::Slot()
                [
                    SNew(SOverlay)

                    // TracksField (les frises)
                    + SOverlay::Slot()
                    [
                        SAssignNew(TracksField, SVerticalBox)
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        [
                            SNew(SBox)
                            .MinDesiredWidth(500.0f)
                            .WidthOverride(TrackFieldWidthAttribute)
                            .HeightOverride(30.0f)
                            [
                                SNew(SImage)
                                .Image(TimelineUnitTopBrushAttribute)
                                .OnMouseButtonDown(this, &SAnimationTimelineWidget::OnTimelineClicked)
                            ]
                        ]
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        [
                            SNew(SBox)
                            .MinDesiredWidth(500.0f)
                            .WidthOverride(TrackFieldWidthAttribute)
                            .HeightOverride(30.0f)
                            [
                                SNew(SImage)
                                .Image(TimelineUnitBottomBrushAttribute)
                                .OnMouseButtonDown(this, &SAnimationTimelineWidget::OnTimelineClicked)
                            ]
                        ]
                    ]

                    + SOverlay::Slot()
                    .HAlign(HAlign_Left)
                    .VAlign(VAlign_Top)
                    [
                        SNew(SBox)
                        .WidthOverride(TrackFieldWidthAttribute) // Largeur de l'espace de clipping
                        .HeightOverride(TrackFieldHeightAttribute) // Hauteur de l'espace de clipping
                        .Clipping(EWidgetClipping::ClipToBounds) // Clipper le contenu excédentaire
                        [
                            // Curseur
                            SNew(SConstraintCanvas)
                            + SConstraintCanvas::Slot()
                            .AutoSize(true) // Le curseur garde sa taille d'origine
                            .Anchors(FAnchors(0.0f, 0.0f)) // Ancrage en haut à gauche
                            .Offset(FMargin(0.0f, 0.0f, 15.0f, 2000.0f)) // Taille fixe (15x2000)
                            .Alignment(FVector2D(0.5f, 0.0f)) // Alignement en haut
                            [
                                SNew(SBox)
                                .WidthOverride(15.0f)
                                .HeightOverride(2000.0f)
                                .RenderTransform(this, &SAnimationTimelineWidget::GetCursorTransform)
                                .RenderTransformPivot(FVector2D(0.0f, 0.5f)) // Point de pivot
                                [
                                    SNew(SImage)
                                    .Image(TimelineCursorBrushAttribute)
                                    .Visibility(EVisibility::HitTestInvisible)
                                ]
                            ]
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
    float Time = FMath::Clamp((ClickX / TrackFieldWidth) * TimelineDuration, 0.0f, TimelineDuration);
    return FMath::RoundToFloat(Time * 100.0f) / 100.0f;
}

TOptional<FSlateRenderTransform> SAnimationTimelineWidget::GetCursorTransform() const
{
    if (TimelineDuration <= 0.0f || TrackFieldWidth <= 0.0f)
    {
        return TOptional<FSlateRenderTransform>(); // Aucun transform s'il n'y a pas de données valides
    }

    // Calcul de la position du curseur
    float CursorPositionX = (CurrentTime / TimelineDuration) * TrackFieldWidth;

    // Retourner la transformation
    return FSlateRenderTransform(FVector2D(CursorPositionX, 0.0f));
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