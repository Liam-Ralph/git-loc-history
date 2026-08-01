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

#include <QtGraphs/QGraphsTheme>
#include <QtQuickWidgets/QQuickWidget>

#include <array>
#include <atomic>
#include <chrono>


// MainWindow Class

class MainWindow : public QMainWindow {

    Q_OBJECT

    public:

        MainWindow();
        ~MainWindow();

    private:

        void show_info();
        void open_path_dialog();
        void create_graph();
        void update_timer();

        QLineEdit *path_entry;
        QTextEdit *excluded_paths_entry;

        QQuickWidget *graph_view;
        QGraphsTheme *graph_theme;

        QPushButton *start_button;
        QCheckBox *progress_check;
        QProgressBar *progress_bar;
        QLabel *section_label;
        QLabel *timer_label;

        std::clock_t start;
        std::array<std::atomic<int>, 6> *progress_ptr;

};


#endif
