#ifndef LANDSCAPEEDITOR_H
#define LANDSCAPEEDITOR_H

#include <QDialog>

namespace Ui {
class LandscapeEditor;
}
class ComboBoxProperty;
///The landscape Editor edits the current landscape on the scene.
class LandscapeEditor : public QDialog
{
    Q_OBJECT

public:
    explicit LandscapeEditor(QWidget *parent = nullptr);
    ~LandscapeEditor();

    /**
     * Initializes the window, and creates all information needed.
     */
    void InitalizeWindow();
    /**
     * Updates the material combo box to show each respective material asset.
     */
    void UpdateComboBox();
    ComboBoxProperty* selectMaterial_{nullptr};
public slots:
    void event_setMesh_clicked();
    void event_selectMaterial_indexChanged();

private:
    Ui::LandscapeEditor *ui;
};

#endif // LANDSCAPEEDITOR_H
