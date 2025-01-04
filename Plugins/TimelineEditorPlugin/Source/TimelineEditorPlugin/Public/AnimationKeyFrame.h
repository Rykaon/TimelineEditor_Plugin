#pragma once

#include "CoreMinimal.h"
#include "AnimationKeyFrame.generated.h"

UENUM(BlueprintType)
enum class EInterpolationType : uint8
{
    Linear UMETA(DisplayName = "Linear"),
    QuadraticIn UMETA(DisplayName = "Quadratic In"),
    QuadraticOut UMETA(DisplayName = "Quadratic Out"),
    QuadraticInOut UMETA(DisplayName = "Quadratic In-Out"),
    CubicIn UMETA(DisplayName = "Cubic In"),
    CubicOut UMETA(DisplayName = "Cubic Out"),
    CubicInOut UMETA(DisplayName = "Cubic In-Out"),
    ExponentialIn UMETA(DisplayName = "Exponential In"),
    ExponentialOut UMETA(DisplayName = "Exponential Out"),
    ExponentialInOut UMETA(DisplayName = "Exponential In-Out"),
    SineIn UMETA(DisplayName = "Sine In"),
    SineOut UMETA(DisplayName = "Sine Out"),
    SineInOut UMETA(DisplayName = "Sine In-Out")
};

USTRUCT(BlueprintType)
struct  FAnimationKeyFrame
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 ParentTrackID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 KeyFrameID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool IsOpen = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Time;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool BoolValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 IntValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double DoubleValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FloatValueX;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FloatValueY;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FloatValueZ;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EInterpolationType InterpolationType = EInterpolationType::Linear;
};