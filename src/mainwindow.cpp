// Includes

#include "definitions.hpp"
#include "mainwindow.hpp"
#include "infowindow.hpp"
#include "create-loc-history.hpp"

#include <QCheckBox>
#include <QFileDialog>
#include <QGridLayout>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QStyleHints>
#include <QToolTip>
#include <QVBoxLayout>
#include <QWidget>

#include <QtCharts/QAreaSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QChartView>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QLineSeries>
#include <QtCharts/QStackedBarSeries>
#include <QtCharts/QValueAxis>

#include <array>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>
using namespace std;


// MainWindow Functions

/**
 * Constructor
 */
MainWindow::MainWindow() : QMainWindow() {

    setWindowTitle("Git LoC History");
    setWindowIcon(QIcon(QString::fromStdString(Definitions::get_path_logo())));
    setWindowState(Qt::WindowMaximized);
    setMinimumSize(800, 550);

    // Get Settings

    settings_map = Definitions::get_config();

    // Create Window

    QWidget *window = new QWidget();
    QVBoxLayout *layout_back = new QVBoxLayout(window);
    setCentralWidget(window);

    // Top

    // Info Button

    QPushButton *info_button = new QPushButton("Info");
    connect(info_button, &QPushButton::clicked, this, &MainWindow::show_info);
    layout_back->addWidget(info_button);
    layout_back->setAlignment(info_button, Qt::AlignRight);

    // Path Entry

    QGridLayout *layout_path_entry = new QGridLayout();
    path_entry = new QLineEdit();
    path_entry->setPlaceholderText("Enter Repository Path");
    path_entry->setMinimumWidth(300);
    layout_path_entry->addWidget(path_entry, 0, 1, Qt::AlignHCenter);
    QPushButton *path_button = new QPushButton("Choose Local Path");
    connect(path_button, &QPushButton::clicked, this, &MainWindow::open_path_dialog);
    layout_path_entry->addWidget(path_button, 0, 2, Qt::AlignLeft);
    layout_path_entry->setColumnStretch(0, 1);
    layout_path_entry->setColumnStretch(2, 1);
    layout_back->addLayout(layout_path_entry);

    // Commit Info

    commit_info_label = new QLabel("");
    commit_info_label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    layout_back->addWidget(commit_info_label);
    layout_back->setAlignment(commit_info_label, Qt::AlignCenter);

    // Middle

    QHBoxLayout *layout_middle = new QHBoxLayout();

    // Excluded Paths

    QVBoxLayout *layout_excluded_paths = new QVBoxLayout();
    layout_excluded_paths->addWidget(new QLabel("Excluded Paths"));
    excluded_paths_entry = new QPlainTextEdit();
    excluded_paths_entry->setPlaceholderText("One path per line");
    excluded_paths_entry->setMinimumWidth(100);
    excluded_paths_entry->setMaximumWidth(400);
    excluded_paths_entry->setToolTip(
        "Paths are not absolute by default (e.g. foo will exclude /foo and /bar/foo). "
        "Absolute paths (e.g. /foo) are relative to repository path."
    );
    layout_excluded_paths->addWidget(excluded_paths_entry);
    layout_middle->addLayout(layout_excluded_paths);

    // Chart

    chart_view = new QChartView(window);
    chart_view->setMinimumWidth(500);
    if (is_dark_mode()) {
        chart_view->setBackgroundBrush(Qt::black);
        chart_view->setForegroundBrush(Qt::black);
    }
    layout_middle->addWidget(chart_view);

    // Options

    QVBoxLayout *layout_options = new QVBoxLayout();

    // Run Options

    layout_options->addWidget(new QLabel("Run Options"));

    branch_entry = new QLineEdit();
    branch_entry->setPlaceholderText("Branch");
    branch_entry->setMaximumWidth(200);
    layout_options->addWidget(branch_entry);

    progress_check = new QCheckBox("Show Progress");
    layout_options->addWidget(progress_check);

    cache_this_check = new QCheckBox("Cache This Result");
    layout_options->addWidget(cache_this_check);

    chart_type_combo = new QComboBox();
    chart_type_combo->addItems({"Line", "Bar"});
    chart_type_combo->setCurrentIndex(0);
    layout_options->addWidget(chart_type_combo);

    // Program Options

    layout_options->addWidget(new QLabel("Program Options"));

    // Settings Check Boxes

    QCheckBox *cache_results_check = new QCheckBox("Cache Results");
    if (settings_map["cache_results"].compare("true") == 0)
        cache_results_check->setChecked(true);
    layout_options->addWidget(cache_results_check);

    QCheckBox *show_warnings_check = new QCheckBox("Show Warnings");
    if (settings_map["show_warnings"].compare("true") == 0)
        show_warnings_check->setChecked(true);
    layout_options->addWidget(show_warnings_check);

    // Connect Setting Change Signals

    #if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
        auto cache_results_sender = cache_results_check;
        auto cache_results_signal = &QCheckBox::checkStateChanged; // This signal was added in 6.7
        auto show_warnings_sender = show_warnings_check;
        auto show_warnings_signal = &QCheckBox::checkStateChanged;
    #else
        QPushButton *update_settings_button = new QPushButton("Update Settings");
        layout_options->addWidget(update_settings_button);
        auto cache_results_sender = update_settings_button;
        auto cache_results_signal = &QPushButton::clicked;
        auto show_warnings_sender = update_settings_button;
        auto show_warnings_signal = &QPushButton::clicked;
    #endif

    connect(
        cache_results_sender, cache_results_signal, this,
        [this, cache_results_check]() {
            const string state = cache_results_check->isChecked() ? "true" : "false";
            int error = Definitions::set_config("cache_results", state);
            if (error != 0) warn_set_config_error(error);
            settings_map["cache_results"] = state;
        }
    );

    connect(
        show_warnings_sender, show_warnings_signal, this,
        [this, show_warnings_check]() {
            const string state = show_warnings_check->isChecked() ? "true" : "false";
            int error = Definitions::set_config("show_warnings", state);
            if (error != 0) warn_set_config_error(error);
            settings_map["show_warnings"] = state;
        }
    );

    // Cache Size and Clear Cache Button

    cache_size_label = new QLabel();
    update_cache_size();
    layout_options->addWidget(cache_size_label);

    QPushButton *clear_cache_button = new QPushButton("Clear Cache");
    connect(clear_cache_button, &QPushButton::clicked, this, [this]() {
        filesystem::remove_all(Definitions::get_path_cache());
        update_cache_size();
    });
    layout_options->addWidget(clear_cache_button);

    layout_options->setAlignment(Qt::AlignTop);
    layout_middle->addLayout(layout_options);

    layout_back->addLayout(layout_middle);

    // Bottom

    // Start Button

    start_button = new QPushButton("Calculate Lines of Code");
    connect(start_button, &QPushButton::clicked, this, &MainWindow::create_chart);
    layout_back->addWidget(start_button);
    layout_back->setAlignment(start_button, Qt::AlignHCenter);

    // Progress Bar

    QHBoxLayout *layout_progress = new QHBoxLayout();
    progress_bar = new QProgressBar();
    progress_bar->setRange(0, 100);
    progress_bar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    layout_progress->addStretch(1);
    layout_progress->addWidget(progress_bar, 3);
    layout_progress->addStretch(1);
    layout_back->addLayout(layout_progress);

    // Section Label

    section_label = new QLabel("Not Running");
    section_label->setMinimumWidth(300);
    section_label->setAlignment(Qt::AlignCenter);
    layout_back->addWidget(section_label);
    layout_back->setAlignment(section_label, Qt::AlignCenter);

    // Timer

    timer_label = new QLabel("0.0s");
    timer_label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    layout_back->addWidget(timer_label);
    layout_back->setAlignment(timer_label, Qt::AlignCenter);

    // Footer Bar

    QHBoxLayout *layout_bottom = new QHBoxLayout();
    QLabel *name_label = new QLabel("Git LoC History");
    layout_bottom->addWidget(name_label);
    layout_bottom->setAlignment(name_label, Qt::AlignLeft);
    QLabel *version_label = new QLabel(QString::fromStdString("v" + Definitions::get_version()));
    layout_bottom->addWidget(version_label);
    layout_bottom->setAlignment(version_label, Qt::AlignRight);
    layout_back->addLayout(layout_bottom);

}

