set(kmagnet_SRCS kmagnetgenerator.cpp kmagnetrenderer.cpp 
   kmagnetscene.cpp
   kmagnetview.cpp
   kmagnet.cpp
   main.cpp
   kmagnetcell.cpp
   kmagnetsolver.cpp
 )

include_directories(${KDEGAMES_INCLUDE_DIRS})

kde4_add_ui_files(kmagnet_SRCS prefs_base.ui)

kde4_add_kcfg_files(kmagnet_SRCS settings.kcfgc )

kde4_add_executable(kmagnet ${kmagnet_SRCS})

target_link_libraries(kmagnet ${KDE4_KDEUI_LIBS} ${KDE4_KFILE_LIBS} ${KDEGAMES_LIBRARY}  )

install(TARGETS kmagnet ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( FILES kmagnet.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR} )
install( FILES kmagnet.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES kMagnetui.rc  DESTINATION  ${DATA_INSTALL_DIR}/kMagnet )
install( FILES kMagnet.knsrc  DESTINATION  ${CONFIG_INSTALL_DIR} )
