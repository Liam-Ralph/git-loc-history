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

        // Constructor/Destructor

        InfoWindow(QWidget *parent);
        ~InfoWindow();

    private:

        // Functions

        std::string read_file(std::string path);
        void open_doc();

        // Variables

        QTextBrowser *doc_viewer;

};


#endif
