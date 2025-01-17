find_package(Qt6 REQUIRED COMPONENTS LinguistTools)
qt_standard_project_setup(I18N_TRANSLATED_LANGUAGES fr de es)
qt_add_translations(MinecraftFishingBot)

target_sources(MinecraftFishingBot PRIVATE icon.rc)
qt_add_resources(MinecraftFishingBot imageresources
    PREFIX "/images"
    FILES icon.png
)