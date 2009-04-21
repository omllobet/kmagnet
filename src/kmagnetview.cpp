/*
 * kmagnetview.cpp
 *
 * Copyright (C) 2008 %{AUTHOR} <%{EMAIL}>
 */
#include "kmagnetview.h"
#include "settings.h"

#include <klocale.h>
#include <QtGui/QLabel>

kmagnetView::kmagnetView(QWidget *)
{
    ui_kmagnetview_base.setupUi(this);
    settingsChanged();
    setAutoFillBackground(true);
}

kmagnetView::~kmagnetView()
{

}

void kmagnetView::switchColors()
{
    // switch the foreground/background colors of the label
    QColor color = Settings::col_background();
    Settings::setCol_background( Settings::col_foreground() );
    Settings::setCol_foreground( color );

    settingsChanged();
}

void kmagnetView::settingsChanged()
{
    QPalette pal;
    pal.setColor( QPalette::Window, Settings::col_background());
    pal.setColor( QPalette::WindowText, Settings::col_foreground());
    ui_kmagnetview_base.kcfg_sillyLabel->setPalette( pal );

    // i18n : internationalization
    ui_kmagnetview_base.kcfg_sillyLabel->setText( i18n("This project is %1 days old",Settings::val_time()) );
    emit signalChangeStatusbar( i18n("Settings changed") );
}

#include "kmagnetview.moc"
