#pragma once
#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP


// Includes

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QProgressBar>
#include <QPushButton>
#include <QTextEdit>

#include <QtCharts/QChartView>

#include <array>
#include <atomic>
#include <ctime>
#include <string>
#include <unordered_map>


// MainWindow Class

class MainWindow : public QMainWindow {

    Q_OBJECT

    public:

        MainWindow();
        ~MainWindow();

        void on_progress(int progress, std::clock_t start);
        void on_section_change(std::string section, std::clock_t start);

    private:

        static bool is_dark_mode();
        void show_info();
        void open_path_dialog();
        void create_graph();
        void update_timer(std::clock_t start);
        void update_cache_size();

        std::unordered_map<std::string, std::string> settings_map;

        QLabel *commit_info_label;

        QLineEdit *path_entry;
        QTextEdit *excluded_paths_entry;

        QChartView *chart_view;
        QCheckBox *progress_check;
        QComboBox *chart_type_combo;
        QLabel *cache_size_label;

        QPushButton *start_button;
        QProgressBar *progress_bar;
        QLabel *section_label;
        QLabel *timer_label;

};


#endif
