#include "AnimationTimelineWidget.h"
#include "Slate.h"
#include "TimelinePluginWidget.h"
#include "AnimationKeyFrame.h"
#include "PaperSprite.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/Texture.h"
#include "Widgets/Layout/SConstraintCanvas.h"
#include <optional>
#include <string>
#include <sstream>
#include <iostream>
#include <cmath>

void SAnimationTimelineWidget::Construct(const FArguments& InArgs)
{
    AnimationTimeline = InArgs._AnimationTimeline;

    if (!AnimationTimeline)
    {
        UE_LOG(LogTemp, Error, TEXT("AnimationTimeline is null!"));
        return;
    }

    CurrentTime = 0.0f;
    TimeScale = 100.0f;
    TrackFieldWidth = (AnimationTimeline->Duration * 100.0f) * (100.0f / TimeScale);

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
    RemoveTrackBrush.SetResourceObject(LoadObject<UPaperSprite>(nullptr, TEXT("/TimelineEditorPlugin/Textures/SPR_RemoveTrack")));
    AddKeyFrameBrush.SetResourceObject(LoadObject<UPaperSprite>(nullptr, TEXT("/TimelineEditorPlugin/Textures/SPR_AddKeyFrame")));
    KeyFrameDefaultBrush.SetResourceObject(LoadObject<UPaperSprite>(nullptr, TEXT("/TimelineEditorPlugin/Textures/SPR_KeyFrameDefault")));
    KeyFrameSelectedBrush.SetResourceObject(LoadObject<UPaperSprite>(nullptr, TEXT("/TimelineEditorPlugin/Textures/SPR_KeyFrameSelected")));

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

void SAnimationTimelineWidget::InitializeTimelineTracks()
{
    for (int32 i = 0; i < AnimationTimeline->Tracks.Num(); ++i)
    {
        AddTrack(AnimationTimeline->Tracks[i]);

        for (int32 j = 0; j < AnimationTimeline->Tracks[i].KeyFrames.Num(); ++j)
        {
            AddKeyFrameSlot(AnimationTimeline->Tracks[i].KeyFrames[j].KeyFrameID);
        }
    }
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

    KeyFramesCanvas = SNew(SConstraintCanvas)
    .Clipping(EWidgetClipping::ClipToBounds);

    ChildSlot
    [
        // Contrôle de TimeScale
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SBox)
                .WidthOverride(160.0f)
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
                    .WidthOverride(160.0f)
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

                    + SOverlay::Slot()
                    [
                        KeyFramesCanvas.ToSharedRef()
                    ]
                ]
            ]
        ]
    ];
}

void SAnimationTimelineWidget::AddTrack(FAnimationTrack& Track)
{
    TArray<TSharedPtr<SEditableTextBox>> ValueTextBoxes;

    TSharedPtr<SEditableTextBox> ValueTextBox_X = SNew(SEditableTextBox)
    .Text(GetTrackDefaultValueAsText(Track.TrackID, 0))
    .Font(SmallFont)
    .SelectAllTextWhenFocused(true)
    .OnTextCommitted_Lambda([this, TrackID = Track.TrackID](const FText& NewText, ETextCommit::Type CommitType)
    {
        OnTextBoxCommittedForTrack(NewText, CommitType, TrackID, 0);
    });
    ValueTextBoxes.Add(ValueTextBox_X);

    float TextBoxWidthValue = 105.0f;

    if (Track.VariableType == "Vector" || Track.VariableType == "Rotator")
    {
        TSharedPtr<SEditableTextBox> ValueTextBox_Y = SNew(SEditableTextBox)
        .Text(GetTrackDefaultValueAsText(Track.TrackID, 1))
        .Font(SmallFont)
        .SelectAllTextWhenFocused(true)
        .OnTextCommitted_Lambda([this, TrackID = Track.TrackID](const FText& NewText, ETextCommit::Type CommitType)
        {
            OnTextBoxCommittedForTrack(NewText, CommitType, TrackID, 1);
        });
        ValueTextBoxes.Add(ValueTextBox_Y);

        TSharedPtr<SEditableTextBox> ValueTextBox_Z = SNew(SEditableTextBox)
        .Text(GetTrackDefaultValueAsText(Track.TrackID, 2))
        .Font(SmallFont)
        .SelectAllTextWhenFocused(true)
        .OnTextCommitted_Lambda([this, TrackID = Track.TrackID](const FText& NewText, ETextCommit::Type CommitType)
        {
            OnTextBoxCommittedForTrack(NewText, CommitType, TrackID, 2);
        });
        ValueTextBoxes.Add(ValueTextBox_Z);

        TextBoxWidthValue = 35.0f;
    }

    TSharedRef<SHorizontalBox> HorizontalBox = SNew(SHorizontalBox);

    // Ajout dans les TMaps
    TracksMap.Add(Track.TrackID, &Track);
    ValuesTextBoxMap.Add(Track.TrackID, ValueTextBoxes);
    VariableFieldMap.Add(Track.TrackID, HorizontalBox);

    // Ajouter le nom de la variable
    HorizontalBox->AddSlot()
    .AutoWidth()
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SBox)
            .WidthOverride(14.0f)
            .HeightOverride(14.0f)
            [
                SNew(SButton)
                .Content()
                [
                    SNew(SImage)
                    .Image(&RemoveTrackBrush)
                ]
                .OnClicked_Lambda([this, TrackID = Track.TrackID]() -> FReply
                {
                    RemoveTrack(TrackID);
                    return FReply::Handled();
                })
            ]
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SSpacer)
            .Size(FVector2D(14.0f, 2.0f))
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SBox)
            .WidthOverride(14.0f)
            .HeightOverride(14.0f)
            [
                SNew(SButton)
                .Content()
                [
                    SNew(SImage)
                    .Image(&AddKeyFrameBrush)
                ]
                .OnClicked_Lambda([this, TrackID = Track.TrackID]() -> FReply
                {
                    AddKeyFrameToTrack(TrackID, CurrentTime, false, FText::FromString("None"), -1);
                    return FReply::Handled();
                })
            ]
        ]
    ];
    
    HorizontalBox->AddSlot()
    .AutoWidth()
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .VAlign(VAlign_Center)
        [
            SNew(SBox)
            .WidthOverride(50.0f)
            .HeightOverride(30.0f)
            [
                SNew(STextBlock)
                .Text(FText::FromName(Track.VariableName))
                .Font(SmallFont)
            ]
        ]
    ];

    // Ajouter les TextBoxes pour X, Y, et Z
    for (TSharedPtr<SEditableTextBox>& TextBox : ValueTextBoxes)
    {
        HorizontalBox->AddSlot()
        .AutoWidth()
        [
            SNew(SBox)
            .WidthOverride(TextBoxWidthValue)
            .HeightOverride(30.0f)
            [
                TextBox.ToSharedRef()
            ]
        ];
    }

    // Add Variable Slot and reconstruct VariableField
    int32 VariableFieldSlotCount = VariablesField->NumSlots();
    InsertVariableSlotAt(VariableFieldSlotCount - 1, HorizontalBox);

    // Add Track Slot and reconstruct TracksField
    int32 TracksFieldSlotCount = TracksField->NumSlots();
    TSharedRef<SBox> TrackImage = SNew(SBox)
    .MinDesiredWidth(500.0f)
    .WidthOverride(TrackFieldWidthAttribute)
    .HeightOverride(30.0f)
    [
        SNew(SImage)
        .Image(TimelineUnitBottomBrushAttribute)
        .OnMouseButtonDown(this, &SAnimationTimelineWidget::OnTimelineClicked)
    ];

    InsertTrackSlotAt(TracksFieldSlotCount, TrackImage);
}

