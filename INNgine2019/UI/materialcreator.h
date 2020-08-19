#ifndef MATERIALCREATOR_H
#define MATERIALCREATOR_H

#include <QDialog>
#include <QPixmap>

namespace Ui {
class MaterialCreator;
}
class LineBoxProperty;
class SpinBoxProperty;
class QLabel;
class QGraphicsPixmapItem;
class ComboBoxProperty;
///Creates and edits all Materials.
class MaterialCreator : public QDialog
{
    Q_OBJECT

public:
    /**
     * Constructor for the MaterialCreator.
     * @param edit Determines if editing or creating. if edit, = true.
     * @param id MaterialID for the specified material to change.
     * @param parent Parent of window.
     */
    explicit MaterialCreator(bool edit = false, size_t id = 0,QWidget *parent = nullptr);
    ~MaterialCreator() override;
    /**
     * Initializes the window, and creates all information needed.
     */
    void InitializeWindow();
    /**
     * Updates each respective combobox, ShaderSelection, TextureSelection and SpecualrMapSelection.
     */
    void UpdateComboBoxes();
public slots:
    void event_colorChanged();
    void event_createMaterial();

private:
    /**
     * Loads infromation from a material if editing =true.
     */
    void loadMaterialData();
    LineBoxProperty* name_{nullptr};
    SpinBoxProperty* color_{nullptr};
    SpinBoxProperty* shininess_{nullptr};
    ComboBoxProperty* shaderSelection_{nullptr};
    ComboBoxProperty* textureSelection_{nullptr};
    ComboBoxProperty* specularMapSelection_{nullptr};
    QImage image_;
    QLabel* colorShowerLabel_{nullptr};
    QPixmap colorshower_;

    bool editingMaterial{false};
    size_t ID{0};

private:
    Ui::MaterialCreator* ui;
};

#endif // MATERIALCREATOR_H
