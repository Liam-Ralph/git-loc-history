// Includes

#include <QCheckBox>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

#include <QtCharts>

#include "mainwindow.h"

MainWindow::MainWindow() {

    setWindowTitle("Git LoC History");

    // Create Window

    QWidget *window = new QWidget();
    QVBoxLayout *layout_back = new QVBoxLayout(window);

    // Top

    QPushButton *info_button = new QPushButton("Info");
    QTextEdit *path_entry = new QTextEdit("Enter Git repo path");

    // Middle
    
    QHBoxLayout *layout_middle = new QHBoxLayout();

    QVBoxLayout *layout_excluded_paths = new QVBoxLayout();
    QTextEdit *excluded_paths_entry = new QTextEdit();
    layout_excluded_paths->addWidget(new QLabel("Excluded Paths"));
    layout_excluded_paths->addWidget(excluded_paths_entry);

    QGraphicsView *results_view = new QGraphicsView();

    QVBoxLayout *layout_options = new QVBoxLayout();
    QCheckBox *progress_check = new QCheckBox("Show Progress");
    layout_options->addWidget(new QLabel("Options"));
    layout_options->addWidget(progress_check);

    layout_middle->addLayout(layout_excluded_paths);
    layout_middle->addWidget(results_view);
    layout_middle->addLayout(layout_options);

    // Bottom

    QProgressBar *progress_bar = new QProgressBar();
    QLabel *timer_label = new QLabel("0.0");

    QHBoxLayout *layout_bottom = new QHBoxLayout();
    layout_bottom->addWidget(new QLabel("Git LoC History"));
    layout_bottom->addWidget(new QLabel("v"));

    layout_back->addWidget(info_button);
    layout_back->addWidget(path_entry);
    layout_back->addLayout(layout_middle);
    layout_back->addWidget(progress_bar);
    layout_back->addWidget(timer_label);
    layout_back->addLayout(layout_bottom);

    setCentralWidget(window);

}

MainWindow::~MainWindow() {}
