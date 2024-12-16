#include "TimelinePluginComponent.h"
#include "TimelinePluginWidget.h"
#include "GameFramework/Actor.h"
#include "UObject/Class.h"
#include "UObject/UnrealType.h"

// Sets default values for this component's properties
UTimelinePluginComponent::UTimelinePluginComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

    // Get OwnerActor Reference
	OwnerActor = GetOwner();
    OwnerActorClass = OwnerActor->GetClass();

    // Set Available Types
	TArray<FString> TypesToAdd = { "Boolean", "Integer", "Float", "Vector", "Rotator" };
	AvailableTypes.Append(TypesToAdd);

    // Set Available Variables of Available Types
    GetVariablesFromParentBlueprint();

    // Set Default Animation Duration
	AnimationDuration = 5.0f;
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
    if (OwnerActor)
    {
        // Parcourir toutes les propri�t�s de la classe de l'acteur
        for (TFieldIterator<FProperty> It(OwnerActorClass); It; ++It)
        {
            FProperty* Property = *It;

            // V�rifier si la propri�t� est publique et de type voulu
            if (Property)
            {
                FString PropertyType = Property->GetCPPType();

                // Filtrage en fonction du type demand� et ajout du nom � la liste
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
    // V�rifier si la variable est d�j� track�e dans les variables track�es
    return TrackedVariables.Contains(VariableName);
}

void UTimelinePluginComponent::PopulateAvailableVariables()
{
    // Vider la liste des variables disponibles
    AvailableVariables.Empty();

    // R�cup�rer les variables non track�es en fonction du type s�lectionn�
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
    // Lorsque le type est s�lectionn�, on met � jour la liste des variables possibles
    SelectedType = *NewSelection;
    PopulateAvailableVariables();
}

void UTimelinePluginComponent::OnVariableSelected(TSharedPtr<FString> NewSelection)
{
    // V�rifiez si la variable est d�j� suivie
    if (IsVariableTracked(*NewSelection))
    {
        return; // Si d�j� suivie, on ne fait rien
    }

    // Lorsqu'une variable est s�lectionn�e, on l'ajoute � la liste des variables track�es
    if (IsVariableTracked(*NewSelection))
    {
        TrackedVariables.Add(*NewSelection);
    }

    FProperty* Property = OwnerActorClass->FindPropertyByName(FName(*NewSelection));

    // Ajoutez la variable � la bonne TMap selon le type s�lectionn�
    if (Property)
    {
        // R�cup�rer le type de la propri�t�
        if (FIntProperty* IntProperty = CastField<FIntProperty>(Property))
        {
            int32 IntValue = IntProperty->GetPropertyValue_InContainer(OwnerActor);
            TrackedInts.Add(*NewSelection, IntValue);
        }
        else if (FFloatProperty* FloatProperty = CastField<FFloatProperty>(Property))
        {
            float FloatValue = FloatProperty->GetPropertyValue_InContainer(OwnerActor);
            TrackedFloats.Add(*NewSelection, FloatValue);
        }
        else if (FBoolProperty* BoolProperty = CastField<FBoolProperty>(Property))
        {
            bool BoolValue = BoolProperty->GetPropertyValue_InContainer(OwnerActor);
            TrackedBools.Add(*NewSelection, BoolValue);
        }
        else if (FStructProperty* StructProperty = CastField<FStructProperty>(Property))
        {
            UScriptStruct* PropertyStruct = StructProperty->Struct;

            if (PropertyStruct->IsA<FVector>())
            {
                FVector* VectorValue = StructProperty->ContainerPtrToValuePtr<FVector>(OwnerActor);
                TrackedVectors.Add(*NewSelection, *VectorValue);
            }
            else if (PropertyStruct->IsA<FRotator>())
            {
                FRotator* RotatorValue = StructProperty->ContainerPtrToValuePtr<FRotator>(OwnerActor);
                TrackedRotators.Add(*NewSelection, *RotatorValue);
            }
        }
    }
}