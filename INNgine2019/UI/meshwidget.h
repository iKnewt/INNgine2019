#ifndef MESHWIDGET_H
#define MESHWIDGET_H
#include "basewidget.h"

class ComboBoxProperty;
class QCheckBox;
class MeshManager;
class MaterialManager;


///The MeshWidget shows all information from the MeshComponent that the user can modify in runtime.
class MeshWidget : public BaseWidget
{
    Q_OBJECT
public:
    MeshWidget(QWidget *parent, Qt::WindowFlags f = Qt::WindowFlags());
    /**
     *Initializes the widget and creates all subwidgets needed.
     */
    void InitializeWidget() override;

    /**
     * Updates the values from the MeshComponent to the Widget.
     * @param mesh the Active Entity's MeshComponent.
     * @param materialManager The MaterialManager.
     * @param meshManager The MeshManager.
     */
    void UpdateMeshWidget(std::shared_ptr<MeshComponent> mesh);
    /**
     * Updates the values from the Widget to the MeshComponent.
     * @param mesh the Active Entity's MeshComponent.
     */
    void UpdateMeshComponent(std::shared_ptr<MeshComponent> mesh);
    /**
     * Updates the selectMesh and sleectMaterial ComboBoxProperties to add or remove any meshes that has been added/deleted in runtime.
     * @param mesh the Active Entity's MeshComponent.
     */
    void UpdateComboBoxValues(std::shared_ptr<MeshComponent> mesh);
    /**
     *Creates the GL Selection that the selectGLDRaw ComboBoxProperty uses. Only needs to be run once.
     */
    void CreateGLSelection();
    /**
     * Sets the GL type from the Widget to the MeshComponent.
     * @param mesh the Active Entity's MeshComponent.
     */
    void SetGLTypeInComponent(std::shared_ptr<MeshComponent> mesh);
    /**
     * Sets the GL type from the MeshComponent to the Widget.
     * @param mesh the Active Entity's MeshComponent.
     */
    void SetGLTypeInWidget(std::shared_ptr<MeshComponent> mesh);

    ///ComboBoxProperty containing all meshes loaded into the game.
    ComboBoxProperty* selectMesh_{nullptr};
    ///ComboBoxProperty containing all materials loaded into the game.
    ComboBoxProperty* selectMaterial_{nullptr};
    ///ComboBoxProperty containing all GL Draw options available to change.
    ComboBoxProperty* selectGLDraw_{nullptr};
    ///
    ComboBoxProperty* selectObjectType{nullptr};
    ///Checkbox to specify the active MeshComponent's collision setting.
    QCheckBox* collisionEnabled_{nullptr};
};

#endif // MESHWIDGET_H
