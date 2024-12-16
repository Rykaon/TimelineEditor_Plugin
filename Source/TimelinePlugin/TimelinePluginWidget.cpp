#include "TimelinePluginWidget.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "Components/ActorComponent.h"
#include "TimelinePluginComponent.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "TimelineBlueprintCustomization"

TSharedRef<IDetailCustomization> UTimelinePluginWidget::MakeInstance()
{
    return MakeShareable(new UTimelinePluginWidget());
}

void UTimelinePluginWidget::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
    // Exemple : Ajouter un bouton à une catégorie "Timeline"
    // Vérifie si l'Actor possède un composant de type TimelinePluginComponent
    TArray<TWeakObjectPtr<UObject>> CustomizedObjects;
    DetailBuilder.GetObjectsBeingCustomized(CustomizedObjects);

    for (TWeakObjectPtr<UObject> Object : CustomizedObjects)
    {
        if (AActor* Actor = Cast<AActor>(Object.Get()))
        {
            for (UActorComponent* Component : Actor->GetComponents())
            {
                if (Component->IsA(UTimelinePluginComponent::StaticClass()))
                {
                    // Ajouter une catégorie personnalisée
                    IDetailCategoryBuilder& Category = DetailBuilder.EditCategory("TimelineEditor");

                    // Exemple : Ajouter un bouton
                    Category.AddCustomRow(LOCTEXT("AddVariableRow", "Add Variable"))
                        .WholeRowWidget
                        [
                            SNew(SButton)
                                .Text(LOCTEXT("AddVariableButton", "Add Variable"))
                                .OnClicked_Lambda([]() {
                                // Action du bouton
                                return FReply::Handled();
                                    })
                        ];

                    break; // Pas besoin de continuer si le composant est trouvé
                }
            }
        }
    }
}