void SAnimationTimelineWidget::InsertVariableSlotAt(int32 Index, TSharedRef<SWidget> NewWidget)
{
    if (VariablesField.IsValid())
    {
        TArray<TSharedRef<SWidget>> Widgets;

        for (int32 i = 0; i < VariablesField->NumSlots(); ++i)
        {
            Widgets.Add(VariablesField->GetChildren()->GetChildAt(i));
        }

        // Vérifie que l'index est valide
        if (Index < 0 || Index > Widgets.Num())
        {
            UE_LOG(LogTemp, Warning, TEXT("Invalid index for VariableSlot insertion: %d"), Index);
            return;
        }

        if (Index == Widgets.Num() - 1)
        {
            
            Widgets.Add(NewWidget);
        }
        else
        {
            Widgets.Insert(NewWidget, Index);
        }
        
        VariablesField->ClearChildren();
        ConstructVariableField(Widgets);
    }
}

void SAnimationTimelineWidget::ConstructVariableField(TArray<TSharedRef<SWidget>> Widgets)
{
    for (int32 i = 0; i < Widgets.Num(); ++i)
    {
        VariablesField->AddSlot()
        .AutoHeight()
        [
            Widgets[i]
        ];
    }
}

void SAnimationTimelineWidget::InsertTrackSlotAt(int32 Index, TSharedRef<SWidget> NewWidget)
{
    if (TracksField.IsValid())
    {
        // Crée une liste temporaire pour stocker les widgets
        TArray<TSharedRef<SWidget>> Widgets;

        // Ajoute tous les widgets actuels à la liste
        for (int32 i = 0; i < TracksField->NumSlots(); ++i)
        {
            Widgets.Add(TracksField->GetChildren()->GetChildAt(i));
        }

        // Vérifie que l'index est valide
        if (Index < 0 || Index > Widgets.Num())
        {
            UE_LOG(LogTemp, Warning, TEXT("Invalid index for TrackSlot insertion: %d"), Index);
            return;
        }

        if (Index == Widgets.Num() - 1)
        {
            Widgets.Add(NewWidget);
        }
        else
        {
            Widgets.Insert(NewWidget, Index);
        }

        TracksField->ClearChildren();
        ConstructTrackField(Widgets);
    }
}

void SAnimationTimelineWidget::ConstructTrackField(TArray<TSharedRef<SWidget>> Widgets)
{
    for (int32 i = 0; i < Widgets.Num(); ++i)
    {
        //UE_LOG(LogTemp, Warning, TEXT("Value of i: %d, Value of Widgets.Num(): %d"), i, Widgets.Num());
        if (i == 0)
        {
            //UE_LOG(LogTemp, Warning, TEXT("TRACK TOP"));
            if (Widgets[i]->GetChildren()->Num() > 0)
            {
                TSharedPtr<SWidget> ChildWidget = Widgets[i]->GetChildren()->GetChildAt(0);
                TSharedPtr<SImage> ImageWidget = StaticCastSharedPtr<SImage>(ChildWidget);
                if (ImageWidget.IsValid())
                {
                    ImageWidget->SetImage(TimelineUnitTopBrushAttribute.Get());
                }
            }
        }
        else if (i == 1 && Widgets.Num() > 2)
        {
            //UE_LOG(LogTemp, Warning, TEXT("TRACK TOP"));
            if (Widgets[i]->GetChildren()->Num() > 0)
            {
                TSharedPtr<SWidget> ChildWidget = Widgets[i]->GetChildren()->GetChildAt(0);
                TSharedPtr<SImage> ImageWidget = StaticCastSharedPtr<SImage>(ChildWidget);
                if (ImageWidget.IsValid())
                {
                    ImageWidget->SetImage(TimelineTrackTopBrushAttribute.Get());
                }
            }
        }
        else if (i >= 2 && i <= Widgets.Num() - 2 && Widgets.Num() >= 5)
        {
            //UE_LOG(LogTemp, Warning, TEXT("TRACK"));
            if (Widgets[i]->GetChildren()->Num() > 0)
            {
                TSharedPtr<SWidget> ChildWidget = Widgets[i]->GetChildren()->GetChildAt(0);
                TSharedPtr<SImage> ImageWidget = StaticCastSharedPtr<SImage>(ChildWidget);
                if (ImageWidget.IsValid())
                {
                    ImageWidget->SetImage(TimelineTrackBrushAttribute.Get());
                }
            }
        }
        else if (i == Widgets.Num() - 2)
        {
            //UE_LOG(LogTemp, Warning, TEXT("TRACK BOTTOM"));
            if (Widgets[i]->GetChildren()->Num() > 0)
            {
                TSharedPtr<SWidget> ChildWidget = Widgets[i]->GetChildren()->GetChildAt(0);
                TSharedPtr<SImage> ImageWidget = StaticCastSharedPtr<SImage>(ChildWidget);
                if (ImageWidget.IsValid())
                {
                    ImageWidget->SetImage(TimelineTrackBottomBrushAttribute.Get());
                }
            }
        }
        else if (i == Widgets.Num() - 1)
        {
            //UE_LOG(LogTemp, Warning, TEXT("TRACK UNIT BOTTOM"));
            if (Widgets[i]->GetChildren()->Num() > 0)
            {
                TSharedPtr<SWidget> ChildWidget = Widgets[i]->GetChildren()->GetChildAt(0);
                TSharedPtr<SImage> ImageWidget = StaticCastSharedPtr<SImage>(ChildWidget);
                if (ImageWidget.IsValid())
                {
                    ImageWidget->SetImage(TimelineUnitBottomBrushAttribute.Get());
                }
            }
        }

        TracksField->AddSlot()
        .AutoHeight()
        [
            Widgets[i]
        ];

        int32 j = i - 1;

        if (j >= 0 && j < AnimationTimeline->Tracks.Num())
        {

            int32 ID = AnimationTimeline->Tracks[j].TrackID;
            TSharedRef<SBox> Widget = StaticCastSharedRef<SBox>(Widgets[i]);

            if (TracksFieldMap.Contains(ID))
            {
                TracksFieldMap[ID] = Widget;
            }
            else 
            {
                TracksFieldMap.Add(ID, Widget);
            }
            
        }
    }
}

void SAnimationTimelineWidget::AddKeyFrameSlot(int32 KeyFrameID)
{
    TSharedRef<SBox> KeyFrameWidget = SNew(SBox)
    .WidthOverride(10.0f)
    .HeightOverride(10.0f)
    .RenderTransform_Lambda([this, KeyFrameID]() -> FSlateRenderTransform
    {
        if (!KeyFramesMap.Contains(KeyFrameID))
        {
            return FSlateRenderTransform();
        }

        TOptional<FSlateRenderTransform> RenderTransform = GetKeyFrameTransform(KeyFrameID);
        
        if (RenderTransform.IsSet())
        {
            return RenderTransform.GetValue();
        }
        else
        {
            return FSlateRenderTransform();
        }
    })
    [
        SNew(SButton)
        .Content()
        [
            SNew(SImage)
            .Image(&KeyFrameDefaultBrush)
        ]
        .OnClicked_Lambda([this, KeyFrameID]() -> FReply
        {
            OpenKeyFrame(KeyFrameID);
            return FReply::Handled();
        })
    ];

    if (!KeyFramesMap.Contains(KeyFrameID))
    {
        KeyFramesMap.Add(KeyFrameID, KeyFrameWidget);
    }

    KeyFramesCanvas->AddSlot()
    .Anchors(FAnchors(0.0f, 0.0f))
    .Offset(FMargin(0.00001f, 0.00001f, 10.0f, 10.0f))
    .Alignment(FVector2D(0.5f, 0.5f))
    [
        KeyFrameWidget
    ];
}

void SAnimationTimelineWidget::ConstrucKeyFrameField(TArray<TSharedRef<SWidget>> Widgets)
{
    for (int32 i = 0; i < Widgets.Num(); ++i)
    {
        KeyFramesCanvas->AddSlot()
        .Anchors(FAnchors(0.0f, 0.0f))
        .AutoSize(true)
        .Alignment(FVector2D(0.5f, 0.5f))
        [
            Widgets[i]
        ];
    }
}

void SAnimationTimelineWidget::ResetTracks()
{
    for (int32 i = 0; i < AnimationTimeline->Tracks.Num(); ++i)
    {
        RemoveTrack(AnimationTimeline->Tracks[i].TrackID);
    }
}