/**
 * Destructor
 */
MainWindow::~MainWindow() {}

bool MainWindow::is_dark_mode() {
    #if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
        return QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;
    #else
        const QPalette defaultPalette;
        return defaultPalette.color(QPalette::WindowText).lightness() >
            defaultPalette.color(QPalette::Window).lightness();
    #endif
}

/**
 * Show info window.
 */
void MainWindow::show_info() {
    InfoWindow *info_window = new InfoWindow(this);
    info_window->show();
}

/**
 * Open dialog to select local path.
 * 
 * Sets selected path to path returned by dialog.
 */
void MainWindow::open_path_dialog() {
    QFileDialog *dialog = new QFileDialog();
    dialog->setOption(QFileDialog::ShowDirsOnly);
    dialog->setFileMode(QFileDialog::Directory);
    dialog->setDirectory(QDir::homePath());
    dialog->exec();
    if (dialog->result() == QDialog::Accepted && !dialog->selectedFiles().isEmpty())
        path_entry->setText(dialog->selectedFiles()[0]);
}

/**
 * Create LoC History and Chart
 */
void MainWindow::create_chart() {

    // Reset Progress Indicators

    start_button->setEnabled(false);
    section_label->setText("Starting...");
    timer_label->setText("0.0s");

    // Get Git Repo Path

    string git_repo_path = path_entry->text().toStdString();
    const bool cloning = git_repo_path.substr(0, 4).compare("http") == 0;

    // Show Local Path Warning

    if (
        settings_map["show_warnings"].compare("true") == 0 && !cloning &&
        settings_map["warn_local_path"].compare("true") == 0
    ) {
        int error = Definitions::set_config("warn_local_path", "false");
        if (error != 0) warn_set_config_error(error);
        settings_map = Definitions::get_config();
        if (
            QMessageBox::question(
                this,
                "Warning: Local Path Chosen",
                "While there are no known issues, "
                "it is recommended to use a fresh clone or to create a backup first. "
                "Continue?",
                QMessageBox::Yes|QMessageBox::No
            ) == QMessageBox::No
        ) {
            start_button->setEnabled(true);
            return;
        }
    }

    progress_bar->setValue(0);

    // Get Excluded Paths

    vector<string> excluded_paths;
    string entry_text = excluded_paths_entry->toPlainText().toStdString();
    entry_text.erase(std::remove(entry_text.begin(), entry_text.end(), '\"'), entry_text.end());
    istringstream ss(entry_text);
    string to;
    while (getline(ss, to, '\n')) if (to.length() != 0) excluded_paths.push_back(to);

    // Create LoC History

    long start = Definitions::get_time_ms();

    vector<Commit> commits;

    // Run Creation Function

    string branch = branch_entry->text().toStdString();
    bool cache_results =
        cache_this_check->isChecked() || (settings_map["cache_results"].compare("true") == 0);
    try {
        if (progress_check->isChecked()) {
            static function<void(double, long)> on_progress_func = bind(
                &MainWindow::on_progress, this, placeholders::_1, placeholders::_2
            );
            static function<void(string, long)> on_section_change_func = bind(
                &MainWindow::on_section_change, this, placeholders::_1, placeholders::_2
            );
            commits = create_loc_history(
                git_repo_path, excluded_paths, cloning, branch, cache_results,
                on_progress_func, on_section_change_func, start
            );
        } else {
            commits = create_loc_history(
                git_repo_path, excluded_paths, cloning, branch, cache_results,
                nullptr, nullptr, start
            );
        }
    } catch (const runtime_error &e) {
        // Show Error in Terminal and GUI
        cerr << e.what() << endl;
        QMessageBox::critical(this, "Error Calculating Lines of Code", e.what());
        start_button->setEnabled(true);
        return;
    }

    // Set Progress Indicators to Finished

    section_label->setText("Finished");
    progress_bar->setValue(100);
    update_timer(start);
    update_cache_size();

    // Create Chart

    // Create Last Commit Info

    const Commit &last_commit = commits[0];
    string last_commit_line = "Last Commit: " + to_string(last_commit.lines) + " LoC";
    for (auto &[lang, lines] : last_commit.language_map) {
        ostringstream ss;
        ss << fixed << setprecision(1) << double(lines) / last_commit.lines * 100;
        last_commit_line += ", " + lang.name + ": " + to_string(lines) + " LoC (" + ss.str() + "%)";
    }
    commit_info_label->setText(QString::fromStdString(last_commit_line));

    // Define Language Colors

    map<Language, QColor> language_colors = {
        {python, QColor::fromString("#0000AA")},
        {java, QColor::fromString("#AA0000")},
        {html, QColor::fromString("#DD4000")},
        {css, QColor::fromString("#600090")},
        {javascript, QColor::fromString("#DDAA00")},
        {typescript, QColor::fromString("#4040FF")},
        {c, QColor::fromString("#5050A0")},
        {cpp, QColor::fromString("#202040")},
        {c_sharp, QColor::fromString("#080820")},
        {go, QColor::fromString("#8080FF")},
        {rust, QColor::fromString("#FF8000")},
        {shell, QColor::fromString("#808080")}
    };

    // Axis Tick Interval Calculation Function

    auto calc_tick_interval = [this](size_t max_lines) {
        int max_ticks = chart_view->height() > 500 ? 10 : 5;
        array<int, 3> jumps = {1, 2, 5};
        int power = 1;
        int j = 0;
        while (max_lines / (jumps[j] * pow(10, power)) > max_ticks) {
            if (j == jumps.size() - 1) {
                j = 0;
                power++;
            } else {
                j++;
            }
        }
        return size_t(jumps[j] * pow(10, power));
    };

    // Create QChart

    QChart *chart = new QChart();
    chart_view->setBackgroundBrush(Qt::transparent);
    chart_view->setForegroundBrush(Qt::transparent);
    if (is_dark_mode())
        chart->setTheme(QChart::ChartTheme::ChartThemeDark);

    if (chart_type_combo->currentIndex() == 0) {

        // Stacked Line (Area) Chart

        // Get Project Languages

        vector<Language> project_languages = {};
        map<Language, QAreaSeries *> area_series_map;
        for (const Language &lang : languages) {
            bool lang_found = false;
            for (const Commit &commit : commits) {
                if (commit.language_map.find(lang) != commit.language_map.end()) {
                    lang_found = true;
                    break;
                }
            }
            if (lang_found) {
                project_languages.push_back(lang);
                QAreaSeries *area_series = new QAreaSeries();
                area_series->setLowerSeries(new QLineSeries());
                area_series->setUpperSeries(new QLineSeries());
                area_series->setName(QString::fromStdString(lang.name));
                area_series->setColor(language_colors[lang]);
                area_series->setBorderColor(Qt::transparent);
                area_series_map.emplace(lang, area_series);
            }
        }

        // Process Commits

        size_t max_lines = 0;

        for (const Commit &commit : commits) {
            if (commit.lines > max_lines) max_lines = commit.lines;
            size_t lower_lines = 0;
            qreal date = qint64(commit.date) * 1000;
            for (const Language &lang : project_languages) {
                size_t lines = (commit.language_map.find(lang) != commit.language_map.end()) ?
                    commit.language_map.at(lang) : 0;
                QAreaSeries *area_series = area_series_map[lang];
                area_series->lowerSeries()->append(date, lower_lines);
                area_series->upperSeries()->append(date, lower_lines + lines);
                lower_lines += lines;
            }
        }

        // Axes

        QDateTimeAxis *axis_x = new QDateTimeAxis();
        axis_x->setMin(QDateTime::fromSecsSinceEpoch(commits.back().date));
        axis_x->setMax(QDateTime::fromSecsSinceEpoch(commits.front().date));
        chart->addAxis(axis_x, Qt::AlignBottom);
        QValueAxis *axis_y = new QValueAxis();
        axis_y->setMin(0);
        axis_y->setMax(max_lines);
        axis_y->setTickType(QValueAxis::TicksDynamic);
        axis_y->setTickInterval(calc_tick_interval(max_lines));
        axis_y->setLabelFormat("%i");
        chart->addAxis(axis_y, Qt::AlignLeft);

        // Add Series to Chart

        for (auto &[lang, series] : area_series_map) {
            chart->addSeries(series);
            series->attachAxis(axis_x);
            series->attachAxis(axis_y);
        }

    } else {

        // Bar Chart
        // One bar per commit

        // Get Project Languages

        vector<Language> project_languages = {};
        map<Language, QBarSet *> bar_set_map;
        for (const Language &lang : languages) {
            bool lang_found = false;
            for (const Commit &commit : commits) {
                if (commit.language_map.find(lang) != commit.language_map.end()) {
                    lang_found = true;
                    break;
                }
            }
            if (lang_found) {
                project_languages.push_back(lang);
                QBarSet *bar_set = new QBarSet(QString::fromStdString(lang.name));
                bar_set->setColor(language_colors[lang]);
                bar_set->setBorderColor(Qt::transparent);
                bar_set_map.emplace(lang, bar_set);
            }
        }

        // Process Commits

        size_t max_lines = 0;

        for (size_t i = commits.size() - 1; i-- > 0; ) {
            const Commit &commit = commits[i];
            if (commit.lines > max_lines) max_lines = commit.lines;
            for (const Language &lang : project_languages)
                bar_set_map[lang]->append(
                    (commit.language_map.find(lang) != commit.language_map.end()) ?
                    commit.language_map.at(lang) : 0
                );
        }

        // Axes

        QValueAxis *axis_x = new QValueAxis();
        axis_x->setLabelFormat("%i");
        chart->addAxis(axis_x, Qt::AlignBottom);
        QValueAxis *axis_y = new QValueAxis();
        axis_y->setMin(0);
        axis_y->setMax(max_lines);
        axis_y->setTickType(QValueAxis::TicksDynamic);
        axis_y->setTickInterval(calc_tick_interval(max_lines));
        axis_y->setLabelFormat("%i");
        chart->addAxis(axis_y, Qt::AlignLeft);

        // Add to Chart

        QStackedBarSeries *bar_series = new QStackedBarSeries();
        bar_series->setBarWidth(1);
        for (auto &[lang, bar_set] : bar_set_map)
            bar_series->append(bar_set);
        chart->addSeries(bar_series);
        bar_series->attachAxis(axis_x);
        bar_series->attachAxis(axis_y);

    }

    // Set Chart View

    chart_view->setChart(chart);

    start_button->setEnabled(true);

}

