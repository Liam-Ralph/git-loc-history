// Includes

#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

#include <QtGraphs>

#include "mainwindow.h"

MainWindow::MainWindow() {

    setWindowTitle("Git LoC History");

    // Create Window

    QWidget *window = new QWidget();
    QVBoxLayout *layout_back = new QVBoxLayout(window);

    QPushButton *info_button = new QPushButton("Info");
    QTextEdit *path_entry = new QTextEdit("Enter Git repo path");
    QHBoxLayout *layout_middle = new QHBoxLayout();
    QProgressBar *progress_bar = new QProgressBar();
    QLabel *timer_label = new QLabel("0.0");
    QHBoxLayout *layout_bottom = new QHBoxLayout();

    layout_back->addWidget(info_button);
    layout_back->addWidget(path_entry);
    layout_back->addLayout(layout_middle);
    layout_back->addWidget(progress_bar);
    layout_back->addWidget(timer_label);
    layout_back->addLayout(layout_bottom);

    setCentralWidget(window);

}

MainWindow::~MainWindow() {}
