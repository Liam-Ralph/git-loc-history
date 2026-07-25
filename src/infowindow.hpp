#pragma once
#ifndef INFOWINDOW_H
#define INFOWINDOW_H


// Includes

#include <string>

#include <QMainWindow>


// InfoWindow Class

class InfoWindow : public QMainWindow {

    Q_OBJECT

    public:

        InfoWindow(QWidget *parent);
        ~InfoWindow();

    private:

        static std::string read_file(std::string path);

};


#endif
