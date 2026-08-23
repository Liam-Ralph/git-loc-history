#pragma once
#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP


// Includes

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QPushButton>

#include <QtCharts/QChartView>

#include <string>
#include <unordered_map>


// MainWindow Class

class MainWindow : public QMainWindow {

    Q_OBJECT

    public:

        // Constructor/Destructor

        MainWindow();
        ~MainWindow();

        // Functions

        void on_progress(int progress, long start);
        void on_section_change(std::string section, long start);

    private:

        // Functions

        static bool is_dark_mode();
        void show_info();
        void open_path_dialog();
        void create_chart();
        void update_timer(long start);
        void update_cache_size();
        void warn_set_config_error(int error);

        // Variables

        std::unordered_map<std::string, std::string> settings_map;

        QLabel *commit_info_label;

        QLineEdit *path_entry;
        QPlainTextEdit *excluded_paths_entry;

        QChartView *chart_view;
        QLineEdit *branch_entry;
        QCheckBox *progress_check;
        QCheckBox *cache_this_check;
        QComboBox *chart_type_combo;
        QLabel *cache_size_label;

        QPushButton *start_button;
        QProgressBar *progress_bar;
        QLabel *section_label;
        QLabel *timer_label;

};


#endif
