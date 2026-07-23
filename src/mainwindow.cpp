// Includes

#include <QCheckBox>
#include <QFileDialog>
#include <QGraphicsView>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QTextEdit>
#include <QToolTip>
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
    layout_back->addWidget(info_button);
    layout_back->setAlignment(info_button, Qt::AlignRight);

    QGridLayout *layout_path_entry = new QGridLayout();
    QLineEdit *path_entry = new QLineEdit();
    path_entry->setPlaceholderText("Enter Local Path");
    path_entry->setMinimumWidth(400);
    layout_path_entry->addWidget(path_entry, 0, 1, Qt::AlignHCenter);
    QPushButton *path_button = new QPushButton("Choose Local Path");
    layout_path_entry->addWidget(path_button, 0, 2, Qt::AlignRight);
    layout_path_entry->setColumnStretch(0, 1);
    layout_path_entry->setColumnStretch(2, 1);
    layout_back->addLayout(layout_path_entry);

    // Middle
    
    QHBoxLayout *layout_middle = new QHBoxLayout();

    QVBoxLayout *layout_excluded_paths = new QVBoxLayout();
    QTextEdit *excluded_paths_entry = new QTextEdit();
    excluded_paths_entry->setPlaceholderText("One path per line");
    excluded_paths_entry->setMinimumWidth(100);
    excluded_paths_entry->setMaximumWidth(400);
    excluded_paths_entry->setToolTip(
        "Paths are not absolute by default (e.g. foo will exclude /foo and /bar/foo). "
        "Absolute paths (e.g. /foo) are relative to repository path."
    );
    layout_excluded_paths->addWidget(new QLabel("Excluded Paths"));
    layout_excluded_paths->addWidget(excluded_paths_entry);
    layout_middle->addLayout(layout_excluded_paths);

    QGraphicsView *results_view = new QGraphicsView();
    layout_middle->addWidget(results_view);

    QVBoxLayout *layout_options = new QVBoxLayout();
    layout_options->addWidget(new QLabel("Options"));
    QCheckBox *progress_check = new QCheckBox("Show Progress");
    layout_options->addWidget(progress_check);
    layout_options->setAlignment(Qt::AlignTop);
    layout_middle->addLayout(layout_options);

    layout_back->addLayout(layout_middle);

    // Bottom

    QProgressBar *progress_bar = new QProgressBar();
    progress_bar->setMinimumWidth(400);
    progress_bar->setMaximumWidth(1000);
    progress_bar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    layout_back->addWidget(progress_bar);
    layout_back->setAlignment(progress_bar, Qt::AlignCenter);
    QLabel *timer_label = new QLabel("0.0s");
    layout_back->addWidget(timer_label);
    layout_back->setAlignment(timer_label, Qt::AlignCenter);

    QHBoxLayout *layout_bottom = new QHBoxLayout();
    QLabel *name_label = new QLabel("Git LoC History");
    layout_bottom->addWidget(name_label);
    layout_bottom->setAlignment(name_label, Qt::AlignLeft);
    QLabel *version_label = new QLabel("v");
    layout_bottom->addWidget(version_label);
    layout_bottom->setAlignment(version_label, Qt::AlignRight);
    layout_back->addLayout(layout_bottom);

    setCentralWidget(window);

}

MainWindow::~MainWindow() {}
