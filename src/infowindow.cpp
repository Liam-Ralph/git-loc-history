// Includes

#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

#include "mainwindow.h"
#include "infowindow.h"

InfoWindow::InfoWindow(QWidget *parent) : QMainWindow(parent) {

    setWindowTitle("Git LoC History Info");

    // Create Window

    QWidget *window = new QWidget();
    QVBoxLayout *layout_back = new QVBoxLayout(window);
    layout_back->setAlignment(Qt::AlignHCenter);

    QLabel *label_name = new QLabel("Git LoC History");
    label_name->setAlignment(Qt::AlignHCenter);
    layout_back->addWidget(label_name);
    QLabel *label_version = new QLabel("Version ");
    label_version->setAlignment(Qt::AlignHCenter);
    layout_back->addWidget(label_version);

    // Credits

    QLabel *credits = new QLabel("Credits");
    QFont font = credits->font();
    font.setUnderline(true);
    credits->setFont(font);
    credits->setAlignment(Qt::AlignHCenter);
    layout_back->addWidget(credits);

    QLabel *credits_text = new QLabel(
        "Copyright (C) 2026 Liam Ralph. "
        "Logo modified from Git logo by Jason Long under the "
        "Creative Commons Attribution 3.0 Unported License. "
        "<a href=\"https://git-scm.com/community/logos\">Git Logo</a>"
    );
    credits_text->setFixedWidth(400);
    credits_text->setWordWrap(true);
    credits_text->setTextFormat(Qt::RichText);
    credits_text->setTextInteractionFlags(Qt::TextBrowserInteraction);
    credits_text->setOpenExternalLinks(true);
    credits_text->setAlignment(Qt::AlignHCenter);
    layout_back->addWidget(credits_text);

    setCentralWidget(window);

}

InfoWindow::~InfoWindow() {}
