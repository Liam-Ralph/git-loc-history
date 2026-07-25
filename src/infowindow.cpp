// Includes

#include "definitions.hpp"
#include "infowindow.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

#include <QLabel>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QWidget>


// Functions

InfoWindow::InfoWindow(QWidget *parent) : QMainWindow(parent) {

    setWindowTitle("Git LoC History Info");

    // Create Window

    QWidget *window = new QWidget();
    QVBoxLayout *layout_back = new QVBoxLayout(window);
    layout_back->setAlignment(Qt::AlignHCenter);

    // Project Info

    static string project_info = []() {
        const string readme_path = Definitions::get_path_readme();
        ifstream file(readme_path);
        if (!file.is_open()) {
            string error = "Error opening file " + readme_path + ".";
            cerr << error << endl;
            return error;
        }
        string result = "";
        string line;
        for (int i = 0; i < 4; i++) {
            getline(file, line);
            if (i == 0) result += line.substr(2);
            else {
                result += line.substr(4);
                if (i < 4) result += "\n";
            }
        }
        file.close();
        return result;
    }();

    QLabel *project_info_label = new QLabel(QString::fromStdString(project_info));
    project_info_label->setWordWrap(true);
    project_info_label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    project_info_label->setAlignment(Qt::AlignHCenter);
    layout_back->addWidget(project_info_label);
    layout_back->addSpacerItem(new QSpacerItem(0, 5));

    // Credits

    QLabel *credits_label = new QLabel("Credits");
    QFont font = credits_label->font();
    font.setUnderline(true);
    credits_label->setFont(font);
    credits_label->setAlignment(Qt::AlignHCenter);
    layout_back->addWidget(credits_label);

    QLabel *credits = new QLabel(
        "Copyright (C) 2026 Liam Ralph. "
        "Logo modified from Git logo by Jason Long under the "
        "Creative Commons Attribution 3.0 Unported License. "
        "<a href=\"https://git-scm.com/community/logos\">Git Logo</a>"
    );
    credits->setFixedWidth(400);
    credits->setWordWrap(true);
    credits->setTextFormat(Qt::RichText);
    credits->setTextInteractionFlags(Qt::TextBrowserInteraction);
    credits->setOpenExternalLinks(true);
    credits->setAlignment(Qt::AlignHCenter);
    layout_back->addWidget(credits);
    layout_back->addSpacerItem(new QSpacerItem(0, 5));

    // Doc Viewer

    

    setCentralWidget(window);

}

InfoWindow::~InfoWindow() {}

string InfoWindow::read_file(string path) {
    ifstream file(path);
    if (!file.is_open()) {
        string error = "Error opening file " + path + ".";
        cerr << error << endl;
        return error;
    }
    ostringstream buffer;
    buffer << file.rdbuf();
    file.close();
    return buffer.str();
}
