#pragma once
#ifndef INFOWINDOW_H
#define INFOWINDOW_H


// Includes

#include <QMainWindow>
#include <QTextBrowser>

#include <string>


// InfoWindow Class

class InfoWindow : public QMainWindow {

    Q_OBJECT

    public:

        InfoWindow(QWidget *parent);
        ~InfoWindow();

        QTextBrowser *doc_viewer;

    private:

        static std::string read_file(std::string path);
        void open_doc();

};


#endif
