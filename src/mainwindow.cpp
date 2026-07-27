// Includes

#include "definitions.hpp"
#include "mainwindow.hpp"
#include "infowindow.hpp"
#include "create-loc-history.hpp"

#include <QCheckBox>
#include <QFileDialog>
#include <QGraphicsView>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QTextEdit>
#include <QToolTip>
#include <QVBoxLayout>
#include <QWidget>

#include <QtCharts>

#include <array>
#include <atomic>
#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>
using namespace std;


// Functions

string format_time(clock_t start) {
    stringstream ss;
    ss << fixed << setprecision(2) << double(clock() - start) / CLOCKS_PER_SEC;
    return ss.str();
}

// MainWindow Functions

MainWindow::MainWindow() : QMainWindow() {

    setWindowTitle("Git LoC History");
    setWindowState(Qt::WindowMaximized);

    // Create Window

    QWidget *window = new QWidget();
    QVBoxLayout *layout_back = new QVBoxLayout(window);
    setCentralWidget(window);

    // Top

    QPushButton *info_button = new QPushButton("Info");
    connect(info_button, &QPushButton::clicked, this, &MainWindow::show_info);
    layout_back->addWidget(info_button);
    layout_back->setAlignment(info_button, Qt::AlignRight);

    QGridLayout *layout_path_entry = new QGridLayout();
    path_entry = new QLineEdit();
    path_entry->setPlaceholderText("Enter Local Path");
    path_entry->setMinimumWidth(300);
    layout_path_entry->addWidget(path_entry, 0, 1, Qt::AlignHCenter);
    QPushButton *path_button = new QPushButton("Choose Local Path");
    connect(path_button, &QPushButton::clicked, this, &MainWindow::open_path_dialog);
    layout_path_entry->addWidget(path_button, 0, 2, Qt::AlignLeft);
    layout_path_entry->setColumnStretch(0, 1);
    layout_path_entry->setColumnStretch(2, 1);
    layout_back->addLayout(layout_path_entry);

    // Middle
    
    QHBoxLayout *layout_middle = new QHBoxLayout();

    QVBoxLayout *layout_excluded_paths = new QVBoxLayout();
    layout_excluded_paths->addWidget(new QLabel("Excluded Paths"));
    excluded_paths_entry = new QTextEdit();
    excluded_paths_entry->setPlaceholderText("One path per line");
    excluded_paths_entry->setMinimumWidth(100);
    excluded_paths_entry->setMaximumWidth(400);
    excluded_paths_entry->setToolTip(
        "Paths are not absolute by default (e.g. foo will exclude /foo and /bar/foo). "
        "Absolute paths (e.g. /foo) are relative to repository path."
    );
    layout_excluded_paths->addWidget(excluded_paths_entry);
    layout_middle->addLayout(layout_excluded_paths);

    results_view = new QGraphicsView();
    results_view->setMinimumSize(400, 400);
    layout_middle->addWidget(results_view);

    QVBoxLayout *layout_options = new QVBoxLayout();
    layout_options->addWidget(new QLabel("Options"));
    progress_check = new QCheckBox("Show Progress");
    layout_options->addWidget(progress_check);
    layout_options->setAlignment(Qt::AlignTop);
    layout_middle->addLayout(layout_options);

    layout_back->addLayout(layout_middle);

    // Bottom

    QPushButton *start_button = new QPushButton("Calculate Lines of Code");
    connect(start_button, &QPushButton::clicked, this, &MainWindow::create_graph);
    layout_back->addWidget(start_button);
    layout_back->setAlignment(start_button, Qt::AlignHCenter);

    QHBoxLayout *layout_progress = new QHBoxLayout();
    progress_bar = new QProgressBar();
    progress_bar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    layout_progress->addStretch(1);
    layout_progress->addWidget(progress_bar, 3);
    layout_progress->addStretch(1);
    layout_back->addLayout(layout_progress);

    timer_label = new QLabel("0.0s");
    layout_back->addWidget(timer_label);
    layout_back->setAlignment(timer_label, Qt::AlignCenter);

    QHBoxLayout *layout_bottom = new QHBoxLayout();
    QLabel *name_label = new QLabel("Git LoC History");
    layout_bottom->addWidget(name_label);
    layout_bottom->setAlignment(name_label, Qt::AlignLeft);
    QString version = QString::fromStdString(Definitions::get_version());
    QLabel *version_label = new QLabel(QString("v") + version);
    layout_bottom->addWidget(version_label);
    layout_bottom->setAlignment(version_label, Qt::AlignRight);
    layout_back->addLayout(layout_bottom);

}

MainWindow::~MainWindow() {}

void MainWindow::show_info() {
    InfoWindow *info_window = new InfoWindow(this);
    info_window->show();
}

void MainWindow::open_path_dialog() {
    QFileDialog *dialog = new QFileDialog();
    dialog->setOption(QFileDialog::ShowDirsOnly);
    dialog->setFileMode(QFileDialog::Directory);
    dialog->setDirectory(QDir::homePath());
    dialog->exec();
    if (dialog->result() == QDialog::Accepted && !dialog->selectedFiles().isEmpty())
        path_entry->setText(dialog->selectedFiles()[0]);
}

void MainWindow::create_graph() {

    // Create LoC History

    array<atomic<int>, 6> progress;
    array<atomic<int>, 6> *progress_ptr = NULL;
    if (progress_check->isChecked() == true) {
        progress[0] = 0;
        progress[1] = 1;
        progress[2] = 0;
        progress[3] = 1;
        progress[4] = 0;
        progress[5] = 1;
        progress_ptr = &progress;
    }

    vector<string> excluded_paths;
    string entry_text = excluded_paths_entry->toPlainText().toStdString();
    stringstream ss(entry_text);
    string to;
    while (getline(ss, to, '\n')) if (to.length() != 0) excluded_paths.push_back(to);

    start = clock();

    vector<Commit> commits;

    string git_repo_path = path_entry->text().toStdString();

    unique_ptr<std::thread> t_ptr;
    if (progress_ptr != NULL)
        t_ptr = unique_ptr<std::thread>(
            new std::thread(
                progress_tracker,
                progress_ptr,
                git_repo_path.substr(0, 4).compare("http") == 0,
                start
            )
        );

    try {
        commits = create_loc_history(git_repo_path, excluded_paths, progress_ptr);
    } catch (const runtime_error &e) {
        cerr << e.what() << endl;
        QMessageBox::critical(this, "Error Calculating Lines of Code", e.what());
        return;
    }

    if (progress_ptr != NULL)
        (*t_ptr).join();

    update_timer();

}

void MainWindow::progress_tracker(array<atomic<int>, 6> *progress_ptr, bool cloning) {

}

void MainWindow::update_timer() {
    stringstream ss;
    ss << fixed << setprecision(2) << double(clock() - start) / CLOCKS_PER_SEC;
    timer_label->setText(QString::fromStdString(ss.str() + "s"));
}
