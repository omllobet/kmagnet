set(kmagnet_SRCS 
   kmagnetscene.cpp
   kmagnetview.cpp
   kmagnet.cpp
   main.cpp
   kmagnetcell.cpp
   kmagnetsolver.cpp
 )

kde4_add_ui_files(kmagnet_SRCS prefs_base.ui)

kde4_add_kcfg_files(kmagnet_SRCS settings.kcfgc )

kde4_add_executable(kmagnet ${kmagnet_SRCS})

target_link_libraries(kmagnet ${KDE4_KDEUI_LIBS} kdegames )

install(TARGETS kmagnet ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( FILES kmagnet.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR} )
install( FILES kmagnet.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES kmagnetui.rc  DESTINATION  ${DATA_INSTALL_DIR}/kmagnet )
