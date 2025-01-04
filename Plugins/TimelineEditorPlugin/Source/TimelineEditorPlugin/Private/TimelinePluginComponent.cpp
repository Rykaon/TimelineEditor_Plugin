#include "TimelinePluginComponent.h"
#include "TimelinePluginWidget.h"
#include "GameFramework/Actor.h"
#include "UObject/Class.h"
#include "UObject/UnrealType.h"
#include "Math/UnrealMathUtility.h"
#include <optional>

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
    OwnerActor = GetOwner();

    if (OwnerActor != nullptr)
    {
        OwnerActorClass = OwnerActor->GetClass();
        UE_LOG(LogTemp, Log, TEXT("TIMELINE PLUGIN : OwnerActor successfully found!"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("TIMELINE PLUGIN : OwnerActor is null in UTimelinePluginComponent constructor"));
    }

    TArray<FString> TypesToAdd = { "Boolean", "Integer", "Float", "Double", "Vector", "Rotator" };
    AvailableTypes.Append(TypesToAdd);

    GetVariablesFromParentBlueprint();

    AnimationTimeline.Duration = 5.0f;

    IsInitialized = true;
}

void UTimelinePluginComponent::BeginPlay()
{
	Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("TIMELINE PLUGIN : BEGIN PLAY"));
    CurrentTime = 0.0f;

    for (int32 i = 0; i < AnimationTimeline.Tracks.Num(); ++i)
    {
        ApplyTrackDefaultValueToVariable(i);
    }
}

void UTimelinePluginComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    UE_LOG(LogTemp, Warning, TEXT("TIMELINE PLUGIN : EVENT TICK // CurrentTime : %f"), CurrentTime);
	CurrentTime += DeltaTime;

    if (IsAnimationLooping)
    {
        if (CurrentTime > AnimationTimeline.Duration)
        {
            CurrentTime = 0.0f;
        }
    }

    if (CurrentTime >= 0 || CurrentTime <= AnimationTimeline.Duration)
    {
        for (int32 i = 0; i < AnimationTimeline.Tracks.Num(); ++i)
        {
            ApplyTrackValueAtTimeToVariable(i, CurrentTime);
        }
    }
}

bool UTimelinePluginComponent::IsAnimationTimelineWidgetValid()
{
    if (!TimelinePluginWidget)
    {
        return false;
    }

    if (!TimelinePluginWidget->AnimationTimelineWidget)
    {
        return false;
    }

    return true;
}

