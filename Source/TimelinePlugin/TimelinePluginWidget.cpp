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

TSharedRef<IDetailCustomization> UTimelinePluginWidget::MakeInstance()
{
    return MakeShareable(new UTimelinePluginWidget());
}

void UTimelinePluginWidget::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
    TArray<UObject*> SelectedObjects;
    GEditor->GetSelectedActors()->GetSelectedObjects(SelectedObjects);  // GEditor accède aux objets sélectionnés dans l'éditeur

    if (SelectedObjects.Num() > 0)
    {
        AActor* Actor = Cast<AActor>(SelectedObjects[0]);
        if (Actor)
        {
            // Trouver le composant TimelinePluginComponent attaché à cet acteur
            TimelineComponent = Actor->FindComponentByClass<UTimelinePluginComponent>();

            if (!TimelineComponent)
            {
                UE_LOG(LogTemp, Log, TEXT("TimelineComponent Not Found!"));
            }
        }
    }

    IDetailCategoryBuilder& TimelineVariableCategory = DetailBuilder.EditCategory("Timeline Variables");

    // Peupler les options pour le type
    TypeOptions.Empty();
    for (const FString& Type : TimelineComponent->AvailableTypes)
    {
        TypeOptions.Add(MakeShared<FString>(Type));
    }

    // Peupler les options pour les variables trackables
    VariableOptions.Empty();
    for (const FString& Variable : TimelineComponent->AvailableVariables)
    {
        VariableOptions.Add(MakeShared<FString>(Variable));
    }

    // Ajouter les dropdowns dans la catégorie
    TimelineVariableCategory.AddCustomRow(FText::FromString("Type and Variable"))
        .ValueContent()
        [
            SNew(SHorizontalBox)

                // Dropdown pour le type
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(SComboBox<TSharedPtr<FString>>)
                        .OptionsSource(&TypeOptions)
                        .OnSelectionChanged(this, &UTimelinePluginWidget::OnTypeSelected)
                        .OnGenerateWidget_Lambda([](TSharedPtr<FString> Item)
                            {
                                return SNew(STextBlock).Text(FText::FromString(*Item));
                            })
                        .Content()
                        [
                            SNew(STextBlock)
                                .Text_Lambda([this]()
                                    {
                                        return SelectedType.IsValid() ? FText::FromString(*SelectedType) : FText::FromString("Choose Type");
                                    })
                        ]
                ]

                // Espacement entre les deux combo boxes
                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(10.0f, 0.0f)

                // Dropdown pour la variable
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(SComboBox<TSharedPtr<FString>>)
                        .OptionsSource(&VariableOptions)
                        .OnSelectionChanged(this, &UTimelinePluginWidget::OnVariableSelected)
                        .OnGenerateWidget_Lambda([](TSharedPtr<FString> Item)
                            {
                                return SNew(STextBlock).Text(FText::FromString(*Item));
                            })
                        .Content()
                        [
                            SNew(STextBlock)
                                .Text_Lambda([this]()
                                    {
                                        return SelectedVariable.IsValid() ? FText::FromString(*SelectedVariable) : FText::FromString("Choose Variable");
                                    })
                        ]
                ]
        ];
}

void UTimelinePluginWidget::OnTypeSelected(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo)
{
    if (NewSelection.IsValid())
    {
        UE_LOG(LogTemp, Log, TEXT("Type selected: %s"), **NewSelection);

        // Logique de mise à jour pour les types disponibles
        TimelineComponent->OnTypeSelected(NewSelection);

        // Repeupler les options
        VariableOptions.Empty();
        for (const FString& Variable : TimelineComponent->AvailableVariables)
        {
            VariableOptions.Add(MakeShared<FString>(Variable));
        }
    }
}

void UTimelinePluginWidget::OnVariableSelected(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo)
{
    if (NewSelection.IsValid())
    {
        UE_LOG(LogTemp, Log, TEXT("Variable selected: %s"), **NewSelection);

        // Ajout de la variable sélectionnée à la liste trackée
        TimelineComponent->OnVariableSelected(NewSelection);
    }
}