#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"
#include "PropertyHandle.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Views/SListView.h"
#include "TimelinePluginComponent.h"
#include "AnimationTimelineWidget.h"
#include "DetailCategoryBuilder.h"

class TIMELINEPLUGIN_API UTimelinePluginWidget : public IDetailCustomization
{
public:
    // Cr�� une instance de cette classe pour la personnalisation
    static TSharedRef<IDetailCustomization> MakeInstance();

    // Impl�mentation de la personnalisation
    virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
    void InitializeComponent();

    void AddDropDowns(IDetailCategoryBuilder& Category);
    void OnTypeSelected(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo);
    void OnVariableSelected(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo);
    
    void AddResetAndUpdateButton(IDetailCategoryBuilder& Category);
    FReply ResetTrackedVariables();
    FReply UpdateAvailableVariables();

    void AddTimelineDurationTextField(IDetailCategoryBuilder& Category);
    FText GetTimelineDuration() const;
    void OnTimelineDurationCommitted(const FText& NewText, ETextCommit::Type CommitType);

    void AddTimelineWidget(IDetailCategoryBuilder& Category);




public:
    // Reference to TimelineComponent
    UPROPERTY()
    class UTimelinePluginComponent* TimelineComponent;

    TSharedPtr<SAnimationTimelineWidget> AnimationTimelineWidget;

    UPROPERTY(meta = (BindWidget))
    TSharedPtr<SComboBox<TSharedPtr<FString>>> TypeDropdown;

    UPROPERTY(meta = (BindWidget))
    TSharedPtr<SComboBox<TSharedPtr<FString>>> VariableDropdown;

    UPROPERTY(meta = (BindWidget))
    TSharedPtr<SEditableTextBox> TimelineDurationTextBox;

    TArray<TSharedPtr<FString>> TypeOptions;
    TArray<TSharedPtr<FString>> VariableOptions;

    UPROPERTY(meta = (BindWidget))
    TSharedPtr<FString> SelectedType;

    UPROPERTY(meta = (BindWidget))
    TSharedPtr<FString> SelectedVariable;
};
