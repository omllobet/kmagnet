/*
 * kmagnet.h
 *
 * Copyright (C) 2008 %{AUTHOR} <%{EMAIL}>
 */
#ifndef KMAGNET_H
#define KMAGNET_H


#include <kxmlguiwindow.h>

#include "ui_prefs_base.h"

class kmagnetView;
class QPrinter;
class KToggleAction;
class KUrl;

/**
 * This class serves as the main window for kmagnet.  It handles the
 * menus, toolbars, and status bars.
 *
 * @short Main window class
 * @author %{AUTHOR} <%{EMAIL}>
 * @version %{VERSION}
 */
class kmagnet : public KXmlGuiWindow
{
    Q_OBJECT
public:
    /**
     * Default Constructor
     */
    kmagnet();

    /**
     * Default Destructor
     */
    virtual ~kmagnet();

private slots:
    void fileNew();
    void optionsPreferences();

private:
    void setupActions();

private:
    Ui::prefs_base ui_prefs_base ;
    kmagnetView *m_view;

    QPrinter   *m_printer;
    KToggleAction *m_toolbarAction;
    KToggleAction *m_statusbarAction;
};

#endif // _KMAGNET_H_
