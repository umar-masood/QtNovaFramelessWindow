# QtNovaFramelessWindow

**QtNovaFramelessWindow** is a Qt-based replacement for the native system window frame. It provides a fully custom **title bar**, **window controls** (close, minimize, maximize), support for **resizing**, and **dark mode** theming.


## ✨ Features
- Frameless custom window (replaces native OS frame).
- Resizable with custom hit-testing.
- Custom title bar with close, minimize, and maximize buttons.
- Dark mode with themed icons.
- Active/inactive state highlighting.
- Embeddable content area for your application UI.
- Interactive custom widgets in title bar (clickable property support).

## 📦 File Structure
**QtNovaFramelessWindow/**

`Window.h`  

`Window.cpp`  

`CMakeLists.txt`

`resources.qrc`

`components/Button.h`

> [!IMPORTANT]
> This customized window uses a custom `Button` component, make sure that component must be present in its folder. To get custom Button component, check out our `QtNovaUI` repo on GitHub.

> [!IMPORTANT]
> Must include `Button.h` dependencies which includes `SmoothOpacity.cpp/.h`, `SmoothShadow.cpp/h`, `SpinnerProgress.cpp/.h`.

## 🚀 Usage

### Include in your project
1. Copy `Window.h` and `Window.cpp` into your project’s `src/` folder.
2. Include the header in your code:

```cpp
#include "Window.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Create custom window
    Window *w = new Window;
    w->setGeometry(100, 100, 800, 600);
    w->show();

    return app.exec();
}
```

## 🧱 Adding Application UI (Content Area)
```cpp
QVBoxLayout *layout = new QVBoxLayout(window->contentArea());
QPushButton *btn = new QPushButton("Click Me");
layout->addWidget(btn);
```
> [!IMPORTANT]
> All application widgets must be parented to the window’s content area.

> [!WARNING]
> `QPushButton *btn = new QPushButton("Click Me", window);`
> 
> Passing window as a parent directly will break layout, resizing and hit-testing

## Custom Title Bar Widgets
```cpp
// Adding Widgets to the Title Bar
QHBoxLayout *layout = new QHBoxLayout(window->titleBar());
QPushButton *btn = new QPushButton("Action");
layout->addWidget(btn);
window->setInteractiveTitleBarWidget(btn);
```
> [!IMPORTANT]
> To make a widget interactive then must pass that widget to this function: 
> `window.setInteractiveTitleBarWidget(myWidget)`;

> [!NOTE]
> `window.setInteractiveTitleBarWidget(myWidget)` will automatically ignore the widget if that widget is already passed to this function.

## Dark Theme Mode
```cpp
w->setDarkMode(true);
```
## 🤝 Contributing

1. Fork the repository.
2. Create a new branch for your change.
3. Follow the existing code style and structure.
4. Do not break window dragging, resizing, or title bar behavior.
5. Test your changes before committing.
6. Write clear commit messages.
7. Update documentation if behavior changes.
8. Push your branch to your fork.
9. Open a pull request against the main branch.