void SAnimationTimelineWidget::RemoveTrack(int32 TrackID)
{
    FAnimationTrack* Track = GetTrackOfID(TrackID);

    if (Track == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("No Track of Track ID %d found in AnimationTimeline"), TrackID);
        return;
    }

    if (TracksMap.Contains(Track->TrackID))
    {
        TracksMap.Remove(Track->TrackID);
        UE_LOG(LogTemp, Warning, TEXT("Remove from TracksMap"));
    }

    if (ValuesTextBoxMap.Contains(Track->TrackID))
    {
        ValuesTextBoxMap.Remove(Track->TrackID);
        UE_LOG(LogTemp, Warning, TEXT("Remove from TextBoxMap"));
    }

    if (TracksField.IsValid())
    {
        TArray<TSharedRef<SWidget>> TracksWidgets;
        int32 IndexToRemove = -1;

        for (int32 i = 0; i < TracksField->NumSlots(); ++i)
        {
            TracksWidgets.Add(TracksField->GetChildren()->GetChildAt(i));
        }

        for (int32 i = 0; i < TracksWidgets.Num(); ++i)
        {
            TSharedRef<SBox> Box = StaticCastSharedRef<SBox>(TracksWidgets[i]);
            
            if (TracksFieldMap.Contains(Track->TrackID))
            {
                if (TracksFieldMap[Track->TrackID] == Box)
                {
                    TracksFieldMap.Remove(Track->TrackID);
                    IndexToRemove = i;
                    break;
                }
            }
        }

        if (IndexToRemove >= 0)
        {
            TArray<TSharedRef<SWidget>> NewTracksWidgets;
            for (int32 i = 0; i < TracksWidgets.Num(); ++i)
            {
                if (i != IndexToRemove)
                {
                    NewTracksWidgets.Add(TracksWidgets[i]);
                }
            }

            TracksField->ClearChildren();
            ConstructTrackField(NewTracksWidgets);
        }
    }

    if (VariablesField.IsValid())
    {
        TArray<TSharedRef<SWidget>> VariableWidgets;
        int32 IndexToRemove = -1;

        for (int32 i = 0; i < VariablesField->NumSlots(); ++i)
        {
            VariableWidgets.Add(VariablesField->GetChildren()->GetChildAt(i));
        }

        for (int32 i = 0; i < VariableWidgets.Num(); ++i)
        {
            TSharedRef<SHorizontalBox> HorizontalBox = StaticCastSharedRef<SHorizontalBox>(VariableWidgets[i]);
            
            if (VariableFieldMap.Contains(Track->TrackID))
            {
                if (VariableFieldMap[Track->TrackID] == HorizontalBox)
                {
                    VariableFieldMap.Remove(Track->TrackID);
                    IndexToRemove = i;
                    break;
                }
            }
        }

        if (IndexToRemove >= 0)
        {
            TArray<TSharedRef<SWidget>> NewVariableWidgets;
            for (int32 i = 0; i < VariableWidgets.Num(); ++i)
            {
                if (i != IndexToRemove)
                {
                    NewVariableWidgets.Add(VariableWidgets[i]);
                }
            }

            VariablesField->ClearChildren();
            ConstructVariableField(NewVariableWidgets);
        }
    }

    for (int32 i = 0; i < Track->KeyFrames.Num(); ++i)
    {        
        if (KeyFramesMap.Contains(Track->KeyFrames[i].KeyFrameID))
        {
            RemoveKeyFrame(Track->KeyFrames[i].KeyFrameID);
        }
    }
    
    TimelinePluginWidget->TimelineComponent->RemoveTrack(TrackID);
}

void SAnimationTimelineWidget::RemoveKeyFrame(int32 KeyFrameID)
{
    FAnimationKeyFrame* KeyFrame = GetKeyFrameOfID(KeyFrameID);

    if (KeyFrame == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("No KeyFrame of KeyFrameID %d found in AnimationTimeline"), KeyFrameID);
        return;
    }

    FAnimationTrack* Track = GetTrackOfID(KeyFrame->ParentTrackID);

    if (Track == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("No Track of Track ID %d found in AnimationTimeline"), KeyFrame->ParentTrackID);
        return;
    }

    if (!KeyFramesMap.Contains(KeyFrame->KeyFrameID))
    {
        UE_LOG(LogTemp, Warning, TEXT("KeyFramesMap doesn't contains KeyFrame of KeyFrameID %d"), KeyFrameID);
        return;
    }

    if (KeyFramesCanvas.IsValid())
    {
        TSharedPtr<SBox> WidgetToRemove = KeyFramesMap[KeyFrame->KeyFrameID];

        if (WidgetToRemove.IsValid())
        {
            // Supprimer le widget de KeyFramesCanvas
            KeyFramesCanvas->RemoveSlot(WidgetToRemove.ToSharedRef());
            KeyFramesMap.Remove(KeyFrame->KeyFrameID);
            WidgetToRemove.Reset();
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("KeyFrame Widget for ID %d is invalid"), KeyFrameID);
        }
    }

    int32 KeyFrameIndex = -1;

    for (int32 i = 0; i < Track->KeyFrames.Num(); ++i)
    {
        if (Track->KeyFrames[i].KeyFrameID == KeyFrame->KeyFrameID)
        {
            KeyFrameIndex = i;
        }
    }

    if (KeyFrameIndex >= 0)
    {
        Track->KeyFrames.RemoveAt(KeyFrameIndex);
    }
}

void SAnimationTimelineWidget::AddKeyFrameToTrack(int32 TrackID, float Time, bool SetValue, FText Value, int32 TextBoxIndex)
{
    if (!KeyFramesCanvas.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("KeyFrameCanvas is not valid"));
        return;
    }
    
    FAnimationTrack* Track = GetTrackOfID(TrackID);

    if (Track == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("No Track of Track ID %d found in AnimationTimeline"), TrackID);
        return;
    }

    if (!TracksFieldMap.Contains(TrackID))
    {
        UE_LOG(LogTemp, Warning, TEXT("TrackID %d does not exist in TracksFieldMap."), TrackID);
        return;
    }

    for (int32 i = 0; i < Track->KeyFrames.Num(); ++i)
    {
        if (Track->KeyFrames[i].Time == Time)
        {
            UE_LOG(LogTemp, Warning, TEXT("A KeyFrame at %f already exist in the Track of ID : %d."), Time, TrackID);
            return;
        }
    }

    FAnimationKeyFrame* KeyFrame = GenerateKeyFrame(TrackID, Time, SetValue, Value, TextBoxIndex);
    if (KeyFrame == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("A KeyFrame at %f can't be generated"), Time);
        return;
    }

    AddKeyFrameSlot(KeyFrame->KeyFrameID);
}

