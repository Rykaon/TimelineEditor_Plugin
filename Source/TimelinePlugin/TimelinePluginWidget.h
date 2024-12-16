#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"
#include "PropertyHandle.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Views/SListView.h"
#include "TimelinePluginComponent.h"

class TIMELINEPLUGIN_API UTimelinePluginWidget : public IDetailCustomization
{
public:
    // Créé une instance de cette classe pour la personnalisation
    static TSharedRef<IDetailCustomization> MakeInstance();

    // Implémentation de la personnalisation
    virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

    void OnTypeSelected(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo);
    void OnVariableSelected(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo);

public:
    // Reference to TimelineComponent
    UPROPERTY()
    UTimelinePluginComponent* TimelineComponent;

    UPROPERTY(meta = (BindWidget))
    class UComboBoxString* TypeDropdown;

    UPROPERTY(meta = (BindWidget))
    class UComboBoxString* VariableDropdown;

    TArray<TSharedPtr<FString>> TypeOptions;
    TArray<TSharedPtr<FString>> VariableOptions;

    UPROPERTY(meta = (BindWidget))
    TSharedPtr<FString> SelectedType;

    UPROPERTY(meta = (BindWidget))
    TSharedPtr<FString> SelectedVariable;
};
