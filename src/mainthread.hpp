

// Includes

#include <QThread>

#include <functional>
#include <string>
#include <vector>


// MainThread Class

class MainThread : public QThread {

    Q_OBJECT

    public:
        void run(
            std::string git_repo_path, std::vector<std::string> excluded_paths,
            const bool cloning, const std::string branch, const bool cache_results,
            std::function<void(int, long)> on_progress,
            std::function<void(std::string, long)> on_section_change,
            const long start
        )

}