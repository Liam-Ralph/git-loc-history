#pragma once
#ifndef INFOWINDOW_H
#define INFOWINDOW_H


// Includes

#include <string>

#include <QMainWindow>
#include <QTextBrowser>


// InfoWindow Class

class InfoWindow : public QMainWindow {

    Q_OBJECT

    public:

        InfoWindow(QWidget *parent);
        ~InfoWindow();

    private:

        static std::string read_file(std::string path);
        void open_doc(char file, QTextBrowser **doc_viewer);

};


#endif
