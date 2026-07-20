#pragma once
#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP


// Includes

#include <QMainWindow>

class MainWindow : public QMainWindow {
    
    Q_OBJECT
    
    public :
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

};


#endif
