#pragma once
#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP


// Includes

#include <QCheckBox>
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


// MainWindow Class

class MainWindow : public QMainWindow {

    Q_OBJECT

    public:

        MainWindow();
        ~MainWindow();

        void on_progress(double progress, std::clock_t start);
        void on_section_change(std::string section, std::clock_t start);

    private:

        static bool is_dark_mode();
        void show_info();
        void open_path_dialog();
        void create_graph();
        void update_timer(std::clock_t start);

        QLineEdit *path_entry;
        QTextEdit *excluded_paths_entry;

        QChartView *chart_view;

        QPushButton *start_button;
        QCheckBox *progress_check;
        QProgressBar *progress_bar;
        QLabel *section_label;
        QLabel *timer_label;

};


#endif
