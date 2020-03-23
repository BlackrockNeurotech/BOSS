#ifndef EDITACTION_H
#define EDITACTION_H

#include <QVector>

enum EditActionType
{
	EDITACTIONTYPE_NONE = 0,
	EDITACTIONTYPE_ALIGN,
	EDITACTIONTYPE_SETUNIT
};

class EditAction
{
public:
	EditAction(int channel, void * params);
	~EditAction();
	
	virtual void undo();
	virtual void redo();
    EditActionType type() { return m_type; }
	
protected:
	EditActionType  m_type;
	void 		   *m_params;
    int             m_channel;
};



struct AlignParams
{
    int          alignTo;
    int          prevAlignTo;
    int          maxShift;
    QVector<int> unitsToAlign;
};

class EditActionAlign : public EditAction
{
public:
    EditActionAlign(int channel, AlignParams * params);
	~EditActionAlign();
	
	void undo();
	void redo();
	
};

class EditActionSetUnit : public EditAction
{
public:
    EditActionSetUnit(int channel, void * params);
	~EditActionSetUnit();
	
	void undo();
	void redo();
	
};

class EditActionManager
{
public:	
    EditActionManager();
    ~EditActionManager();
	void addAction(EditAction * action);
    void undo();
    void redo();

    bool isUndoEmpty() { return undoStack.isEmpty(); }
    bool isRedoEmpty() { return redoStack.isEmpty(); }
	
    
private:
    QVector<EditAction *> undoStack;
    QVector<EditAction *> redoStack;
    void removeElementsFromStack();
	
};

#endif //EDITACTION_H