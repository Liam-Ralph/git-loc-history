#include <QObject>
#include <vector>
#include "create-loc-history.hpp"

class Worker : public QObject {
    Q_OBJECT
    public:
        Worker(QObject *parent = nullptr);
    public slots:
        void get_commits(
            std::string git_repo_path, std::vector<std::string> excluded_paths,
            const bool cloning, const std::string branch, const bool cache_results,
            std::function<void(int)> on_progress,
            std::function<void(std::string)> on_section_change,
            const long start
        );
    signals:
        void commits_finished(std::vector<Commit>);
};