FAnimationKeyFrame* SAnimationTimelineWidget::GenerateKeyFrame(int32 TrackID, float Time, bool SetValue, FText Value, int32 TextBoxIndex)
{
    FAnimationTrack* Track = GetTrackOfID(TrackID);

    if (Track == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("No Track of Track ID %d found in AnimationTimeline"), TrackID);
        return nullptr;
    }

    FAnimationKeyFrame KeyFrame;
    KeyFrame.ParentTrackID = Track->TrackID;
    KeyFrame.KeyFrameID = TimelinePluginWidget->TimelineComponent->GenerateUniqueRandomID();
    KeyFrame.Time = Time;

    if (SetValue)
    {
        if (Track->VariableType == "Boolean")
        {
            if (IsValidBoolean(Value.ToString()))
            {
                if (Value.ToString() == "true")
                {
                    KeyFrame.BoolValue = true;
                }
                else
                {
                    KeyFrame.BoolValue = false;
                }
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Failed to set boolean value from Text."));
                return nullptr;
            }
        }
        else if (Track->VariableType == "Integer")
        {
            if (IsValidInteger(Value.ToString()))
            {
                KeyFrame.IntValue = FCString::Atoi(*Value.ToString());
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Failed to set Integer value from Text."));
                return nullptr;
            }
        }
        else if (Track->VariableType == "Float" || Track->VariableType == "Double")
        {
            if (IsValidFloat(Value.ToString()))
            {
                if (Track->VariableType == "Float")
                {
                    KeyFrame.FloatValueX = FCString::Atof(*Value.ToString());
                }
                else
                {
                    KeyFrame.DoubleValue = FCString::Atof(*Value.ToString());
                }
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Failed to set float value from Text."));
                return nullptr;
            }
        }
        else if (Track->VariableType == "Vector" || Track->VariableType == "Rotator")
        {
            if (IsValidFloat(Value.ToString()))
            {
                float X = GetTrackFloatValueAtTime(Track->TrackID, Time, 0);
                float Y = GetTrackFloatValueAtTime(Track->TrackID, Time, 1);
                float Z = GetTrackFloatValueAtTime(Track->TrackID, Time, 2);

                if (!std::isnan(X) && !std::isnan(Y) && !std::isnan(Z))
                {
                    switch (TextBoxIndex)
                    {
                        case 0:
                            KeyFrame.FloatValueX = FCString::Atof(*Value.ToString());
                            break;
                        case 1:
                            KeyFrame.FloatValueY = FCString::Atof(*Value.ToString());
                            break;
                        case 2:
                            KeyFrame.FloatValueZ = FCString::Atof(*Value.ToString());
                            break;
                        default:
                            UE_LOG(LogTemp, Warning, TEXT("Invalid Index: %d"), TextBoxIndex);
                            return nullptr;
                    }

                    if (TextBoxIndex == 0)
                    {
                        KeyFrame.FloatValueY = Y;
                        KeyFrame.FloatValueZ = Z;
                    }
                    else if (TextBoxIndex == 1)
                    {
                        KeyFrame.FloatValueX = X;
                        KeyFrame.FloatValueZ = Z;
                    }
                    if (TextBoxIndex == 2)
                    {
                        KeyFrame.FloatValueX = X;
                        KeyFrame.FloatValueY = Y;
                    }
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("Text Value is a valid float but failed to set vector others value."));
                    return nullptr;
                }
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Failed to set vector value from Text."));
                return nullptr;
            }
        }
    }
    else 
    {
        if (Track->VariableType == "Boolean")
        {
            std::optional<bool> BoolValue = GetTrackBoolValueAtTime(KeyFrame.ParentTrackID, Time);

            if (BoolValue.has_value())
            {
                KeyFrame.BoolValue = BoolValue.value();
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Failed to get boolean value."));
                return nullptr;
            }
        }
        else if (Track->VariableType == "Integer")
        {
            std::optional<int32> IntValue = GetTrackIntValueAtTime(KeyFrame.ParentTrackID, Time);

            if (IntValue.has_value())
            {
                KeyFrame.IntValue = IntValue.value();
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Failed to get integer value."));
                return nullptr;
            }
        }
        else if (Track->VariableType == "Float")
        {
            float FloatValue = GetTrackFloatValueAtTime(KeyFrame.ParentTrackID, Time, 0);

            if (!std::isnan(FloatValue))
            {
                KeyFrame.FloatValueX = FloatValue;
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Failed to get float value."));
                return nullptr;
            }
        }
        else if (Track->VariableType == "Double")
        {
            double DoubleValue = GetTrackDoubleValueAtTime(KeyFrame.ParentTrackID, Time);

            if (!std::isnan(DoubleValue))
            {
                KeyFrame.DoubleValue = DoubleValue;
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Failed to get double value."));
                return nullptr;
            }
        }
        else if (Track->VariableType == "Vector")
        {
            FVector VectorValue;
            VectorValue.X = GetTrackFloatValueAtTime(KeyFrame.ParentTrackID, Time, 0);
            VectorValue.Y = GetTrackFloatValueAtTime(KeyFrame.ParentTrackID, Time, 1);
            VectorValue.Z = GetTrackFloatValueAtTime(KeyFrame.ParentTrackID, Time, 2);

            if (!std::isnan(VectorValue.X) && !std::isnan(VectorValue.Y) && !std::isnan(VectorValue.Z))
            {
                KeyFrame.FloatValueX = VectorValue.X;
                KeyFrame.FloatValueY = VectorValue.Y;
                KeyFrame.FloatValueZ = VectorValue.Z;
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Failed to get vector value."));
                return nullptr;
            }
        }
        else if (Track->VariableType == "Rotator")
        {
            FRotator RotatorValue;
            RotatorValue.Pitch = GetTrackFloatValueAtTime(KeyFrame.ParentTrackID, Time, 0);
            RotatorValue.Yaw = GetTrackFloatValueAtTime(KeyFrame.ParentTrackID, Time, 1);
            RotatorValue.Roll = GetTrackFloatValueAtTime(KeyFrame.ParentTrackID, Time, 2);

            if (!std::isnan(RotatorValue.Pitch) && !std::isnan(RotatorValue.Yaw) && !std::isnan(RotatorValue.Roll))
            {
                KeyFrame.FloatValueX = RotatorValue.Pitch;
                KeyFrame.FloatValueY = RotatorValue.Yaw;
                KeyFrame.FloatValueZ = RotatorValue.Roll;
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Failed to get rotator value."));
                return nullptr;
            }
        }
    }

    Track->KeyFrames.Add(KeyFrame);
    return &Track->KeyFrames[Track->KeyFrames.Num() - 1];
}

void SAnimationTimelineWidget::OpenKeyFrame(int32 KeyFrameID)
{
    if (KeyFrameEditorWindow.IsValid())
    {
        TSharedRef<SWidget> WidgetRef = KeyFrameEditorWindow->GetContent();
        TSharedRef<SKeyFrameEditorWidget> ExistingWidget = StaticCastSharedRef<SKeyFrameEditorWidget>(WidgetRef);

        if (ExistingWidget->KeyFrame)
        {
            ExistingWidget->KeyFrame->IsOpen = false;
        }

        FSlateApplication::Get().RequestDestroyWindow(KeyFrameEditorWindow.ToSharedRef());
        KeyFrameEditorWindow.Reset();
    }

    FAnimationKeyFrame* KeyFrame = GetKeyFrameOfID(KeyFrameID);
    if (!KeyFrame)
    {
        UE_LOG(LogTemp, Warning, TEXT("No KeyFrame found for ID: %d"), KeyFrameID);
        return;
    }

    FAnimationTrack* Track = GetTrackOfID(KeyFrame->ParentTrackID);
    if (!Track)
    {
        UE_LOG(LogTemp, Warning, TEXT("No Track found for ID: %d"), KeyFrame->ParentTrackID);
        return;
    }

    KeyFrame->IsOpen = true;

    KeyFrameEditorWindow = SNew(SWindow)
        .Title(FText::FromString("Edit KeyFrame"))
        .ClientSize(FVector2D(400, 300))
        .SupportsMaximize(false)
        .SupportsMinimize(false)
        .HasCloseButton(false);

    TSharedRef<SKeyFrameEditorWidget> KeyFrameEditorWidget = SNew(SKeyFrameEditorWidget)
        .ParentWidget(this)
        .Track(Track)
        .KeyFrame(KeyFrame);

    KeyFrameEditorWidget->SetParentWindow(KeyFrameEditorWindow);
    KeyFrameEditorWindow->SetContent(KeyFrameEditorWidget);

    FSlateApplication::Get().AddWindow(KeyFrameEditorWindow.ToSharedRef());
}

