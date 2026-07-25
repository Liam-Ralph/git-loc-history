#pragma once
#ifndef MAINWINDOW_H
#define MAINWINDOW_H


// Includes

#include <QMainWindow>


// MainWindow Class

class MainWindow : public QMainWindow {

    Q_OBJECT

    public :

        MainWindow();
        ~MainWindow();
    
    private:

        void show_info();

};


#endif
