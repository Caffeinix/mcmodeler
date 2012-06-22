HEADERS = \
    about_box.h \
    application.h \
    bill_of_materials_window.h \
    block_instance.h \
    block_manager.h \
    block_oracle.h \
    block_orientation.h \
    block_position.h \
    block_properties.h \
    block_prototype.h \
    block_type.h \
    camera.h \
    cube.h \
    cubic_renderable.h \
    diagram.h \
    enums.h \
    flow_layout.h \
    frame_timer.h \
    gl_preview_window.h \
    gl_widget.h \
    level_widget.h \
    main_window.h \
    matrix.h \
    mouselook_cam.h \
    overlapping_faces_renderable.h \
    rectangular_prism_renderable.h \
    render_delegate.h \
    renderable.h \
    texture.h \
    block_transaction.h \
    macros.h \
    tool.h \
    line_tool.h

SOURCES = \
    about_box.cc \
    application.cc \
    bill_of_materials_window.cc \
    block_manager.cc \
    block_orientation.cc \
    block_position.cc \
    block_properties.cc \
    block_prototype.cc \
    cube.cc \
    cubic_renderable.cc \
    diagram.cc \
    flow_layout.cc \
    frame_timer.cc \
    gl_preview_window.cc \
    gl_widget.cc \
    level_widget.cc \
    main.cc \
    main_window.cc \
    matrix.cc \
    mouselook_cam.cc \
    overlapping_faces_renderable.cc \
    rectangular_prism_renderable.cc \
    renderable.cc \
    texture.cc \
    block_transaction.cc \
    block_instance.cc \
    line_tool.cc

QT += opengl

RESOURCES += \
    textures.qrc

FORMS += \
    about_box.ui \
    bill_of_materials_window.ui \
    gl_preview_window.ui \
    main_window.ui

INCLUDEPATH += ../third_party/qjson/include
LIBS += ../third_party/qjson/lib/qjson0.dll

TARGET = "MCModeler"
