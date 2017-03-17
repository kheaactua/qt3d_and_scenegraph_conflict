This repo exists as an example of the Scene Graph interfering (flicking) with the Qt3D forward renderer.

# Build

The project is setup to build with CMake, *i.e.*

```bash
# Linux

cmake -DQt_VERSION_RELEASE=5 -DQt_VERSION_MAJOR=8 -DQt_VERSION_MINOR=0 -DQt_DIR=/opt/Qt5.8.0/5.8/gcc_64 ${SOURCE}

# Windows

cmake -DQt_VERSION_RELEASE=5 -DQt_VERSION_MAJOR=8 -DQt_VERSION_MINOR=0 -DQt_DIR=c:/opt/Qt5.8.0/5.8/msvc2013_64 %SOURCE%
```

Not all the Qt options above are required, the CMakeList does have defaults for
them, but the explicit example above is good if you have a non-default
installation.

Once CMake generates build files, the project should be able to build.  The program will be output to `${BUILD_DIR}/app`.

The CMake files also included code to create an installer with the [nullsoft
scriptable install system (NSIS)](http://nsis.sourceforge.net/Main_Page).
Because the flicking issue only occurs on a Windows Surface, which in our case
does not have a compiler, we run the installer and transfer it to the Surface.

The installer can be run in MSVC using the `Install` and `Package` targets.
Not relevant, but the Linux installer currently does not configure the rpath on
some of the build targets, so to run this example from an installed version in
Linux, the `LD_LIBRARY_PATH` needs to be set.

# Code

The Qt3D renderer is implemented as:
1. Off the root, a `Scene3d` is used, which instantiates `ShipScene`
1. `ShipScene` creates a `ViewportFrameGraphs` and adds it as a component
1. `ViewportFrameGraphs` is modelled off the [multi viewport example](http://doc.qt.io/qt-5/qt3d-multiviewport-example.html)
1. The renderer calls `ClearBuffer` with a fully opaque colour

The scene graph, modelled after the [Scene Graph - OpenGL Under
QML](http://doc.qt.io/qt-5/qtquick-scenegraph-openglunderqml-example.html)
example.

1. In `main.qml`, a `Ground` object is created.  This is just a wrapper to `GroundModel`.
1. `GroundModel`, a `QQuickItem` attaches to `&QQuickWindow::afterRendering` which syncs its state to `GroundRenderer` and calls `GroundRenderer::paint`
1. In this example, the renderer does not clear before drawing.

# Discussion

Before setting up this example, I was connecting to
`&QQuickWindow::beforeRendering`, calling clear in the scene graph, and
ClearBuffers with a transparent colour in `ViewportFrameGraphs`.  I felt that
this example though was simpler and displayed the same flickering issue.

Another weird eccentricity is that when the SceneGraph is simply initialized
(not never drawn to), the QML displays the 3D model upsidedown.  *i.e.* the Top
camera shows the ship model from underneath, the back from the front, and the
user camera from the bottom right instead of the top left.  I haven't yet spent
time debugging this though.