/**
 * Update progress bar and timer.
 * 
 * @param progress Current progress 0-100.
 * @param start Milliseconds since epoch at calculation start.
 */
void MainWindow::on_progress(int progress, const long start) {
    update_timer(start);
    progress_bar->setValue(progress);
    progress_bar->update();
}

/**
 * Update the section title and timer.
 * 
 * @param section Current sections.
 * @param start Milliseconds since epoch at calculation start.
 */
void MainWindow::on_section_change(string section, const long start) {
    update_timer(start);
    section_label->setText(QString::fromStdString(section));
    section_label->update();
}

/** Update timer.
 * 
 * @param start Milliseconds since epoch at calculation start.
 */
void MainWindow::update_timer(const long start) {
    ostringstream ss;
    ss << fixed << setprecision(2) << double(Definitions::get_time_ms() - start) / 1000 << "s";
    timer_label->setText(QString::fromStdString(ss.str()));
    timer_label->update();
}

/**
 * Update displayed cache size.
 */
void MainWindow::update_cache_size() {
    cache_size_label->setText(
        QString::fromStdString("Cache Size: " + Definitions::get_cache_size())
    );
}

/**
 * Shows a warning when Definitions::set_config() returns an error.
 * 
 * See Defintions::set_config() for more info on possible errors.
 * 
 * @param error The code of the error to display.
 */
void MainWindow::warn_set_config_error(int error) {
    array<string, 3> errors = {
        "Error opening file " + Definitions::get_path_config() + " (read).",
        "Setting not found.",
        "Error opening file " + Definitions::get_path_config() + " (write)."
    };
    QMessageBox::warning(
        this,
        QString::fromStdString("Error Setting Config"), QString::fromStdString(errors[error - 1])
    );
}
