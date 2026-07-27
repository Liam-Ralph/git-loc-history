#pragma once
#ifndef MAINWINDOW_H
#define MAINWINDOW_H


// Includes

#include <QGraphicsView>
#include <QLineEdit>
#include <QMainWindow>


// MainWindow Class

class MainWindow : public QMainWindow {

    Q_OBJECT

    public:

        MainWindow();
        ~MainWindow();

        QLineEdit *path_entry;
        QGraphicsView *results_view;

    private:

        void show_info();
        void open_path_dialog();

};


#endif