void SAnimationTimelineWidget::SetKeyFrameValueAtTime(int32 KeyFrameID, float Time)
{
    FAnimationKeyFrame* KeyFrame = GetKeyFrameOfID(KeyFrameID);

    if (KeyFrame == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("No KeyFrame of KeyFrameID %d found in AnimationTimeline"), KeyFrameID);
        return;
    }

    FAnimationTrack* Track = GetTrackOfID(KeyFrame->ParentTrackID);

    if (Track == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("No Track of Track ID %d found in AnimationTimeline"), KeyFrame->ParentTrackID);
        return;
    }

    if (Track->VariableType == "Boolean")
    {
        std::optional<bool> BoolValue = GetTrackBoolValueAtTime(KeyFrame->ParentTrackID, Time);

        if (BoolValue.has_value())
        {
            KeyFrame->BoolValue = BoolValue.value();
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to get boolean value."));
            return;
        }
    }
    else if (Track->VariableType == "Integer")
    {
        std::optional<int32> IntValue = GetTrackIntValueAtTime(KeyFrame->ParentTrackID, Time);

        if (IntValue.has_value())
        {
            KeyFrame->IntValue = IntValue.value();
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to get integer value."));
            return;
        }
    }
    else if (Track->VariableType == "Float")
    {
        float FloatValue = GetTrackFloatValueAtTime(KeyFrame->ParentTrackID, Time, 0);

        if (!std::isnan(FloatValue))
        {
            KeyFrame->FloatValueX = FloatValue;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to get float value."));
            return;
        }
    }
    else if (Track->VariableType == "Double")
    {
        double DoubleValue = GetTrackDoubleValueAtTime(KeyFrame->ParentTrackID, Time);

        if (!std::isnan(DoubleValue))
        {
            KeyFrame->DoubleValue = DoubleValue;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to get double value."));
            return;
        }
    }
    else if (Track->VariableType == "Vector")
    {
        FVector VectorValue;
        VectorValue.X = GetTrackFloatValueAtTime(KeyFrame->ParentTrackID, Time, 0);
        VectorValue.Y = GetTrackFloatValueAtTime(KeyFrame->ParentTrackID, Time, 1);
        VectorValue.Z = GetTrackFloatValueAtTime(KeyFrame->ParentTrackID, Time, 2);

        if (!std::isnan(VectorValue.X) && !std::isnan(VectorValue.Y) && !std::isnan(VectorValue.Z))
        {
            KeyFrame->FloatValueX = VectorValue.X;
            KeyFrame->FloatValueY = VectorValue.Y;
            KeyFrame->FloatValueZ = VectorValue.Z;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to get vector value."));
            return;
        }
    }
    else if (Track->VariableType == "Rotator")
    {
        FRotator RotatorValue;
        RotatorValue.Pitch = GetTrackFloatValueAtTime(KeyFrame->ParentTrackID, Time, 0);
        RotatorValue.Yaw = GetTrackFloatValueAtTime(KeyFrame->ParentTrackID, Time, 1);
        RotatorValue.Roll = GetTrackFloatValueAtTime(KeyFrame->ParentTrackID, Time, 2);

        if (!std::isnan(RotatorValue.Pitch) && !std::isnan(RotatorValue.Yaw) && !std::isnan(RotatorValue.Roll))
        {
            KeyFrame->FloatValueX = RotatorValue.Pitch;
            KeyFrame->FloatValueY = RotatorValue.Yaw;
            KeyFrame->FloatValueZ = RotatorValue.Roll;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to get rotator value."));
            return;
        }
    }
}

void SAnimationTimelineWidget::SetTimelineDuration()
{
    if (AnimationTimeline->Duration > 1)
    {
        TrackFieldWidth = (AnimationTimeline->Duration * 100.0f) * (100.0f / TimeScale);
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
    float NewTime = GetTimeFromClick(LocalClickPosition);

    if (NewTime >= 0.0f && NewTime <= AnimationTimeline->Duration)
    {
        CurrentTime = NewTime;
        SetTracksValueAtTime(CurrentTime);
    }

    return FReply::Handled();
}

float SAnimationTimelineWidget::GetTimeFromClick(const FVector2D& ClickPosition) const
{
    float ClickX = ClickPosition.X;
    float Time = FMath::Clamp((ClickX / TrackFieldWidth) * AnimationTimeline->Duration, 0.0f, AnimationTimeline->Duration);
    return FMath::RoundToFloat(Time * 100.0f) / 100.0f;
}

TOptional<FSlateRenderTransform> SAnimationTimelineWidget::GetCursorTransform() const
{
    if (AnimationTimeline->Duration <= 0.0f || TrackFieldWidth <= 0.0f)
    {
        return TOptional<FSlateRenderTransform>();
    }

    float CursorPositionX = (CurrentTime / AnimationTimeline->Duration) * TrackFieldWidth;
    return FSlateRenderTransform(FVector2D(CursorPositionX, 0.0f));
}

TOptional<FSlateRenderTransform> SAnimationTimelineWidget::GetKeyFrameTransform(int32 KeyFrameID)
{
    FAnimationKeyFrame* KeyFrame = GetKeyFrameOfID(KeyFrameID);

    if (KeyFrame == nullptr)
    {
        return TOptional<FSlateRenderTransform>();
    }

    if (AnimationTimeline->Duration <= 0.0f || TrackFieldWidth <= 0.0f)
    {
        return TOptional<FSlateRenderTransform>();
    }

    int32 TrackIndex = -1;

    for (int32 i = 0; i < AnimationTimeline->Tracks.Num(); ++i)
    {
        if (AnimationTimeline->Tracks[i].TrackID == KeyFrame->ParentTrackID)
        {
            TrackIndex = i;
            break;
        }
    }

    float KeyFramePositionX = (KeyFrame->Time / AnimationTimeline->Duration) * TrackFieldWidth;
    float KeyFramePositionY = 45 + (TrackIndex * 30);

    return FSlateRenderTransform(FVector2D(KeyFramePositionX, KeyFramePositionY));
}

void SAnimationTimelineWidget::SetTracksValueAtTime(float Time)
{
    TArray<int32> IDs;

    for (const TPair<int32, FAnimationTrack*>& TrackPair : TracksMap)
    {
        int32 Key = TrackPair.Key;
        FAnimationTrack* Value = TrackPair.Value;

        if (Value)
        {
            IDs.Add(Key);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Key: %d, Value: nullptr"), Key);
        }
    }

    for (int32 i = 0; i < IDs.Num(); ++i)
    {
        SetTrackValueAtTime(IDs[i], Time);
    }
}

void SAnimationTimelineWidget::SetTrackValueAtTime(int32 TrackID, float Time)
{
    if (ValuesTextBoxMap.Contains(TrackID))
    {
        TArray<TSharedPtr<SEditableTextBox>>& TextBoxes = ValuesTextBoxMap[TrackID];

        for (int32 j = 0; j < TextBoxes.Num(); ++j)
        {
            if (TextBoxes[j].IsValid())
            {
                TextBoxes[j]->SetText(GetTrackValueAtTimeAsText(TrackID, Time, j)); 
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("TextBox %d is invalid"), j);
            }
        }

        for (int32 i = 0; i < AnimationTimeline->Tracks.Num(); ++i)
        {
            if (AnimationTimeline->Tracks[i].TrackID == TrackID)
            {
                TimelinePluginWidget->TimelineComponent->ApplyTrackValueAtTimeToVariable(i, Time);
            }
        } 
    }
}

