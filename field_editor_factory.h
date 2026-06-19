#ifndef FIELD_EDITOR_FACTORY
#define FIELD_EDITOR_FACTORY

#include <QWidget>

class FieldEditorFactory
{
public:
    static std::unique_ptr<QWidget> createNewEditor();
};

#endif // FIELD_EDITOR_FACTORY
