// Includes

#include "definitions.hpp"
#include "mainwindow.hpp"
#include "infowindow.hpp"
#include "create-loc-history.hpp"

#include <QCheckBox>
#include <QFileDialog>
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

#include <QtGraphs/QGraphsTheme>
#include <QtGraphs/QLineSeries>
#include <QtQuickWidgets/QQuickWidget>

#include <array>
#include <ctime>
#include <iostream>
#include <sstream>
#include <vector>
using namespace std;


// MainWindow Functions

MainWindow::MainWindow() : QMainWindow() {

    setWindowTitle("Git LoC History");
    setWindowIcon(QIcon(QString::fromStdString(Definitions::get_path_logo())));
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

    graph_view = new QQuickWidget();
    graph_view->setMinimumSize(400, 400);
    graph_view->setResizeMode(QQuickWidget::SizeRootObjectToView);
    graph_theme = new QGraphsTheme(graph_view);
    graph_theme->setTheme(QGraphsTheme::Theme::MixSeries);
    graph_theme->setBackgroundColor(Qt::black);
    layout_middle->addWidget(graph_view);

    QVBoxLayout *layout_options = new QVBoxLayout();
    QLabel *options_label = new QLabel("Options");
    options_label->setMaximumWidth(200);
    layout_options->addWidget(options_label);
    progress_check = new QCheckBox("Show Progress");
    progress_check->setMaximumWidth(200);
    layout_options->addWidget(progress_check);
    layout_options->setAlignment(Qt::AlignTop);
    layout_middle->addLayout(layout_options);

    layout_back->addLayout(layout_middle);

    // Bottom

    start_button = new QPushButton("Calculate Lines of Code");
    connect(start_button, &QPushButton::clicked, this, &MainWindow::create_graph);
    layout_back->addWidget(start_button);
    layout_back->setAlignment(start_button, Qt::AlignHCenter);

    QHBoxLayout *layout_progress = new QHBoxLayout();
    progress_bar = new QProgressBar();
    progress_bar->setRange(0, 100);
    progress_bar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    layout_progress->addStretch(1);
    layout_progress->addWidget(progress_bar, 3);
    layout_progress->addStretch(1);
    layout_back->addLayout(layout_progress);

    section_label = new QLabel("Not Running");
    layout_back->addWidget(section_label);
    layout_back->setAlignment(section_label, Qt::AlignCenter);

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

    start_button->setEnabled(false);

    // Create LoC History

    progress_bar->setValue(0);

    vector<string> excluded_paths;
    string entry_text = excluded_paths_entry->toPlainText().toStdString();
    stringstream ss(entry_text);
    string to;
    while (getline(ss, to, '\n')) if (to.length() != 0) excluded_paths.push_back(to);

    clock_t start = clock();

    vector<Commit> commits;

    string git_repo_path = path_entry->text().toStdString();

    try {
        if (progress_check->isChecked()) {
            function<void(double, clock_t)> on_progress_func = bind(
                &MainWindow::on_progress, this, placeholders::_1, placeholders::_2
            );
            function<void(string, clock_t)> on_section_change_func = bind(
                &MainWindow::on_section_change, this, placeholders::_1, placeholders::_2
            );
            commits = create_loc_history(
                git_repo_path, excluded_paths,
                on_progress_func, on_section_change_func, start
            );
        } else {
            commits = create_loc_history(git_repo_path, excluded_paths, nullptr, nullptr, start);
        }
    } catch (const runtime_error &e) {
        cerr << e.what() << endl;
        QMessageBox::critical(this, "Error Calculating Lines of Code", e.what());
        start_button->setEnabled(true);
        return;
    }

    section_label->setText("Finished");
    progress_bar->setValue(100);
    update_timer(start);

    // Create Graph

    // map<Language, QLineSeries *> line_series_map;

    // for (const Commit &commit : commits) {
    //     for (const auto &[lang, lines] : commit.language_map) {
    //         QLineSeries *series;
    //         series->setName(QString::fromStdString(lang.name));
    //         if (line_series_map.find(lang) == line_series_map.end())
    //             series = new QLineSeries();
    //         else
    //             series = line_series_map[lang];
    //         series->append(commit.date * 1000, lines);
    //     }
    // }

    // QList<QLineSeries *> series_list;
    // for (const auto &[lang, series] : line_series_map)
    //     series_list.append(series);

    // graph_view->setInitialProperties({
    //     {"theme", QVariant::fromValue(graph_theme)},
    //     {"seriesList", QVariant::fromValue(series_list)}
    // });
    // graph_view->loadFromModule("QtGraphs", "GraphsView");

    start_button->setEnabled(true);

}

void MainWindow::on_progress(double progress, const clock_t start) {
    update_timer(start);
    static int progress_int_prev = 0;
    int progress_int = int(round(progress * 100));
    if (progress_int > progress_int_prev) {
        progress_int_prev = progress_int;
        progress_bar->setValue(progress_int);
    }
}

void MainWindow::on_section_change(string section, const clock_t start) {
    update_timer(start);
    section_label->setText(QString::fromStdString(section));
}

void MainWindow::update_timer(const clock_t start) {
    stringstream ss;
    ss << fixed << setprecision(2) << double(clock() - start) / CLOCKS_PER_SEC << "s";
    timer_label->setText(QString::fromStdString(ss.str()));
}
