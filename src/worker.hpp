#pragma once
#ifndef WORKER_HPP
#define WORKER_HPP


// Includes

#include "create-loc-history.hpp"

#include <QObject>

#include <string>
#include <vector>


// Worker Class

class Worker : public QObject {

    Q_OBJECT

    // Constructor

    public:
        Worker(QObject *parent = nullptr);

    // Signals

    signals:
        void got_commits(std::vector<Commit>);

    // Slots

    public slots:
        void get_commits(
            std::string git_repo_path, std::vector<std::string> excluded_paths,
            const bool cloning, const std::string branch, const bool cache_results,
            std::function<void(int)> on_progress,
            std::function<void(std::string)> on_section_change,
            const long start
        );

};


#endif
