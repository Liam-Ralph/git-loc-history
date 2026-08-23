// Includes

#include "infowindow.hpp"
#include "definitions.hpp"

#include <QLabel>
#include <QPushButton>
#include <QScreen>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QWidget>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;


// Functions

/**
 * Constructor
 * 
 * @param parent MainWindow, the parent widget.
 */
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
            if (i == 0) result += line.substr(2); // Git Loc History
            else result += line.substr(3); // Released, Version, Updated
            if (i < 3) result += "\n";
        }
        file.close();
        return result;
    }();

    QLabel *project_info_label = new QLabel(QString::fromStdString(project_info));
    project_info_label->setWordWrap(true);
    project_info_label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    project_info_label->setAlignment(Qt::AlignHCenter);
    layout_back->addWidget(project_info_label);

    // Credits

    QLabel *credits_title = new QLabel("Credits");
    QFont font = credits_title->font();
    font.setUnderline(true);
    credits_title->setFont(font);
    credits_title->setAlignment(Qt::AlignHCenter);
    layout_back->addWidget(credits_title);

    QLabel *credits_label = new QLabel(
        "Copyright (C) 2026 Liam Ralph.<br>"
        "Logo modified from Git logo by Jason Long under the<br>"
        "Creative Commons Attribution 3.0 Unported License.<br>"
        "<a href=\"https://git-scm.com/community/logos\">Git Logo Website</a>"
    );
    credits_label->setTextFormat(Qt::RichText);
    credits_label->setTextInteractionFlags(Qt::TextBrowserInteraction);
    credits_label->setOpenExternalLinks(true);
    credits_label->setAlignment(Qt::AlignHCenter);
    layout_back->addWidget(credits_label);

    // License

    QLabel *license_title = new QLabel("License");
    font = license_title->font();
    font.setUnderline(true);
    license_title->setFont(font);
    license_title->setAlignment(Qt::AlignHCenter);
    layout_back->addWidget(license_title);

    QLabel *license_label = new QLabel(QString::fromStdString(
        "This project is licensed under the MIT/Expat License.<br>"
        "This license can be found in the following locations:<br>"
        "<a href=\"" + Definitions::get_path_license() + "\">Local Copy</a><br>"
        "<a href=\"https://github.com/liam-ralph/git-loc-history/blob/main/LICENSE\">"
        "GitHub Repo (Official)</a><br>"
        "<a href=\"https://mit-license.org/\">MIT License Website</a><br>"
    ));
    license_label->setTextFormat(Qt::RichText);
    license_label->setTextInteractionFlags(Qt::TextBrowserInteraction);
    license_label->setOpenExternalLinks(true);
    license_label->setAlignment(Qt::AlignHCenter);
    layout_back->addWidget(license_label);

    // Doc Viewer

    QHBoxLayout *layout_buttons = new QHBoxLayout();
    QPushButton *view_readme = new QPushButton("View README.md");
    view_readme->setObjectName("view_readme");
    layout_buttons->addWidget(view_readme);
    QPushButton *view_changelog = new QPushButton("View CHANGELOG.md");
    view_changelog->setObjectName("view_changelog");
    layout_buttons->addWidget(view_changelog);
    QPushButton *view_license = new QPushButton("View LICENSE");
    view_license->setObjectName("view_license");
    layout_buttons->addWidget(view_license);
    layout_back->addLayout(layout_buttons);

    doc_viewer = new QTextBrowser();
    doc_viewer->setReadOnly(true);
    doc_viewer->setTextInteractionFlags(Qt::TextBrowserInteraction);
    doc_viewer->setOpenExternalLinks(true);
    doc_viewer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout_back->addWidget(doc_viewer);

    connect(view_readme, &QPushButton::clicked, this, &InfoWindow::open_doc);
    connect(view_changelog, &QPushButton::clicked, this, &InfoWindow::open_doc);
    connect(view_license, &QPushButton::clicked, this, &InfoWindow::open_doc);

    setCentralWidget(window);

}

/**
 * Deconstructor
 */
InfoWindow::~InfoWindow() {}

/**
 * Read a documentation file.
 * 
 * @param path Path to file.
 * @return File contents, or error message if file not opened.
 */
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

/**
 * Read and display a documentation file.
 * 
 * Used as a slot for documentation viewer buttons.
 */
void InfoWindow::open_doc() {

    resize(this->width(), qMin(800, this->screen()->availableGeometry().height()));

    QObject *button = QObject::sender();
    string contents;

    // Read and Display Documentation File

    if (button->objectName() == "view_readme") {
        static string readme_contents = read_file(Definitions::get_path_readme());
        contents = readme_contents;
        doc_viewer->setMarkdown(QString::fromStdString(contents));
    } else if (button->objectName() == "view_changelog") {
        static string changelog_contents =
            read_file(Definitions::get_path_doc() + "/CHANGELOG.md");
        contents = changelog_contents;
        doc_viewer->setMarkdown(QString::fromStdString(contents));
    } else if (button->objectName() == "view_license") {
        static string license_contents = read_file(Definitions::get_path_license());
        contents = license_contents;
        doc_viewer->setMarkdown("");
        doc_viewer->setText(QString::fromStdString(contents));
    }

}
