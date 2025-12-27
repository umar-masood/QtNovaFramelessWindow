# QtNovaFrameless-Window

**QtNovaFrameless-Window** is a Qt-based replacement for the native system window frame. It provides a fully custom **title bar**, **window controls** (close, minimize, maximize), support for **resizing**, and **dark mode** theming.


## ✨ Features
- Frameless custom window (replaces native OS frame).
- Resizable with custom hit-testing.
- Custom title bar with close, minimize, and maximize buttons.
- Dark mode with themed icons.
- Active/inactive state highlighting.
- Embeddable content area for your application UI.
- Interactive custom widgets in title bar (clickable property support).

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
    Window w;
    w.setGeometry(100, 100, 800, 600);
    w.show();
    return app.exec();
}
```
### You can add your own widgets into the window’s content area like this:
> [!IMPORTANT]
> In order to add widget into the content area of window, then you must pass frameless custom window _contentArea() to the constructor of a widget.

```cpp
QPushButton *btn = new QPushButton("Click Me", window._contentArea());
QVBoxLayout *layout = new QVBoxLayout(window._contentArea());
layout->addWidget(btn);
```

### You can add interactive widgets into Custom Title bar
> [!IMPORTANT]
> If you want to add widget into the custom tilebar of frameless custom window then pass window _CustomTitleBarArea() to the constructor of that widget.
```cpp
    // Adding a button into custom title bar (interactive)
    QPushButton *titleBtn = new QPushButton("Title Action", w._CustomTitleBarArea());
    titleBtn->setProperty("clickable widget", true);  // Mark as clickable
    w._titleBarLayout()->addWidget(titleBtn);
```
> [!IMPORTANT]
> The custom title bar can hold widgets, but by default, the whole title bar is treated as draggable space. To make a widget clickable (interactive), you must set:
> ```cpp
> myWidget->setProperty("clickable widget", true);
> ```
> If not set, the widget will be considered part of the draggable title bar.