void SAnimationTimelineWidget::OnTextBoxCommittedForTrack(const FText& NewText, ETextCommit::Type CommitType, int32 TrackID, int32 TextBoxIndex)
{
    FAnimationTrack* Track = GetTrackOfID(TrackID);

    if (Track == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("No Track of Track ID %d found in AnimationTimeline"), TrackID);
        return;
    }
    else 
    {
        TArray<TSharedPtr<SEditableTextBox>> TextBoxes = ValuesTextBoxMap[TrackID];

        if (TextBoxes.Num() >= TextBoxIndex)
        {
            TSharedPtr<SEditableTextBox> TextBox = TextBoxes[TextBoxIndex];

            if (TextBox)
            {
                std::optional<bool> HasKeyFrameAtTime = DoesTrackContainKeyFrameAtTime(TrackID, CurrentTime);

                if (!HasKeyFrameAtTime.has_value())
                {
                    UE_LOG(LogTemp, Warning, TEXT("DoesTrackContainKeyFrameAtTime() return invalid value trying to get Track of ID : %d."), TrackID);
                }
                else
                {
                    if (Track->VariableType == "Boolean")
                    {
                        if (IsValidBoolean(NewText.ToString()))
                        {
                            if (HasKeyFrameAtTime.value())
                            {
                                int32 KeyFrameIndex = GetIndexOfKeyFrameAtTimeInTrack(TrackID, CurrentTime);

                                if (KeyFrameIndex != -1)
                                {
                                    if (NewText.ToString() == "true")
                                    {
                                        Track->KeyFrames[KeyFrameIndex].BoolValue = true;
                                    }
                                    else
                                    {
                                        Track->KeyFrames[KeyFrameIndex].BoolValue = false;
                                    }
                                }
                                else
                                {
                                    UE_LOG(LogTemp, Warning, TEXT("Failed to get index of KeyFrame of TrackID : %d."), TrackID);
                                }
                            }
                            else
                            {
                                AddKeyFrameToTrack(TrackID, CurrentTime, true, NewText, TextBoxIndex);
                            }
                        }
                        else
                        {
                            UE_LOG(LogTemp, Warning, TEXT("NewText not a valid Boolean"));
                        }
                    }
                    else if (Track->VariableType == "Integer")
                    {
                        if (IsValidInteger(NewText.ToString()))
                        {
                            if (HasKeyFrameAtTime.value())
                            {
                                int32 KeyFrameIndex = GetIndexOfKeyFrameAtTimeInTrack(TrackID, CurrentTime);

                                if (KeyFrameIndex != -1)
                                {
                                    int32 value = FCString::Atoi(*NewText.ToString());
                                    Track->KeyFrames[KeyFrameIndex].IntValue = value;
                                }
                                else
                                {
                                    UE_LOG(LogTemp, Warning, TEXT("Failed to get index of KeyFrame of TrackID : %d."), TrackID);
                                }
                            }
                            else
                            {
                                AddKeyFrameToTrack(TrackID, CurrentTime, true, NewText, TextBoxIndex);
                            }
                        }
                        else
                        {
                            UE_LOG(LogTemp, Warning, TEXT("NewText not a valid Integer"));
                        }
                    }
                    else if (Track->VariableType == "Float" || Track->VariableType == "Double")
                    {
                        if (IsValidFloat(NewText.ToString()))
                        {
                            if (HasKeyFrameAtTime.value())
                            {
                                int32 KeyFrameIndex = GetIndexOfKeyFrameAtTimeInTrack(TrackID, CurrentTime);

                                if (KeyFrameIndex != -1)
                                {
                                    float Value = FCString::Atof(*NewText.ToString());

                                    if (Track->VariableType == "Float")
                                    {
                                        Track->KeyFrames[KeyFrameIndex].FloatValueX = Value;
                                    }
                                    else
                                    {
                                        Track->KeyFrames[KeyFrameIndex].DoubleValue = Value;
                                    }
                                }
                                else
                                {
                                    UE_LOG(LogTemp, Warning, TEXT("Failed to get index of KeyFrame of TrackID : %d."), TrackID);
                                }
                            }
                            else
                            {
                                AddKeyFrameToTrack(TrackID, CurrentTime, true, NewText, TextBoxIndex);
                            }
                        }
                        else
                        {
                            UE_LOG(LogTemp, Warning, TEXT("NewText not a valid Float"));
                        }
                    }
                    else if (Track->VariableType == "Vector" || Track->VariableType == "Rotator")
                    {
                        if (IsValidFloat(NewText.ToString()))
                        {
                            
                            if (HasKeyFrameAtTime.value())
                            {
                                int32 KeyFrameIndex = GetIndexOfKeyFrameAtTimeInTrack(TrackID, CurrentTime);

                                if (KeyFrameIndex != -1)
                                {
                                    float Value = FCString::Atof(*NewText.ToString());

                                    switch (TextBoxIndex)
                                    {
                                        case 0: 
                                            Track->KeyFrames[KeyFrameIndex].FloatValueX = Value;
                                            break;
                                        case 1: 
                                            Track->KeyFrames[KeyFrameIndex].FloatValueY = Value;
                                            break;
                                        case 2: 
                                            Track->KeyFrames[KeyFrameIndex].FloatValueZ = Value;
                                            break;
                                    }
                                }
                                else
                                {
                                    UE_LOG(LogTemp, Warning, TEXT("Failed to get index of KeyFrame of TrackID : %d."), TrackID);
                                }
                            }
                            else
                            {
                                AddKeyFrameToTrack(TrackID, CurrentTime, true, NewText, TextBoxIndex);
                            }
                        }
                        else
                        {
                            UE_LOG(LogTemp, Warning, TEXT("NewText not a valid Float"));
                        }
                    }
                }

                UE_LOG(LogTemp, Log, TEXT("Track Updated: %s, Index: %d, New Value: %s"), *Track->VariableName.ToString(), TextBoxIndex, *NewText.ToString());

                TextBox->SetText(GetTrackValueAtTimeAsText(TrackID, CurrentTime, TextBoxIndex));
                SetTrackValueAtTime(TrackID, CurrentTime);
            }
        }
    }
}

FAnimationTrack* SAnimationTimelineWidget::GetTrackOfID(int32 TrackID)
{
    FAnimationTrack* Track = nullptr;

    for (int32 i = 0; i < AnimationTimeline->Tracks.Num(); ++i)
    {
        if (AnimationTimeline->Tracks[i].TrackID == TrackID)
        {
            return &AnimationTimeline->Tracks[i];
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("No Track of Track ID %d found in AnimationTimeline"), TrackID);
    return nullptr;
}

FAnimationKeyFrame* SAnimationTimelineWidget::GetKeyFrameOfID(int32 KeyFrameID)
{
    FAnimationKeyFrame* KeyFrame = nullptr;

    for (int32 i = 0; i < AnimationTimeline->Tracks.Num(); ++i)
    {
        for (int32 j = 0; j < AnimationTimeline->Tracks[i].KeyFrames.Num(); ++j)
        {
            if (AnimationTimeline->Tracks[i].KeyFrames[j].KeyFrameID == KeyFrameID)
            {
                return &AnimationTimeline->Tracks[i].KeyFrames[j];
            }
        }
    }

    return nullptr;
}

bool SAnimationTimelineWidget::IsValidBoolean(const FString& InputString)
{
    if (InputString == "true" || InputString == "false")
    {
        return true;
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("TIMELINE PLUGIN : Value not set as valid Boolean!"));
        return false;
    }
}

bool SAnimationTimelineWidget::IsValidInteger(const FString& InputString)
{
    if (InputString.IsEmpty())
    {
        UE_LOG(LogTemp, Log, TEXT("TIMELINE PLUGIN : Value not set as valid Integer!"));
        return false;
    }

    for (int32 i = 0; i < InputString.Len(); ++i)
    {
        if (i == 0 && InputString[i] == '-')
        {
            continue;
        }
        if (!FChar::IsDigit(InputString[i]))
        {
            UE_LOG(LogTemp, Log, TEXT("TIMELINE PLUGIN : Value not set as valid Integer!"));
            return false;
        }
    }

    return true;
}

bool SAnimationTimelineWidget::IsValidFloat(const FString& InputString)
{
    const std::string& str = TCHAR_TO_UTF8(*InputString);
    std::istringstream iss(str);
    float value;
    iss >> value;

    if (!iss.fail() && iss.eof())
    {
        return true;
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("TIMELINE PLUGIN : Value not set as valid Float!"));
        return false;
    }
}

std::optional<bool> SAnimationTimelineWidget::DoesTrackContainKeyFrameAtTime(int32 TrackID, float Time)
{
    FAnimationTrack* Track = GetTrackOfID(TrackID);

    if (Track == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("No Track of Track ID %d found in AnimationTimeline"), TrackID);
        return std::nullopt;
    }

    for (int32 i = 0; i < Track->KeyFrames.Num(); ++i)
    {
        if (Track->KeyFrames[i].Time == Time)
        {
            return true;
        }
    }

    return false;
}

