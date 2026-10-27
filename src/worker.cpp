// Includes

#include "worker.hpp"
#include "create-loc-history.hpp"

#include <QObject>

#include <string>
#include <vector>
using namespace std;


// Worker Functions

/**
 * Constructor
 */
Worker::Worker(QObject *parent) : QObject(parent) {}

// Slots

/**
 * Get commits using `create-loc-history`.
 * 
 * Get commits and emits `got_commits` with the resulting vector.
 * Parameters explained in `create-loc-history`.
 * Emits `warning` on runtime_error.
 */
void Worker::get_commits(
    string git_repo_path, vector<string> excluded_paths,
    const bool cloning, const string branch, const bool cache_results,
    function<void(int)> on_progress,
    function<void(string)> on_section_change,
    const long start
) {
    try {
        emit got_commits(create_loc_history(
            git_repo_path, excluded_paths, cloning, branch, cache_results,
            on_progress, on_section_change, start
        ));
    } catch (const runtime_error &e) {
        emit warning("Error Calculating Lines of Code", e.what());
    }
}