#include "TimelinePluginComponent.h"
#include "TimelinePluginWidget.h"
#include "GameFramework/Actor.h"
#include "UObject/Class.h"
#include "UObject/UnrealType.h"
#include "Math/UnrealMathUtility.h"

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
        InitializeComponent();
    }
}

void UTimelinePluginComponent::InitializeComponent()
{
    // Get OwnerActor Reference
    OwnerActor = GetOwner();

    if (OwnerActor != nullptr)  // V�rifie si l'acteur est valide avant d'acc�der � ses propri�t�s
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
    TArray<FString> TypesToAdd = { "Boolean", "Integer", "Float", "Double", "Vector", "Rotator" };
    AvailableTypes.Append(TypesToAdd);

    // Set Available Variables of Available Types
    GetVariablesFromParentBlueprint();

    AnimationTimeline.Duration = 5.0f;

    IsInitialized = true;
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
        // Parcourir toutes les propri�t�s de la classe de l'acteur
        for (TFieldIterator<FProperty> It(OwnerActorClass, EFieldIteratorFlags::IncludeSuper); It; ++It)
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
    else
    {
        TrackedVariables.Add(*NewSelection, SelectedType);
    }

    FProperty* Property = OwnerActorClass->FindPropertyByName(FName(*NewSelection));
    FAnimationTrack Track;
    Track.VariableName = FName(*NewSelection);
    Track.VariableType = SelectedType;

    // Ajoutez la variable � la bonne TMap selon le type s�lectionn�
    if (Property)
    {
        FString PropertyType = Property->GetCPPType();
        FStructProperty* StructProperty = (FStructProperty*)OwnerActorClass->FindPropertyByName(FName(*NewSelection));

        // Filtrage en fonction du type demand� et ajout du nom � la liste
        if (PropertyType == TEXT("bool"))
        {
            bool BoolValue = CastField<FBoolProperty>(Property)->GetPropertyValue_InContainer(OwnerActor);
            TrackedBools.Add(*NewSelection, BoolValue);
            Track.DefaultBoolValue = BoolValue;
        }
        else if (PropertyType == TEXT("int32"))
        {
            int32 IntValue = CastField<FIntProperty>(Property)->GetPropertyValue_InContainer(OwnerActor);
            TrackedInts.Add(*NewSelection, IntValue);
            Track.DefaultIntValue = IntValue;
        }
        else if (PropertyType == TEXT("float"))
        {
            float FloatValue = CastField<FFloatProperty>(Property)->GetPropertyValue_InContainer(OwnerActor);
            TrackedFloats.Add(*NewSelection, FloatValue);
            Track.DefaultFloatValueX = FloatValue;
        }
        else if (PropertyType == TEXT("double"))
        {
            double DoubleValue = CastField<FDoubleProperty>(Property)->GetPropertyValue_InContainer(OwnerActor);
            TrackedDoubles.Add(Property->GetName());
            Track.DefaultDoubleValue = DoubleValue;
        }
        else if (PropertyType == TEXT("FVector"))
        {
            FVector* VectorValue = StructProperty->ContainerPtrToValuePtr<FVector>(OwnerActor);
            TrackedVectors.Add(*NewSelection, *VectorValue);
            Track.DefaultFloatValueX = VectorValue->X;
            Track.DefaultFloatValueY = VectorValue->Y;
            Track.DefaultFloatValueZ = VectorValue->Z;
        }
        else if (PropertyType == TEXT("FRotator"))
        {
            FRotator* RotatorValue = StructProperty->ContainerPtrToValuePtr<FRotator>(OwnerActor);
            TrackedRotators.Add(*NewSelection, *RotatorValue);
            Track.DefaultFloatValueX = RotatorValue->Pitch;
            Track.DefaultFloatValueY = RotatorValue->Yaw;
            Track.DefaultFloatValueZ = RotatorValue->Roll;
        }

        Track.TrackID = GenerateUniqueRandomID();
        AnimationTimeline.Tracks.Add(Track);
        TimelinePluginWidget->AnimationTimelineWidget->AddTrack(Track);
    }
}