int32 SAnimationTimelineWidget::GetIndexOfKeyFrameAtTimeInTrack(int32 TrackID, float Time)
{
    FAnimationTrack* Track = GetTrackOfID(TrackID);

    if (Track == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("No Track of Track ID %d found in AnimationTimeline"), TrackID);
        return -1;
    }

    for (int32 i = 0; i < Track->KeyFrames.Num(); ++i)
    {
        if (Track->KeyFrames[i].Time == Time)
        {
            return i;
        }
    }

    return -1;
}

FText SAnimationTimelineWidget::GetTrackDefaultValueAsText(int32 TrackID, int32 Index)
{
    FAnimationTrack* Track = GetTrackOfID(TrackID);

    if (Track == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("No Track of Track ID %d found in AnimationTimeline"), TrackID);
        return FText::FromString("null");
    }

    if (Track->VariableType == "Boolean")
    {
        if (Track->DefaultBoolValue)
        {
            return FText::FromString("true");
        }
        else
        {
            return FText::FromString("false");
        }
    }
    else if (Track->VariableType == "Integer")
    {
        return FText::FromString(FString::FromInt(Track->DefaultIntValue));
    }
    else if (Track->VariableType == "Float")
    {
        return FText::FromString(FString::SanitizeFloat(Track->DefaultFloatValueX));
    }
    else if (Track->VariableType == "Double")
    {
        return FText::FromString(FString::Printf(TEXT("%.15g"), Track->DefaultDoubleValue));
    }
    else if (Track->VariableType == "Vector" || Track->VariableType == "Rotator")
    {
        if (Index == 0)
        {
            return FText::FromString(FString::SanitizeFloat(Track->DefaultFloatValueX));
        }
        else if (Index == 1)
        {
            return FText::FromString(FString::SanitizeFloat(Track->DefaultFloatValueY));
        }
        else if (Index == 2)
        {
            return FText::FromString(FString::SanitizeFloat(Track->DefaultFloatValueZ));
        }
    }

    return FText::FromString("null");
}

FText SAnimationTimelineWidget::GetTrackValueAtTimeAsText(int32 TrackID, float Time, int32 Index)
{
    FAnimationTrack* Track = GetTrackOfID(TrackID);

    if (Track == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("No Track of Track ID %d found in AnimationTimeline"), TrackID);
        return FText::FromString("null");
    }

    if (Track->VariableType == "Boolean")
    {
        std::optional<bool> BoolValue = GetTrackBoolValueAtTime(TrackID, Time);

        if (BoolValue.has_value())
        {
            return BoolValue.value() ? FText::FromString("true") : FText::FromString("false");
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to retrieve boolean value for Track ID %d"), Track->TrackID);
            return FText::FromString("null");
        }
    }
    else if (Track->VariableType == "Integer")
    {
        std::optional<int32> IntValue = GetTrackIntValueAtTime(TrackID, Time);

        if (IntValue.has_value())
        {
            return FText::FromString(FString::FromInt(IntValue.value()));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to retrieve integer value for Track ID %d"), Track->TrackID);
            return FText::FromString("null");
        }
    }
    else if (Track->VariableType == "Float")
    {
        float FloatValue = GetTrackFloatValueAtTime(TrackID, Time, 0);

        if (!std::isnan(FloatValue))
        {
            return FText::FromString(FString::SanitizeFloat(FloatValue));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to retrieve float value for Track ID %d"), Track->TrackID);
            return FText::FromString("null");
        }
    }
    else if (Track->VariableType == "Double")
    {
        double DoubleValue = GetTrackDoubleValueAtTime(TrackID, Time);

        if (!std::isnan(DoubleValue))
        {
            return FText::FromString(FString::Printf(TEXT("%.15g"), DoubleValue));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to retrieve double value for Track ID %d"), Track->TrackID);
            return FText::FromString("null");
        }
    }
    else if (Track->VariableType == "Vector" || Track->VariableType == "Rotator")
    {
        float FloatValue = GetTrackFloatValueAtTime(TrackID, Time, Index);

        if (!std::isnan(FloatValue))
        {
            return FText::FromString(FString::SanitizeFloat(FloatValue));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to retrieve vector/rotator value for Track ID %d"), Track->TrackID);
            return FText::FromString("null");
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Unsupported variable type for Track ID %d"), Track->TrackID);
    return FText::FromString("null");
}

std::optional<bool> SAnimationTimelineWidget::GetTrackBoolValueAtTime(int32 TrackID, float Time)
{
    FAnimationTrack* Track = GetTrackOfID(TrackID);

    if (Track == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("No Track of Track ID %d found in AnimationTimeline"), TrackID);
        return std::nullopt;
    }

    FAnimationKeyFrame* StartKey = nullptr;
    FAnimationKeyFrame* EndKey = nullptr;

    for (int32 i = 0; i < Track->KeyFrames.Num(); ++i)
    {
        if (Track->KeyFrames[i].Time < Time)
        {            
            if (StartKey)
            {
                if (Track->KeyFrames[i].Time > StartKey->Time)
                {
                    StartKey = &Track->KeyFrames[i];
                }
            }
            else
            {
                StartKey = &Track->KeyFrames[i];
            }
        }
        else if (Track->KeyFrames[i].Time == Time)
        {
            return Track->KeyFrames[i].BoolValue;
        }
        else
        {
            if (EndKey)
            {
                if (Track->KeyFrames[i].Time < EndKey->Time)
                {
                    EndKey = &Track->KeyFrames[i];
                }
            }
            else
            {
                EndKey = &Track->KeyFrames[i];
            }
        }
    }

    if (!StartKey)
    {
        return Track->DefaultBoolValue;
    }
    else
    {
        return StartKey->BoolValue;
    }
}

std::optional<int32> SAnimationTimelineWidget::GetTrackIntValueAtTime(int32 TrackID, float Time)
{
    FAnimationTrack* Track = GetTrackOfID(TrackID);

    if (Track == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("No Track of Track ID %d found in AnimationTimeline"), TrackID);
        return std::nullopt;
    }

    FAnimationKeyFrame* StartKey = nullptr;
    FAnimationKeyFrame* EndKey = nullptr;

    for (int32 i = 0; i < Track->KeyFrames.Num(); ++i)
    {
        if (Track->KeyFrames[i].Time < Time)
        {            
            if (StartKey)
            {
                if (Track->KeyFrames[i].Time > StartKey->Time)
                {
                    StartKey = &Track->KeyFrames[i];
                }
            }
            else
            {
                StartKey = &Track->KeyFrames[i];
            }
        }
        else if (Track->KeyFrames[i].Time == Time)
        {
            return Track->KeyFrames[i].IntValue;
        }
        else
        {
            if (EndKey)
            {
                if (Track->KeyFrames[i].Time < EndKey->Time)
                {
                    EndKey = &Track->KeyFrames[i];
                }
            }
            else
            {
                EndKey = &Track->KeyFrames[i];
            }
        }
    }

    if (!StartKey)
    {
        return Track->DefaultIntValue;
    }

    if (!EndKey)
    {
        return StartKey->IntValue;
    }
    else 
    {
        float Alpha = GetAlphaAtTime(StartKey->Time, EndKey->Time, Time);
        return InterpolateInteger(StartKey->IntValue, EndKey->IntValue, Alpha, StartKey->InterpolationType);
    }
}

