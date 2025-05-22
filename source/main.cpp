#include "scrapper.hpp"
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <csignal>
#include <vector>
#include <atomic>
#include <mutex>
#include <condition_variable>

constexpr int MAX_PROCESSES = 8;
std::atomic<int> active_processes(0);
std::mutex process_mutex;
std::condition_variable process_cv;

bool has_extension(const std::string& filename, const std::string& extension)
{
    return filename.size() >= 4 && filename.compare(filename.size() - 4, 4, std::string(".") + extension) == 0;
}

// Spawn new visit in child process with concurrency limit
void spawn_visit(const std::string& url, const std::string& domain, const std::string& download_path)
{
    std::unique_lock<std::mutex> lock(process_mutex);
    process_cv.wait(lock, [] { return active_processes < MAX_PROCESSES; });

    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        visit(url, domain, download_path);
        _exit(0);  // Always use _exit after fork in child
    }
    else if (pid > 0) {
        // Parent process
        active_processes++;
        lock.unlock();

        // Launch a detached thread to wait for this child
        std::thread([pid]() {
            int status;
            waitpid(pid, &status, 0);
            {
                std::lock_guard<std::mutex> lock(process_mutex);
                active_processes--;
            }
            process_cv.notify_one();
        }).detach();
    }
    else {
        std::cerr << "Failed to fork.\n";
    }
}

void visit(const std::string& url, const std::string& domain, const std::string& download_path)
{
    reload::scrap::scrapper s;

    std::cout << "Visiting: " << url << "\n";

    std::string html = s.request(url);
    reload::scrap::document doc = s.parse(html);

    auto v = reload::scrap::node::find(doc.root, "a");
    for (auto e : v)
    {
        std::string href = reload::scrap::node::get_attribute_value(e, "href");
        if (href.empty())
            continue;

        // Ignore "./" and "../" links
        if (href.ends_with("./") || href.ends_with("../"))
            continue;

        std::cout << "Found link: " << href << "\n";

        if (has_extension(href, "pdf"))
        {
            std::string full_url = (href.starts_with("http://") || href.starts_with("https://"))
                ? href
                : url + "/" + href;

            if (full_url.starts_with(domain))
            {
                bool success = s.download_to_file(full_url, download_path, domain);
                if (success)
                    std::cout << "File downloaded: " << href << "\n";
                else
                    std::cout << "Error downloading file: " << href << "\n";
            }
        }
        else
        {
            if (!href.ends_with("/"))
                continue;

            std::string next_url = href;
            if (!(href.starts_with("http://") || href.starts_with("https://")))
            {
                std::string base = url;
                if (!base.empty() && base.back() == '/')
                    base.pop_back();
                if (!href.empty() && href.front() == '/')
                    href.erase(0, 1);

                next_url = base + "/" + href;
            }

            if (next_url.starts_with(domain)) {
                spawn_visit(next_url, domain, download_path);  // 🔁 Forked visit
            }
        }
    }
}

int main(int argc, char** argv)
{
    visit("https://the-eye.eu/public/Site-Dumps/campdivision.com/camp/Text%20Files/PDF/",
          "https://the-eye.eu",
          "/home/shared/Documents/Scrapped");

    // Wait for all children to finish
    while (active_processes > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}
