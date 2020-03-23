#ifndef EDITGROUP_H
#define EDITGROUP_H

#include <QWidget>
#include <QMap>

class QLineEdit;

class EditGroup : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString currentValue READ currentValue WRITE setcurrentValue NOTIFY valueChanged USER true)
public:
    EditGroup(QWidget *parent = 0);

    QString currentValue() const;
    void setcurrentValue(QString value);

    void addLineEdit(QLineEdit *edit, QString key);
    void setKey(QString key);
    virtual QString convertValue(QString value, QString keyIn, QString keyOut) = 0;

signals:
    void valueChanged(QString selection);

public slots:
    void slotEditingFinished();

private:
    QString m_CurrentValue;
    QString m_key;
    QMap<QString, QLineEdit*> m_editMap;

    void updateEdits(QString key, QString value);
};

#endif // EDITGROUP_H
