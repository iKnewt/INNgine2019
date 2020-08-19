#ifndef VALUEBOX_H
#define VALUEBOX_H
#include <QLineEdit>

///The custom made valuebox, basically a QLineEdit with a QValidator on it.
class ValueBox : public QLineEdit
{
    Q_OBJECT
public:
    ValueBox(int min, int max, int incrementValue, unsigned int index = 0, QWidget *parent = nullptr);
    ~ValueBox();
    ///The ValueBox's index in a SpinBoxProperty.
    unsigned int boxIndex = 0;
public slots:
    /**
     * Runs whenever the valuebox has been changed.
     */
    void valueBox_EditingFinished();
signals:
    /**
     * Sends a signal to SpinBoxProperty that it's been changed.
     * @param index the boxIndex specified in its contruction.
     */
    void boxChanged(unsigned int index);
};

#endif // VALUEBOX_H
