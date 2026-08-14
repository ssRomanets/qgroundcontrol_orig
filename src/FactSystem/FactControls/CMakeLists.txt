qt_add_library(FactControlsModule STATIC)

qt_add_qml_module(FactControlsModule
    URI QUAV.FactControls
    VERSION 1.0
    RESOURCE_PREFIX /qml
    QML_FILES
        FactTextField.qml
        FactCheckBoxSlider.qml
        FactComboBox.qml
        LabelledFactComboBox.qml
        LabelledFactTextField.qml
    NO_PLUGIN
)
