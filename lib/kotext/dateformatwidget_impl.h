#ifndef DATEFORMATWIDGET_H
#define DATEFORMATWIDGET_H
#include "timedateformatwidget.h"

class DateFormatWidget : public TimeDateFormatWidgetPrototype
{ 
    Q_OBJECT

public:
    DateFormatWidget( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~DateFormatWidget();
    QString resultString();
public slots:
    void updateLabel();
    void comboActivated();
    void slotPersonalizeChanged(bool b);
    void slotDefaultValueChanged(const QString & );
};

#endif // DATEFORMATWIDGET_H
