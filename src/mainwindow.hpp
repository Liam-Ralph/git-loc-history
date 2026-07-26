#pragma once
#ifndef MAINWINDOW_H
#define MAINWINDOW_H


// Includes

#include <QLineEdit>
#include <QMainWindow>


// MainWindow Class

class MainWindow : public QMainWindow {

    Q_OBJECT

    public:

        MainWindow();
        ~MainWindow();
    
    private:

        void show_info();
        void open_path_dialog(QLineEdit *path_entry);

};


#endif
