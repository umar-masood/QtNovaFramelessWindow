#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include "Window.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Create main window
    Window w;
    w.resize(800, 500);  // Set initial size

    QWidget* content = w.contentArea();
    QVBoxLayout* layout = new QVBoxLayout(content);
    layout->setAlignment(Qt::AlignCenter);

    QPushButton* toggleThemeBtn = new QPushButton("Toggle Dark/Light Mode", content);
    layout->addWidget(toggleThemeBtn);

    QObject::connect(toggleThemeBtn, &QPushButton::clicked, [&w]() {
        w.setDarkMode(!w.isDarkMode());
    });

    QPushButton* testBtn = new QPushButton("Test Button", content);
    layout->addWidget(testBtn);

    w.show();
    return app.exec();
}
