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
    diagram.h \
    enums.h \
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
    line_tool.h \
    bed_renderable.h \
    door_renderable.h \
    stairs_renderable.h \
    basic_renderable.h \
    skybox_renderable.h \
    block_picker.h \
    block_picker_item_delegate.h \
    ladder_renderable.h \
    sprite_engine.h \
    texture_pack.h \
    pencil_tool.h \
    rectangle_tool.h \
    tool_picker.h \
    tool_picker_item_delegate.h \
    pane_renderable.h \
    qvariant_ptr.h \
    undo_command.h \
    eraser_tool.h \
    filled_rectangle_tool.h \
    flood_fill_tool.h \
    track_renderable.h

SOURCES = \
    about_box.cc \
    application.cc \
    bill_of_materials_window.cc \
    block_manager.cc \
    block_orientation.cc \
    block_position.cc \
    block_properties.cc \
    block_prototype.cc \
    diagram.cc \
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
    line_tool.cc \
    bed_renderable.cc \
    door_renderable.cc \
    stairs_renderable.cc \
    basic_renderable.cc \
    skybox_renderable.cc \
    block_picker.cc \
    block_picker_item_delegate.cc \
    ladder_renderable.cc \
    sprite_engine.cc \
    texture_pack.cc \
    tool.cc \
    pencil_tool.cc \
    rectangle_tool.cc \
    tool_picker.cc \
    tool_picker_item_delegate.cc \
    pane_renderable.cc \
    undo_command.cc \
    eraser_tool.cc \
    filled_rectangle_tool.cc \
    flood_fill_tool.cc \
    track_renderable.cc

QT += opengl

RESOURCES += \
    textures.qrc \
    icons.qrc

FORMS += \
    about_box.ui \
    bill_of_materials_window.ui \
    gl_preview_window.ui \
    main_window.ui \
    block_picker.ui \
    tool_picker.ui

INCLUDEPATH += ../third_party \
               ../third_party/qjson/include

win32:INCLUDEPATH += ../third_party/zlib-1.2.5

macx {
    QMAKE_LFLAGS += -F ../third_party/qjson/lib -L ../third_party/quazip/lib
    LIBS += -lquazip.1 -framework qjson -framework CoreFoundation
    QMAKE_POST_LINK += echo "Running install_name_tool..."; \
                       install_name_tool -id @loader_path/../Frameworks/qjson.framework/Versions/0/qjson \
                                             ../third_party/qjson/lib/qjson.framework/Versions/0/qjson; \
                       install_name_tool -change qjson.framework/Versions/0/qjson \
                                                 @loader_path/../Frameworks/qjson.framework/Versions/0/qjson \
                                                 $$OUT_PWD/MCModeler.app/Contents/MacOS/$$TARGET; \
                       install_name_tool -id @loader_path/../Frameworks/libquazip.1.0.0.dylib \
                                             ../third_party/quazip/lib/libquazip.1.0.0.dylib; \
                       install_name_tool -change libquazip.1.dylib \
                                                 @loader_path/../Frameworks/libquazip.1.0.0.dylib \
                                                 $$OUT_PWD/MCModeler.app/Contents/MacOS/$$TARGET;

    QJsonFramework.files = ../third_party/qjson/lib/qjson.framework
    QJsonFramework.path = Contents/Frameworks
    QMAKE_BUNDLE_DATA += QJsonFramework

    QuaZip.files = ../third_party/quazip/lib/libquazip.1.0.0.dylib
    QuaZip.path = Contents/Frameworks
    QMAKE_BUNDLE_DATA += QuaZip

    BlocksJson.files = ../blocks.json
    BlocksJson.path = Contents/Resources
    QMAKE_BUNDLE_DATA += BlocksJson
}

win32 {
    LIBS += ../third_party/quazip/lib/release/quazip.dll \
            ../third_party/qjson/lib/qjson0.dll
}

TARGET = "MCModeler"


























