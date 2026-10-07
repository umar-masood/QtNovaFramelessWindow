#include <QApplication>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QTimer>

#include "Window.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Window window;
    window.resize(900, 600);
    window.setBackgroundColor("#FF8088", "#FF1235");

    // --------------------------------------------------
    // Content
    // --------------------------------------------------

    auto *layout = new QVBoxLayout(window.contentArea());

    auto *label = new QLabel("Window Test");
    label->setAlignment(Qt::AlignCenter);

    auto *darkMode = new QCheckBox("Dark Mode");

    auto *blockInteraction = new QCheckBox("Block Interaction");

    auto *borderButton = new QPushButton("Change Border");

    layout->addWidget(label);
    layout->addWidget(darkMode);
    layout->addWidget(blockInteraction);
    layout->addWidget(borderButton);

    // --------------------------------------------------
    // Tests
    // --------------------------------------------------

    QObject::connect(darkMode, &QCheckBox::toggled,
                     &window, &Window::setDarkMode);

    QObject::connect(blockInteraction, &QCheckBox::toggled, &window, [&window](){
        window.setModal(true); 
        QTimer::singleShot(15000, [&window](){
            window.setModal(false);
        });
    });

    QObject::connect(borderButton, &QPushButton::clicked,
                     [&window]() {
                         window.setBorderColor(Qt::red);
                     });

  

    window.show();

    return app.exec();
}