float SAnimationTimelineWidget::GetTrackFloatValueAtTime(int32 TrackID, float Time, int32 Index)
{
    FAnimationTrack* Track = GetTrackOfID(TrackID);

    if (Track == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("No Track of Track ID %d found in AnimationTimeline"), TrackID);
        return NAN;
    }

    FAnimationKeyFrame* StartKey = nullptr;
    FAnimationKeyFrame* EndKey = nullptr;

    for (int32 i = 0; i < Track->KeyFrames.Num(); ++i)
    {
        if (Track->KeyFrames[i].Time < Time)
        {            
            if (StartKey)
            {
                if (Track->KeyFrames[i].Time > StartKey->Time)
                {
                    StartKey = &Track->KeyFrames[i];
                }
            }
            else
            {
                StartKey = &Track->KeyFrames[i];
            }
        }
        else if (Track->KeyFrames[i].Time == Time)
        {
            switch (Index)
            {
                case 0: return Track->KeyFrames[i].FloatValueX;
                case 1: return Track->KeyFrames[i].FloatValueY;
                case 2: return Track->KeyFrames[i].FloatValueZ;
                default:
                    UE_LOG(LogTemp, Warning, TEXT("Invalid Index: %d"), Index);
                    return NAN;
            }
        }
        else
        {
            if (EndKey)
            {
                if (Track->KeyFrames[i].Time < EndKey->Time)
                {
                    EndKey = &Track->KeyFrames[i];
                }
            }
            else
            {
                EndKey = &Track->KeyFrames[i];
            }
        }
    }

    if (!StartKey)
    {
        switch (Index)
        {
            case 0: return Track->DefaultFloatValueX;
            case 1: return Track->DefaultFloatValueY;
            case 2: return Track->DefaultFloatValueZ;
            default:
                UE_LOG(LogTemp, Warning, TEXT("Invalid Index: %d"), Index);
                return NAN;
        }
    }

    if (!EndKey)
    {
        switch (Index)
        {
            case 0: return StartKey->FloatValueX;
            case 1: return StartKey->FloatValueY;
            case 2: return StartKey->FloatValueZ;
            default:
                UE_LOG(LogTemp, Warning, TEXT("Invalid Index: %d"), Index);
                return NAN;
        }
    }
    else
    {
        float Alpha = GetAlphaAtTime(StartKey->Time, EndKey->Time, Time);

        switch (Index)
        {
            case 0: return InterpolateFloat(StartKey->FloatValueX, EndKey->FloatValueX, Alpha, StartKey->InterpolationType);
            case 1: return InterpolateFloat(StartKey->FloatValueY, EndKey->FloatValueY, Alpha, StartKey->InterpolationType);
            case 2: return InterpolateFloat(StartKey->FloatValueZ, EndKey->FloatValueZ, Alpha, StartKey->InterpolationType);
            default:
                UE_LOG(LogTemp, Warning, TEXT("Invalid Index: %d"), Index);
                return NAN;
        }
    }
}

double SAnimationTimelineWidget::GetTrackDoubleValueAtTime(int32 TrackID, float Time)
{
    FAnimationTrack* Track = GetTrackOfID(TrackID);

    if (Track == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("No Track of Track ID %d found in AnimationTimeline"), TrackID);
        return NAN;
    }

    FAnimationKeyFrame* StartKey = nullptr;
    FAnimationKeyFrame* EndKey = nullptr;

    for (int32 i = 0; i < Track->KeyFrames.Num(); ++i)
    {
        if (Track->KeyFrames[i].Time < Time)
        {            
            if (StartKey)
            {
                if (Track->KeyFrames[i].Time > StartKey->Time)
                {
                    StartKey = &Track->KeyFrames[i];
                }
            }
            else
            {
                StartKey = &Track->KeyFrames[i];
            }
        }
        else if (Track->KeyFrames[i].Time == Time)
        {
            return Track->KeyFrames[i].DoubleValue;
        }
        else
        {
            if (EndKey)
            {
                if (Track->KeyFrames[i].Time < EndKey->Time)
                {
                    EndKey = &Track->KeyFrames[i];
                }
            }
            else
            {
                EndKey = &Track->KeyFrames[i];
            }
        }
    }

    if (!StartKey)
    {
        return Track->DefaultDoubleValue;
    }

    if (!EndKey)
    {
        return StartKey->DoubleValue;
    }
    else 
    {
        float Alpha = GetAlphaAtTime(StartKey->Time, EndKey->Time, Time);
        return InterpolateFloat(StartKey->DoubleValue, EndKey->DoubleValue, Alpha, StartKey->InterpolationType);
    }
}

float SAnimationTimelineWidget::GetAlphaAtTime(float Start, float End, float Time)
{
    if (FMath::IsNearlyEqual(Start, End))
    {
        UE_LOG(LogTemp, Warning, TEXT("Start and End are nearly equal. Returning 0.0f."));
        return 0.0f;
    }

    float Alpha = (Time - Start) / (End - Start);

    return FMath::Clamp(Alpha, 0.0f, 1.0f);
}

float SAnimationTimelineWidget::InterpolateFloat(float StartValue, float EndValue, float Alpha, EInterpolationType InterpolationType)
{
    switch (InterpolationType)
    {
        case EInterpolationType::Linear:
            return FMath::Lerp(StartValue, EndValue, Alpha);
        case EInterpolationType::QuadraticIn:
            return FMath::InterpEaseIn(StartValue, EndValue, Alpha, 2.0f);
        case EInterpolationType::QuadraticOut:
            return FMath::InterpEaseOut(StartValue, EndValue, Alpha, 2.0f);
        case EInterpolationType::QuadraticInOut:
            return FMath::InterpEaseInOut(StartValue, EndValue, Alpha, 2.0f);
        case EInterpolationType::CubicIn:
            return FMath::InterpEaseIn(StartValue, EndValue, Alpha, 3.0f);
        case EInterpolationType::CubicOut:
            return FMath::InterpEaseOut(StartValue, EndValue, Alpha, 3.0f);
        case EInterpolationType::CubicInOut:
            return FMath::InterpEaseInOut(StartValue, EndValue, Alpha, 3.0f);
        case EInterpolationType::ExponentialIn:
            return FMath::InterpExpoIn(StartValue, EndValue, Alpha);
        case EInterpolationType::ExponentialOut:
            return FMath::InterpExpoOut(StartValue, EndValue, Alpha);
        case EInterpolationType::ExponentialInOut:
            return FMath::InterpExpoInOut(StartValue, EndValue, Alpha);
        case EInterpolationType::SineIn:
            return FMath::InterpSinIn(StartValue, EndValue, Alpha);
        case EInterpolationType::SineOut:
            return FMath::InterpSinOut(StartValue, EndValue, Alpha);
        case EInterpolationType::SineInOut:
            return FMath::InterpSinInOut(StartValue, EndValue, Alpha);
        default:
            return StartValue;
    }
}

int32 SAnimationTimelineWidget::InterpolateInteger(int32 StartValue, int32 EndValue, float Alpha, EInterpolationType InterpolationType)
{
    return FMath::RoundToInt(InterpolateFloat((float)StartValue, (float)EndValue, Alpha, InterpolationType));
}

double SAnimationTimelineWidget::InterpolateDouble(double StartValue, double EndValue, float Alpha, EInterpolationType InterpolationType)
{
    return InterpolateFloat(StartValue, EndValue, Alpha, InterpolationType);
}

FVector SAnimationTimelineWidget::InterpolateVector(FVector StartValue, FVector EndValue, float Alpha, EInterpolationType InterpolationType)
{
    FVector VectorValue;
    VectorValue.X = InterpolateFloat(StartValue.X, EndValue.X, Alpha, InterpolationType);
    VectorValue.Y = InterpolateFloat(StartValue.Y, EndValue.Y, Alpha, InterpolationType);
    VectorValue.Z = InterpolateFloat(StartValue.Z, EndValue.Z, Alpha, InterpolationType);
    return VectorValue;
}

FRotator SAnimationTimelineWidget::InterpolateRotator(FRotator StartValue, FRotator EndValue, float Alpha, EInterpolationType InterpolationType)
{
    FRotator RotatorValue;
    RotatorValue.Pitch = InterpolateFloat(StartValue.Pitch, EndValue.Pitch, Alpha, InterpolationType);
    RotatorValue.Yaw = InterpolateFloat(StartValue.Yaw, EndValue.Yaw, Alpha, InterpolationType);
    RotatorValue.Roll = InterpolateFloat(StartValue.Roll, EndValue.Roll, Alpha, InterpolationType);
    return RotatorValue;
}