#include "TimelinePluginWidget.h"
#include "TimelinePluginComponent.h"
#include "DetailLayoutBuilder.h"
#include "Editor/UnrealEd/Public/Editor.h"
#include "Engine/Selection.h"                   //Use for GEditor to get a reference to TimelineComponent
#include "GameFramework/Actor.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Text/STextBlock.h"
#include <string>
#include <sstream>
#include <iostream>

TSharedRef<IDetailCustomization> UTimelinePluginWidget::MakeInstance()
{
    return MakeShareable(new UTimelinePluginWidget());
}

void UTimelinePluginWidget::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
    TArray<UObject*> SelectedObjects;
    GEditor->GetSelectedActors()->GetSelectedObjects(SelectedObjects);  // GEditor acc�de aux objets s�lectionn�s dans l'�diteur

    if (SelectedObjects.Num() > 0)
    {
        AActor* Actor = Cast<AActor>(SelectedObjects[0]);
        if (Actor)
        {
            // Trouver le composant TimelinePluginComponent attach� � cet acteur
            TimelineComponent = Actor->FindComponentByClass<UTimelinePluginComponent>();

            if (!TimelineComponent)
            {
                UE_LOG(LogTemp, Warning, TEXT("TIMELINE PLUGIN : TimelineComponent Not Found in TimelineWIdget!"));
            }
            else
            {
                TimelineComponent->TimelinePluginWidget = this;
            }
        }
    }

    IDetailCategoryBuilder& TimelineVariableCategory = DetailBuilder.EditCategory("Timeline Variables");
    IDetailCategoryBuilder& TimelineWidgetCategory = DetailBuilder.EditCategory("Timeline Animation");

    AddDropDowns(TimelineVariableCategory);
    AddResetAndUpdateButton(TimelineVariableCategory);
    AddTimelineDurationTextField(TimelineVariableCategory);

    AddTimelineWidget(TimelineWidgetCategory);

    InitializeComponent();
}

void UTimelinePluginWidget::InitializeComponent() 
{
    if (!TimelineComponent->IsInitialized)
    {
        TimelineComponent->InitializeComponent();
    }
    
    TimelineComponent->AnimationTimeline.Duration = 5.0f;
    TimelineComponent->AnimationTimelineDuration = 5.0f;
}





//Create & Manage DropDowns
void UTimelinePluginWidget::AddDropDowns(IDetailCategoryBuilder& Category)
{
    FSlateFontInfo SmallFont = FCoreStyle::GetDefaultFontStyle("Regular", 8);

    // Peupler les options pour le type et s'assurer que SelectedType corresponde au premier index
    TypeOptions.Empty();
    for (const FString& Type : TimelineComponent->AvailableTypes)
    {
        TypeOptions.Add(MakeShared<FString>(Type));
    }

    if (TimelineComponent->AvailableTypes.Num() > 0)
    {
        SelectedType = MakeShared<FString>(TimelineComponent->AvailableTypes[0]);
    }

    // Peupler les options pour les variables trackables
    VariableOptions.Empty();
    for (const FString& Variable : TimelineComponent->AvailableVariables)
    {
        VariableOptions.Add(MakeShared<FString>(Variable));
    }

    // Ajouter les dropdowns dans la cat�gorie
    Category.AddCustomRow(FText::FromString("Type and Variable"))
        .NameContent()
        [
            SNew(STextBlock).Text(FText::FromString("Variables To Track")).Font(SmallFont)
        ]
        .ValueContent()
        [
            SNew(SVerticalBox)

                // Dropdown pour le type
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SComboBox<TSharedPtr<FString>>)
                        .OptionsSource(&TypeOptions)
                        .OnSelectionChanged(this, &UTimelinePluginWidget::OnTypeSelected)
                        .OnGenerateWidget_Lambda([SmallFont](TSharedPtr<FString> Item)
                            {
                                return SNew(STextBlock)
                                    .Text(FText::FromString(*Item))
                                    .Font(SmallFont);
                            })
                        .Content()
                        [
                            SNew(STextBlock)
                                .Text_Lambda([this]()
                                    {
                                        return SelectedType.IsValid() ? FText::FromString(*SelectedType) : FText::FromString("Choose Type");
                                    })
                                .Font(SmallFont)
                        ]
                ]

                // Espacement entre les deux combo boxes
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(0.0f, 1.0f)

                // Dropdown pour la variable
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SComboBox<TSharedPtr<FString>>)
                        .OptionsSource(&VariableOptions)
                        .OnSelectionChanged(this, &UTimelinePluginWidget::OnVariableSelected)
                        .OnGenerateWidget_Lambda([SmallFont](TSharedPtr<FString> Item)
                            {
                                return SNew(STextBlock)
                                    .Text(FText::FromString(*Item))
                                    .Font(SmallFont);
                            })
                        .Content()
                        [
                            SNew(STextBlock)
                                .Text_Lambda([this]()
                                    {
                                        return SelectedVariable.IsValid() ? FText::FromString(*SelectedVariable) : FText::FromString("Choose Variable");
                                    })
                                .Font(SmallFont)
                        ]
                ]
        ];
}

void UTimelinePluginWidget::OnTypeSelected(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo)
{
    if (NewSelection.IsValid())
    {
        UE_LOG(LogTemp, Log, TEXT("TIMELINE PLUGIN : Type selected: %s"), **NewSelection);

        // Logique de mise � jour pour les types disponibles
        TimelineComponent->OnTypeSelected(NewSelection);

        // Repeupler les options
        VariableOptions.Empty();
        for (const FString& Variable : TimelineComponent->AvailableVariables)
        {
            VariableOptions.Add(MakeShared<FString>(Variable));
        }

        SelectedType = NewSelection;
        SelectedVariable = nullptr;
    }
}

