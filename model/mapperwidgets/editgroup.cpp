#include <QLineEdit>
#include "../model/mapperwidgets/editgroup.h"

EditGroup::EditGroup(QWidget *parent) :
    QWidget(parent)
{
    m_CurrentValue = "";
    m_key = "";
}

// property used by the model
QString EditGroup::currentValue() const
{
    return m_CurrentValue;
}

// property used by the model
void EditGroup::setcurrentValue(QString value)
{
    updateEdits(m_key, value);
}

// add a line edit to the map and listen for signals from it
void EditGroup::addLineEdit(QLineEdit *edit, QString key)
{
    m_editMap[key] = edit;
    connect(edit, SIGNAL(editingFinished()), this, SLOT(slotEditingFinished()));
}

// set the key for the value the model sees
void EditGroup::setKey(QString key)
{
    m_key = key;
}

// an edit field was changed. Convert and set the value in the other edits and signal the model
void EditGroup::slotEditingFinished()
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());
    Q_ASSERT(edit);
    QString key = m_editMap.key(edit);
    QString value = edit->text();
    updateEdits(key, value);

    emit valueChanged(m_CurrentValue);
}

void EditGroup::updateEdits(QString key, QString value)
{
    for (QMap<QString, QLineEdit*>::iterator iter = m_editMap.begin(); iter != m_editMap.end(); ++iter)
    {
        QString val = convertValue(value, key, iter.key());
        iter.value()->setText(val);

        if (iter.key() == m_key)
            m_CurrentValue = val;
    }
}