TArray<FName> UTimelinePluginComponent::GetAvailableFunctions()
{
    TArray<FName> FunctionNames;

    if (OwnerActor)
    {
        for (TFieldIterator<UFunction> FuncIt(OwnerActorClass, EFieldIteratorFlags::IncludeSuper); FuncIt; ++FuncIt)
        {
            UFunction* Function = *FuncIt;

            if (Function->HasAnyFunctionFlags(FUNC_BlueprintCallable) && Function->NumParms == 0)
            {
                FunctionNames.Add(Function->GetFName());
            }
        }
    }

    return FunctionNames;
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
        for (TFieldIterator<FProperty> It(OwnerActorClass, EFieldIteratorFlags::IncludeSuper); It; ++It)
        {
            FProperty* Property = *It;

            if (Property)
            {
                FString PropertyType = Property->GetCPPType();

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
    return TrackedVariables.Contains(VariableName);
}

void UTimelinePluginComponent::PopulateAvailableVariables()
{
    AvailableVariables.Empty();

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
    SelectedType = *NewSelection;
    PopulateAvailableVariables();
}

void UTimelinePluginComponent::OnVariableSelected(TSharedPtr<FString> NewSelection)
{
    if (IsVariableTracked(*NewSelection))
    {
        return;
    }
    else
    {
        TrackedVariables.Add(*NewSelection, SelectedType);
    }

    FProperty* Property = OwnerActorClass->FindPropertyByName(FName(*NewSelection));
    FAnimationTrack Track;
    Track.VariableName = FName(*NewSelection);
    Track.VariableType = SelectedType;

    if (Property)
    {
        FString PropertyType = Property->GetCPPType();
        FStructProperty* StructProperty = (FStructProperty*)OwnerActorClass->FindPropertyByName(FName(*NewSelection));

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

void UTimelinePluginComponent::ApplyTrackDefaultValueToVariable(int32 TrackIndex)
{
    if (!OwnerActor)
    {
        return;
    }

    if (TrackIndex < 0 || TrackIndex >= AnimationTimeline.Tracks.Num())
    {
        return;
    }

    FAnimationTrack Track = AnimationTimeline.Tracks[TrackIndex];
    
    FProperty* Property = OwnerActorClass->FindPropertyByName(Track.VariableName);
    if (!Property)
    {
        return;
    }

    FString PropertyType = Property->GetCPPType();
    FStructProperty* StructProperty = CastField<FStructProperty>(Property);

    if (PropertyType == TEXT("bool"))
    {
        bool CurrentValue = CastField<FBoolProperty>(Property)->GetPropertyValue_InContainer(OwnerActor);
        if (CurrentValue != Track.DefaultBoolValue)
        {
            CastField<FBoolProperty>(Property)->SetPropertyValue_InContainer(OwnerActor, Track.DefaultBoolValue);
        }
    }
    else if (PropertyType == TEXT("int32"))
    {
        int32 CurrentValue = CastField<FIntProperty>(Property)->GetPropertyValue_InContainer(OwnerActor);
        if (CurrentValue != Track.DefaultIntValue)
        {
            CastField<FIntProperty>(Property)->SetPropertyValue_InContainer(OwnerActor, Track.DefaultIntValue);
        }
    }
    else if (PropertyType == TEXT("float"))
    {
        float CurrentValue = CastField<FFloatProperty>(Property)->GetPropertyValue_InContainer(OwnerActor);
        if (!FMath::IsNearlyEqual(CurrentValue, Track.DefaultFloatValueX))
        {
            CastField<FFloatProperty>(Property)->SetPropertyValue_InContainer(OwnerActor, Track.DefaultFloatValueX);
        }
    }
    else if (PropertyType == TEXT("double"))
    {
        double CurrentValue = CastField<FDoubleProperty>(Property)->GetPropertyValue_InContainer(OwnerActor);
        if (!FMath::IsNearlyEqual(CurrentValue, Track.DefaultDoubleValue))
        {
            CastField<FDoubleProperty>(Property)->SetPropertyValue_InContainer(OwnerActor, Track.DefaultDoubleValue);
        }
    }
    else if (PropertyType == TEXT("FVector") && StructProperty)
    {
        FVector* CurrentValue = StructProperty->ContainerPtrToValuePtr<FVector>(OwnerActor);
        if (CurrentValue && (*CurrentValue != FVector(Track.DefaultFloatValueX, Track.DefaultFloatValueY, Track.DefaultFloatValueZ)))
        {
            CurrentValue->X = Track.DefaultFloatValueX;
            CurrentValue->Y = Track.DefaultFloatValueY;
            CurrentValue->Z = Track.DefaultFloatValueZ;
        }
    }
    else if (PropertyType == TEXT("FRotator") && StructProperty)
    {
        FRotator* CurrentValue = StructProperty->ContainerPtrToValuePtr<FRotator>(OwnerActor);
        if (CurrentValue && (*CurrentValue != FRotator(Track.DefaultFloatValueX, Track.DefaultFloatValueY, Track.DefaultFloatValueZ)))
        {
            CurrentValue->Pitch = Track.DefaultFloatValueX;
            CurrentValue->Yaw = Track.DefaultFloatValueY;
            CurrentValue->Roll = Track.DefaultFloatValueZ;
        }
    }
}

void UTimelinePluginComponent::ApplyTrackValueAtTimeToVariable(int32 TrackIndex, float Time)
{
    if (!OwnerActor)
    {
        return;
    }

    if (TrackIndex < 0 || TrackIndex >= AnimationTimeline.Tracks.Num())
    {
        return;
    }

    FAnimationTrack Track = AnimationTimeline.Tracks[TrackIndex];

    UE_LOG(LogTemp, Warning, TEXT("TIMELINE PLUGIN : Fais un truc frère a %d et %f"), TrackIndex, Time);
    
    FProperty* Property = OwnerActorClass->FindPropertyByName(Track.VariableName);
    if (!Property)
    {
        return;
    }

    FString PropertyType = Property->GetCPPType();
    FStructProperty* StructProperty = CastField<FStructProperty>(Property);
    UE_LOG(LogTemp, Warning, TEXT("Set Track of type %s"), *PropertyType);

    if (PropertyType == TEXT("bool"))
    {
        bool CurrentValue = CastField<FBoolProperty>(Property)->GetPropertyValue_InContainer(OwnerActor);
        std::optional<bool> DesiredValue = GetTrackBoolValueAtTime(Track.TrackID, Time);

        if (DesiredValue.has_value())
        {
            if (CurrentValue != DesiredValue.value())
            {
                CastField<FBoolProperty>(Property)->SetPropertyValue_InContainer(OwnerActor, DesiredValue.value());
            }
        }
        else
        {
            return;
        }
    }
    else if (PropertyType == TEXT("int32"))
    {
        int32 CurrentValue = CastField<FIntProperty>(Property)->GetPropertyValue_InContainer(OwnerActor);
        std::optional<int32> DesiredValue = GetTrackIntValueAtTime(Track.TrackID, Time);

        if (DesiredValue.has_value())
        {
            if (CurrentValue != DesiredValue.value())
            {
                CastField<FIntProperty>(Property)->SetPropertyValue_InContainer(OwnerActor, DesiredValue.value());
            }
        }
        else
        {
            return;
        }
    }
    else if (PropertyType == TEXT("float"))
    {
        float CurrentValue = CastField<FFloatProperty>(Property)->GetPropertyValue_InContainer(OwnerActor);
        float DesiredValue = GetTrackFloatValueAtTime(Track.TrackID, Time, 0);

        if (!std::isnan(DesiredValue))
        {
            if (!FMath::IsNearlyEqual(CurrentValue, DesiredValue))
            {
                CastField<FFloatProperty>(Property)->SetPropertyValue_InContainer(OwnerActor, DesiredValue);
            }
        }
        else
        {
            return;
        }
    }
    else if (PropertyType == TEXT("double"))
    {
        double CurrentValue = CastField<FDoubleProperty>(Property)->GetPropertyValue_InContainer(OwnerActor);
        double DesiredValue = GetTrackDoubleValueAtTime(Track.TrackID, Time);

        if (!std::isnan(DesiredValue))
        {
            if (!FMath::IsNearlyEqual(CurrentValue, DesiredValue))
            {
                CastField<FDoubleProperty>(Property)->SetPropertyValue_InContainer(OwnerActor, DesiredValue);
            }
        }
        else
        {
            return;
        }
    }
    else if (PropertyType == TEXT("FVector") && StructProperty)
    {
        FVector* CurrentValue = StructProperty->ContainerPtrToValuePtr<FVector>(OwnerActor);
        float DesiredValueX = GetTrackFloatValueAtTime(Track.TrackID, Time, 0);
        float DesiredValueY = GetTrackFloatValueAtTime(Track.TrackID, Time, 1);
        float DesiredValueZ = GetTrackFloatValueAtTime(Track.TrackID, Time, 2);

        if (!std::isnan(DesiredValueX) && !std::isnan(DesiredValueY) && !std::isnan(DesiredValueZ))
        {
            if (CurrentValue && (*CurrentValue != FVector(DesiredValueX, DesiredValueY, DesiredValueZ)))
            {
                CurrentValue->X = DesiredValueX;
                CurrentValue->Y = DesiredValueY;
                CurrentValue->Z = DesiredValueZ;
            }
        }
        else
        {
            return;
        }
    }
    else if (PropertyType == TEXT("FRotator") && StructProperty)
    {
        UE_LOG(LogTemp, Warning, TEXT("Set Track Rotator"));
        FRotator* CurrentValue = StructProperty->ContainerPtrToValuePtr<FRotator>(OwnerActor);
        UE_LOG(LogTemp, Warning, TEXT("Current Value: X=%f, Y=%f, Z=%f"), CurrentValue->Pitch, CurrentValue->Yaw, CurrentValue->Roll);
        float DesiredValueX = GetTrackFloatValueAtTime(Track.TrackID, Time, 0);
        float DesiredValueY = GetTrackFloatValueAtTime(Track.TrackID, Time, 1);
        float DesiredValueZ = GetTrackFloatValueAtTime(Track.TrackID, Time, 2);

        if (!std::isnan(DesiredValueX) && !std::isnan(DesiredValueY) && !std::isnan(DesiredValueZ))
        {
            UE_LOG(LogTemp, Warning, TEXT("Set Track Valid Rotator"));
            if (CurrentValue && (*CurrentValue != FRotator(DesiredValueX, DesiredValueY, DesiredValueZ)))
            {
                CurrentValue->Pitch = DesiredValueX;
                CurrentValue->Yaw = DesiredValueY;
                CurrentValue->Roll = DesiredValueZ;
                UE_LOG(LogTemp, Warning, TEXT("Updated Value: X=%f, Y=%f, Z=%f"), CurrentValue->Pitch, CurrentValue->Yaw, CurrentValue->Roll);
            }
        }
        else
        {
            return;
        }
    }

    #if WITH_EDITOR
    if (GEditor)
    {
        CallSelectedFunction();
    }
    #endif
}

void UTimelinePluginComponent::CallSelectedFunction()
{
    if (!OwnerActor)
    {
        return;
    }

    if (!IsAnimationTimelineWidgetValid())
    {
        return;
    }

    UFunction* Function = OwnerActor->FindFunction(SelectedFunction);

    if (Function)
    {
        OwnerActor->ProcessEvent(Function, nullptr);
    }
}







FAnimationTrack* UTimelinePluginComponent::GetTrackOfID(int32 TrackID)
{
    FAnimationTrack* Track = nullptr;

    for (int32 i = 0; i < AnimationTimeline.Tracks.Num(); ++i)
    {
        if (AnimationTimeline.Tracks[i].TrackID == TrackID)
        {
            return &AnimationTimeline.Tracks[i];
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("No Track of Track ID %d found in AnimationTimeline"), TrackID);
    return nullptr;
}

std::optional<bool> UTimelinePluginComponent::GetTrackBoolValueAtTime(int32 TrackID, float Time)
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

std::optional<int32> UTimelinePluginComponent::GetTrackIntValueAtTime(int32 TrackID, float Time)
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

float UTimelinePluginComponent::GetTrackFloatValueAtTime(int32 TrackID, float Time, int32 Index)
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

double UTimelinePluginComponent::GetTrackDoubleValueAtTime(int32 TrackID, float Time)
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

float UTimelinePluginComponent::GetAlphaAtTime(float Start, float End, float Time)
{
    if (FMath::IsNearlyEqual(Start, End))
    {
        UE_LOG(LogTemp, Warning, TEXT("Start and End are nearly equal. Returning 0.0f."));
        return 0.0f;
    }

    float Alpha = (Time - Start) / (End - Start);

    return FMath::Clamp(Alpha, 0.0f, 1.0f);
}

float UTimelinePluginComponent::InterpolateFloat(float StartValue, float EndValue, float Alpha, EInterpolationType InterpolationType)
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

int32 UTimelinePluginComponent::InterpolateInteger(int32 StartValue, int32 EndValue, float Alpha, EInterpolationType InterpolationType)
{
    return FMath::RoundToInt(InterpolateFloat((float)StartValue, (float)EndValue, Alpha, InterpolationType));
}

double UTimelinePluginComponent::InterpolateDouble(double StartValue, double EndValue, float Alpha, EInterpolationType InterpolationType)
{
    return InterpolateFloat(StartValue, EndValue, Alpha, InterpolationType);
}

FVector UTimelinePluginComponent::InterpolateVector(FVector StartValue, FVector EndValue, float Alpha, EInterpolationType InterpolationType)
{
    FVector VectorValue;
    VectorValue.X = InterpolateFloat(StartValue.X, EndValue.X, Alpha, InterpolationType);
    VectorValue.Y = InterpolateFloat(StartValue.Y, EndValue.Y, Alpha, InterpolationType);
    VectorValue.Z = InterpolateFloat(StartValue.Z, EndValue.Z, Alpha, InterpolationType);
    return VectorValue;
}

FRotator UTimelinePluginComponent::InterpolateRotator(FRotator StartValue, FRotator EndValue, float Alpha, EInterpolationType InterpolationType)
{
    FRotator RotatorValue;
    RotatorValue.Pitch = InterpolateFloat(StartValue.Pitch, EndValue.Pitch, Alpha, InterpolationType);
    RotatorValue.Yaw = InterpolateFloat(StartValue.Yaw, EndValue.Yaw, Alpha, InterpolationType);
    RotatorValue.Roll = InterpolateFloat(StartValue.Roll, EndValue.Roll, Alpha, InterpolationType);
    return RotatorValue;
}