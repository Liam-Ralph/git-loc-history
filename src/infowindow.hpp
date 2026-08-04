#pragma once
#ifndef INFOWINDOW_HPP
#define INFOWINDOW_HPP


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

    private:

        std::string read_file(std::string path);
        void open_doc();

        QTextBrowser *doc_viewer;

};


#endif
