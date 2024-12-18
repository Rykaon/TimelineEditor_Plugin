#include "TimelinePluginComponent.h"
#include "TimelinePluginWidget.h"
#include "GameFramework/Actor.h"
#include "UObject/Class.h"
#include "UObject/UnrealType.h"

// Sets default values for this component's properties
UTimelinePluginComponent::UTimelinePluginComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTimelinePluginComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (!IsInitialized)
    {
        // Get OwnerActor Reference
        OwnerActor = GetOwner();

        if (OwnerActor != nullptr)  // Vérifie si l'acteur est valide avant d'accéder à ses propriétés
        {
            OwnerActorClass = OwnerActor->GetClass();
            UE_LOG(LogTemp, Log, TEXT("TIMELINE PLUGIN : OwnerActor successfully found!"));
        }
        else
        {
            // Log ou gestion d'erreur si OwnerActor est nul
            UE_LOG(LogTemp, Warning, TEXT("TIMELINE PLUGIN : OwnerActor is null in UTimelinePluginComponent constructor"));
        }

        // Set Available Types
        TArray<FString> TypesToAdd = { "Boolean", "Integer", "Float", "Double", "Vector", "Rotator"};
        AvailableTypes.Append(TypesToAdd);

        // Set Available Variables of Available Types
        GetVariablesFromParentBlueprint();

        IsInitialized = true;
    }
}


// Called when the game starts
void UTimelinePluginComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UTimelinePluginComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UTimelinePluginComponent::GetVariablesFromParentBlueprint()
{
    AvailableBools.Empty();
    AvailableInts.Empty();
    AvailableFloats.Empty();
    AvailableDoubles.Empty();
    AvailableVectors.Empty();
    AvailableRotators.Empty();

    if (OwnerActor)
    {
        // Parcourir toutes les propriétés de la classe de l'acteur
        for (TFieldIterator<FProperty> It(OwnerActorClass, EFieldIteratorFlags::IncludeSuper); It; ++It)
        {
            FProperty* Property = *It;

            // Vérifier si la propriété est publique et de type voulu
            if (Property)
            {
                FString PropertyType = Property->GetCPPType();

                // Filtrage en fonction du type demandé et ajout du nom à la liste
                if (PropertyType == TEXT("bool"))
                {
                    AvailableBools.Add(Property->GetName());
                }
                else if (PropertyType == TEXT("int32"))
                {
                    AvailableInts.Add(Property->GetName());
                }
                else if (PropertyType == TEXT("float"))
                {
                    AvailableFloats.Add(Property->GetName());
                }
                else if (PropertyType == TEXT("double"))
                {
                    AvailableDoubles.Add(Property->GetName());
                }
                else if (PropertyType == TEXT("FVector"))
                {
                    AvailableVectors.Add(Property->GetName());
                }
                else if (PropertyType == TEXT("FRotator"))
                {
                    AvailableRotators.Add(Property->GetName());
                }
            }
        }
    }
}

bool UTimelinePluginComponent::IsVariableTracked(const FString& VariableName)
{
    // Vérifier si la variable est déjà trackée dans les variables trackées
    return TrackedVariables.Contains(VariableName);
}

void UTimelinePluginComponent::PopulateAvailableVariables()
{
    // Vider la liste des variables disponibles
    AvailableVariables.Empty();

    // Récupérer les variables non trackées en fonction du type sélectionné
    if (SelectedType == "Boolean")
    {
        for (int32 i = 0; i < AvailableBools.Num(); ++i)
        {
            if (!IsVariableTracked(AvailableBools[i]))
            {
                AvailableVariables.Add(AvailableBools[i]);
            }
        }
    }
    else if (SelectedType == "Integer")
    {
        for (int32 i = 0; i < AvailableInts.Num(); ++i)
        {
            if (!IsVariableTracked(AvailableInts[i]))
            {
                AvailableVariables.Add(AvailableInts[i]);
            }
        }
    }
    else if (SelectedType == "Float")
    {
        for (int32 i = 0; i < AvailableFloats.Num(); ++i)
        {
            if (!IsVariableTracked(AvailableFloats[i]))
            {
                AvailableVariables.Add(AvailableFloats[i]);
            }
        }
    }
    else if (SelectedType == "Double")
    {
        for (int32 i = 0; i < AvailableDoubles.Num(); ++i)
        {
            if (!IsVariableTracked(AvailableDoubles[i]))
            {
                AvailableVariables.Add(AvailableDoubles[i]);
            }
        }
    }
    else if (SelectedType == "Vector")
    {
        for (int32 i = 0; i < AvailableVectors.Num(); ++i)
        {
            if (!IsVariableTracked(AvailableVectors[i]))
            {
                AvailableVariables.Add(AvailableVectors[i]);
            }
        }
    }
    else if (SelectedType == "Rotator")
    {
        for (int32 i = 0; i < AvailableRotators.Num(); ++i)
        {
            if (!IsVariableTracked(AvailableRotators[i]))
            {
                AvailableVariables.Add(AvailableRotators[i]);
            }
        }
    }
}

void UTimelinePluginComponent::OnTypeSelected(TSharedPtr<FString> NewSelection)
{
    // Lorsque le type est sélectionné, on met à jour la liste des variables possibles
    SelectedType = *NewSelection;
    PopulateAvailableVariables();
}

void UTimelinePluginComponent::OnVariableSelected(TSharedPtr<FString> NewSelection)
{
    // Vérifiez si la variable est déjà suivie
    if (IsVariableTracked(*NewSelection))
    {
        return; // Si déjà suivie, on ne fait rien
    }
    else
    {
        TrackedVariables.Add(*NewSelection, SelectedType);
    }

    FProperty* Property = OwnerActorClass->FindPropertyByName(FName(*NewSelection));

    // Ajoutez la variable à la bonne TMap selon le type sélectionné
    if (Property)
    {
        FString PropertyType = Property->GetCPPType();
        FStructProperty* StructProperty = (FStructProperty*)OwnerActorClass->FindPropertyByName(FName(*NewSelection));

        // Filtrage en fonction du type demandé et ajout du nom à la liste
        if (PropertyType == TEXT("bool"))
        {
            bool BoolValue = CastField<FBoolProperty>(Property)->GetPropertyValue_InContainer(OwnerActor);
            TrackedBools.Add(*NewSelection, BoolValue);
        }
        else if (PropertyType == TEXT("int32"))
        {
            int32 IntValue = CastField<FIntProperty>(Property)->GetPropertyValue_InContainer(OwnerActor);
            TrackedInts.Add(*NewSelection, IntValue);
        }
        else if (PropertyType == TEXT("float"))
        {
            float FloatValue = CastField<FFloatProperty>(Property)->GetPropertyValue_InContainer(OwnerActor);
            TrackedFloats.Add(*NewSelection, FloatValue);
        }
        else if (PropertyType == TEXT("double"))
        {
            double DoubleValue = CastField<FDoubleProperty>(Property)->GetPropertyValue_InContainer(OwnerActor);
            TrackedDoubles.Add(Property->GetName());
        }
        else if (PropertyType == TEXT("FVector"))
        {
            FVector* VectorValue = StructProperty->ContainerPtrToValuePtr<FVector>(OwnerActor);
            TrackedVectors.Add(*NewSelection, *VectorValue);
        }
        else if (PropertyType == TEXT("FRotator"))
        {
            FRotator* RotatorValue = StructProperty->ContainerPtrToValuePtr<FRotator>(OwnerActor);
            TrackedRotators.Add(*NewSelection, *RotatorValue);
        }
    }
}