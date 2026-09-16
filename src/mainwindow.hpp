#pragma once
#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP


// Includes

#include "create-loc-history.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QPushButton>

#include <QtCharts/QChartView>

#include <QtCore/QFutureWatcher>

#include <string>
#include <unordered_map>
#include <vector>


// MainWindow Class

class MainWindow : public QMainWindow {

    Q_OBJECT

    public:

        // Constructor/Destructor

        MainWindow();
        ~MainWindow();

    private:

        // Functions

        bool is_dark_mode();
        void show_info();
        void open_path_dialog();
        void get_commits();
        void create_chart();
        void update_timer();
        void update_cache_size();
        void warn_set_config_error(int error);

        // Variables

        std::unordered_map<std::string, std::string> settings_map;
        long start;
        std::vector<Commit> commits;

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

    signals:

        void progressed(int progress);
        void section_changed(std::string section);

    public slots:

        void on_progress(int progress);
        void on_section_change(std::string section);

};


#endif
