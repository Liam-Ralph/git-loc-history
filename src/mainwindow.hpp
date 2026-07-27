#pragma once
#ifndef MAINWINDOW_H
#define MAINWINDOW_H


// Includes

#include <QCheckBox>
#include <QGraphicsView>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QProgressBar>

#include <array>
#include <atomic>
#include <chrono>


// MainWindow Class

class MainWindow : public QMainWindow {

    Q_OBJECT

    public:

        MainWindow();
        ~MainWindow();

        QLineEdit *path_entry;
        QTextEdit *excluded_paths_entry;
        QGraphicsView *results_view;
        QCheckBox *progress_check;
        QProgressBar *progress_bar;
        QLabel *timer_label;

        std::clock_t start;

    private:

        void show_info();
        void open_path_dialog();
        void create_graph();
        void progress_tracker( std::array<std::atomic<int>, 6> *progress_ptr, bool cloning);
        void update_timer();

};


#endif
