#include "worker.hpp"
#include "create-loc-history.hpp"

#include <QThread>
#include <QObject>

Worker::Worker(QObject *parent) : QObject(parent) {}

void Worker::get_commits(
    std::string git_repo_path, std::vector<std::string> excluded_paths,
    const bool cloning, const std::string branch, const bool cache_results,
    std::function<void(int)> on_progress,
    std::function<void(std::string)> on_section_change,
    const long start
) {
    emit commits_finished(create_loc_history(
        git_repo_path, excluded_paths, cloning, branch, cache_results,
        on_progress, on_section_change, start
    ));
}