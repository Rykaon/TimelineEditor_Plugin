#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "TimelinePlugin.h"
#include "TimelinePluginComponent.h"
#include "TimelinePluginWidget.h"

IMPLEMENT_MODULE(FTimelinePluginModule, MyEditorPlugin)

void FTimelinePluginModule::StartupModule()
{
    UE_LOG(LogTemp, Log, TEXT("TimelinePlugin has started!"));

    // Charger le module PropertyEditor
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

    // Enregistrer la personnalisation des Blueprints
    PropertyModule.RegisterCustomClassLayout(
        "Actor", // Tous les Blueprints Actor
        FOnGetDetailCustomizationInstance::CreateStatic(&UTimelinePluginWidget::MakeInstance)
    );

    PropertyModule.NotifyCustomizationModuleChanged();
}

void FTimelinePluginModule::ShutdownModule()
{
    if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
    {
        FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

        // Désenregistrer la personnalisation
        PropertyModule.UnregisterCustomClassLayout("Actor");
    }
}