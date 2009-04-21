/*
 * kmagnetview.h
 *
 * Copyright (C) 2007 %{AUTHOR} <%{EMAIL}>
 */
#ifndef KMAGNETVIEW_H
#define KMAGNETVIEW_H

#include <QtGui/QWidget>

#include "ui_kmagnetview_base.h"

class QPainter;
class KUrl;

/**
 * This is the main view class for kmagnet.  Most of the non-menu,
 * non-toolbar, and non-statusbar (e.g., non frame) GUI code should go
 * here.
 *
 * @short Main view
 * @author %{AUTHOR} <%{EMAIL}>
 * @version %{VERSION}
 */

class kmagnetView : public QWidget, public Ui::kmagnetview_base
{
    Q_OBJECT
public:
    /**
     * Default constructor
     */
    kmagnetView(QWidget *parent);

    /**
     * Destructor
     */
    virtual ~kmagnetView();

private:
    Ui::kmagnetview_base ui_kmagnetview_base;

signals:
    /**
     * Use this signal to change the content of the statusbar
     */
    void signalChangeStatusbar(const QString& text);

    /**
     * Use this signal to change the content of the caption
     */
    void signalChangeCaption(const QString& text);

private slots:
    void switchColors();
    void settingsChanged();
};

#endif // kmagnetVIEW_H
