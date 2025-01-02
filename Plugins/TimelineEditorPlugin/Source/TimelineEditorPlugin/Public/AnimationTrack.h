#pragma once

#include "CoreMinimal.h"
#include "AnimationKeyFrame.h"
#include "AnimationTrack.generated.h"

USTRUCT(BlueprintType)
struct  FAnimationTrack
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName VariableName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString VariableType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 TrackID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool DefaultBoolValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 DefaultIntValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double DefaultDoubleValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DefaultFloatValueX;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DefaultFloatValueY;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DefaultFloatValueZ;

    TArray<TSharedPtr<STextBlock>> NameBoxes;
    TArray<TSharedPtr<SEditableTextBox>> ValueBoxes;

    TArray<FAnimationKeyFrame> KeyFrames;
};