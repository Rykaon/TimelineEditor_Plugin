#include "TimelinePlugin.h"
#include "TimelinePluginComponent.h"
#include "TimelinePluginWidget.h"
#include "PropertyEditorModule.h"

IMPLEMENT_MODULE(FTimelinePluginModule, MyEditorPlugin)

void FTimelinePluginModule::StartupModule()
{
    UE_LOG(LogTemp, Log, TEXT("TIMELINE PLUGIN : TimelinePlugin has started!"));

    FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

    // Lier la personnalisation de détails à la classe du composant
    PropertyEditorModule.RegisterCustomClassLayout(
        UTimelinePluginComponent::StaticClass()->GetFName(),
        FOnGetDetailCustomizationInstance::CreateStatic(&UTimelinePluginWidget::MakeInstance)
    );
}

void FTimelinePluginModule::ShutdownModule()
{
    UE_LOG(LogTemp, Log, TEXT("TIMELINE PLUGIN : TimelinePlugin has shutdown!"));

    // Unregister la personnalisation à la fermeture
    if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
    {
        FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
        PropertyEditorModule.UnregisterCustomClassLayout(UTimelinePluginComponent::StaticClass()->GetFName());
    }
}