void UTimelinePluginComponent::RemoveTrack(int32 TrackID)
{
    int32 TrackIndex = -1;

    for (int32 i = 0; i < AnimationTimeline.Tracks.Num(); ++i)
    {
        if (AnimationTimeline.Tracks[i].TrackID == TrackID)
        {   
            TrackIndex = i;

            if (TrackedVariables.Contains(AnimationTimeline.Tracks[i].VariableName.ToString()))
            {
                TrackedVariables.Remove(AnimationTimeline.Tracks[i].VariableName.ToString());
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("TIMELINE PLUGIN : TrackedVariables doesn't contain Variable of name %d"), *AnimationTimeline.Tracks[i].VariableName.ToString());
            }
            
            if (AnimationTimeline.Tracks[i].VariableType == "Boolean")
            {
                if (TrackedBools.Contains(AnimationTimeline.Tracks[i].VariableName.ToString()))
                {
                    TrackedBools.Remove(AnimationTimeline.Tracks[i].VariableName.ToString());
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("TIMELINE PLUGIN : TrackedBools doesn't contain Variable of name %d"), *AnimationTimeline.Tracks[i].VariableName.ToString());
                }
            }
            else if (AnimationTimeline.Tracks[i].VariableType == "Integer")
            {
                if (TrackedInts.Contains(AnimationTimeline.Tracks[i].VariableName.ToString()))
                {
                    TrackedInts.Remove(AnimationTimeline.Tracks[i].VariableName.ToString());
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("TIMELINE PLUGIN : TrackedInts doesn't contain Variable of name %d"), *AnimationTimeline.Tracks[i].VariableName.ToString());
                }
            }
            else if (AnimationTimeline.Tracks[i].VariableType == "Float")
            {
                if (TrackedFloats.Contains(AnimationTimeline.Tracks[i].VariableName.ToString()))
                {
                    TrackedFloats.Remove(AnimationTimeline.Tracks[i].VariableName.ToString());
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("TIMELINE PLUGIN : TrackedFloats doesn't contain Variable of name %d"), *AnimationTimeline.Tracks[i].VariableName.ToString());
                }
            }
            else if (AnimationTimeline.Tracks[i].VariableType == "Double")
            {
                if (TrackedDoubles.Contains(AnimationTimeline.Tracks[i].VariableName.ToString()))
                {
                    TrackedDoubles.Remove(AnimationTimeline.Tracks[i].VariableName.ToString());
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("TIMELINE PLUGIN : TrackedDoubles doesn't contain Variable of name %d"), *AnimationTimeline.Tracks[i].VariableName.ToString());
                }
            }
            else if (AnimationTimeline.Tracks[i].VariableType == "Vector")
            {
                if (TrackedVectors.Contains(AnimationTimeline.Tracks[i].VariableName.ToString()))
                {
                    TrackedVectors.Remove(AnimationTimeline.Tracks[i].VariableName.ToString());
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("TIMELINE PLUGIN : TrackedVectors doesn't contain Variable of name %d"), *AnimationTimeline.Tracks[i].VariableName.ToString());
                }
            }
            else if (AnimationTimeline.Tracks[i].VariableType == "Rotator")
            {
                if (TrackedRotators.Contains(AnimationTimeline.Tracks[i].VariableName.ToString()))
                {
                    TrackedRotators.Remove(AnimationTimeline.Tracks[i].VariableName.ToString());
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("TIMELINE PLUGIN : TrackedRotators doesn't contain Variable of name %d"), *AnimationTimeline.Tracks[i].VariableName.ToString());
                }
            }

            if (TrackedIDs.Contains(TrackID))
            {
                TrackedIDs.Remove(TrackID);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("TIMELINE PLUGIN : TrackedIDs doesn't contain %d"), TrackID);
            }
        }
    }

    if (TrackIndex >= 0)
    {
        AnimationTimeline.Tracks.RemoveAt(TrackIndex);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("TIMELINE PLUGIN : AnimationTimeline doesn't contain track of ID : %d"), TrackID);
    }
}

int32 UTimelinePluginComponent::GenerateUniqueRandomID()
{
    int32 NewID;
    do
    {
        NewID = FMath::RandRange(100000, 999999);
    } while (TrackedIDs.Contains(NewID));

    TrackedIDs.Add(NewID);
    return NewID;
}