void UTimelinePluginWidget::OnVariableSelected(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo)
{
    if (NewSelection.IsValid())
    {
        UE_LOG(LogTemp, Log, TEXT("TIMELINE PLUGIN : Variable selected: %s"), **NewSelection);

        // Ajout de la variable s�lectionn�e � la liste track�e
        TimelineComponent->OnVariableSelected(NewSelection);

        SelectedVariable = NewSelection;
    }
}





// Create & Manage Buttons
void UTimelinePluginWidget::AddResetAndUpdateButton(IDetailCategoryBuilder& Category)
{
    FSlateFontInfo SmallFont = FCoreStyle::GetDefaultFontStyle("Regular", 8);

    Category.AddCustomRow(FText::FromString("Variables Management"))
        .NameContent()
        [
            SNew(STextBlock).Text(FText::FromString("Variables Management")).Font(SmallFont)
        ]
        .ValueContent()
        [
            SNew(SVerticalBox)

                // Dropdown pour le type
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SButton)
                        .Text(FText::FromString("Reset Tracked Variables"))
                        .Content()
                        [
                            SNew(STextBlock)
                                .Text(FText::FromString("Reset Tracked Variables"))
                                .Font(SmallFont)
                        ]
                        .OnClicked(this, &UTimelinePluginWidget::ResetTrackedVariables)
                ]

                // Espacement entre les deux combo boxes
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(0.0f, 1.0f)

                // Dropdown pour la variable
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SButton)
                        .Text(FText::FromString("Update Available Variables"))
                        .Content()
                        [
                            SNew(STextBlock)
                                .Text(FText::FromString("Update Available Variables"))
                                .Font(SmallFont)
                        ]
                        .OnClicked(this, &UTimelinePluginWidget::UpdateAvailableVariables)
                ]
        ];
}

FReply UTimelinePluginWidget::ResetTrackedVariables()
{
    TimelineComponent->TrackedVariables.Empty();
    TimelineComponent->TrackedBools.Empty();
    TimelineComponent->TrackedInts.Empty();
    TimelineComponent->TrackedFloats.Empty();
    TimelineComponent->TrackedDoubles.Empty();
    TimelineComponent->TrackedVectors.Empty();
    TimelineComponent->TrackedRotators.Empty();

    return FReply::Handled();
}

FReply UTimelinePluginWidget::UpdateAvailableVariables()
{
    TimelineComponent->GetVariablesFromParentBlueprint();

    return FReply::Handled();
}





// Create Duration Typing Field
void UTimelinePluginWidget::AddTimelineDurationTextField(IDetailCategoryBuilder& Category)
{
    FSlateFontInfo SmallFont = FCoreStyle::GetDefaultFontStyle("Regular", 8);

    // Cr�er le widget et l'assigner � la variable
    TimelineDurationTextBox = SNew(SEditableTextBox)
        .Text(this, &UTimelinePluginWidget::GetTimelineDuration)
        .Font(SmallFont)
        .OnTextCommitted(this, &UTimelinePluginWidget::OnTimelineDurationCommitted)
        .SelectAllTextWhenFocused(true)
        .MinDesiredWidth(200.0f);

    // Ajouter le widget � la cat�gorie
    Category.AddCustomRow(FText::FromString("Timeline Duration"))
        .NameContent()
        [
            SNew(STextBlock)
                .Text(FText::FromString("Timeline Duration")).Font(SmallFont)

        ]
        .ValueContent()
        [
            TimelineDurationTextBox.ToSharedRef()
        ];

    UE_LOG(LogTemp, Log, TEXT("TIMELINE PLUGIN : TimelineDuration TextBox successfully created!"));
}

FText UTimelinePluginWidget::GetTimelineDuration() const
{
    FText DurationText = FText::AsNumber(TimelineComponent->AnimationTimeline.Duration);
    FText AdditionalText = FText::FromString(" seconds");

    return FText::Format(FText::FromString("{0}{1}"), DurationText, AdditionalText);;
}

void UTimelinePluginWidget::OnTimelineDurationCommitted(const FText& NewText, ETextCommit::Type CommitType)
{
    if (CommitType == ETextCommit::OnEnter || CommitType == ETextCommit::OnUserMovedFocus)
    {
        if (!TimelineComponent->IsInitialized)
        {
            TimelineComponent->InitializeComponent();
        }

        FString TypingFieldContent = NewText.ToString();

        const std::string& str = TCHAR_TO_UTF8(*TypingFieldContent);
        std::istringstream iss(str);
        float value;
        iss >> value;

        if (!iss.fail() && iss.eof())
        {
            TimelineComponent->AnimationTimeline.Duration = value;
            TimelineComponent->AnimationTimelineDuration = value;
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("TIMELINE PLUGIN : TimelineDuration not set as valid Float!"));
        }

        TimelineDurationTextBox->SetText(GetTimelineDuration());
    }
}





// Create Timeline Widget
void UTimelinePluginWidget::AddTimelineWidget(IDetailCategoryBuilder& Category)
{
    AnimationTimelineWidget = SNew(SAnimationTimelineWidget);
    AnimationTimelineWidget->TimelinePluginWidget = this;
    AnimationTimelineWidget->AddTimeline();

    Category.AddCustomRow(FText::FromString("Animation Timeline"))
        .WholeRowContent()
        [
            AnimationTimelineWidget.ToSharedRef()
        ];
}