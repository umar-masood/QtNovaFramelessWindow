#include "Window.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Window w;
    w.resize(500, 400);

    // --- Add to content area ---
    QVBoxLayout* contentLayout = new QVBoxLayout(w._contentArea());
    contentLayout->setContentsMargins(10, 10, 10, 10);
    contentLayout->setSpacing(10);
    contentLayout->addWidget(new QLabel("Welcome to the content area"));
    contentLayout->addWidget(new QPushButton("Click Me"));

    // --- Add to title bar ---
    QLabel* titleLabel = new QLabel("Custom Title");
    titleLabel->setStyleSheet("font-weight: bold; color: white;");
    titleLabel->setProperty("clickable widget", false); // Allow drag to work
    w._titleBarLayout()->addWidget(titleLabel);

    w.show();
    return app.exec();
}

