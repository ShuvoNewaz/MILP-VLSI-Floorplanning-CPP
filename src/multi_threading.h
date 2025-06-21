#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <atomic>

class ThreadPool
{
    public:
        ThreadPool(size_t num_threads);
        ~ThreadPool();

        // Submit a job and get a future result
        template<class F, class... Args>
        auto submit(F&& f, Args&&... args) 
            -> std::future<typename std::invoke_result<F, Args...>::type>;

    private:
        std::vector<std::thread> threads;
        std::queue<std::function<void()>> tasks;

        std::mutex queue_mutex;
        std::condition_variable condition;
        std::atomic<bool> stop;
};

// Constructor starts the threads
ThreadPool::ThreadPool(size_t num_threads) : stop(false)
{
    for (size_t i = 0; i < num_threads; i++)
    {
        // Push back a thread to the threads vector and initialize it
        threads.emplace_back([this]
        {
            while (true)
            {
                std::function<void()> task; // task placeholder to be run later
                {
                    std::unique_lock<std::mutex> lock(queue_mutex);

                    /*  Stay blocked until shutdown is initiated or there is more task to complete.
                        Task is added in the 'submit' method.
                        shutdown is initiated by the destructor.   */
                    condition.wait(lock, [this] { return stop || !tasks.empty(); });
                    if (stop && tasks.empty())
                    {
                        return; // Exit thread if shutdown is requested and there is no more task
                    }

                    // Remove completed task from list
                    task = std::move(tasks.front());
                    tasks.pop();
                }
                task(); // Run task
            }
        });
    }
}

// Destructor joins threads
ThreadPool::~ThreadPool()
{
    /* Tell all the worker threads that the pool is
    shutting down and no more tasks will be submitted.*/
    stop = true;
    condition.notify_all(); // Check constructor wait condition for all threads
    for (std::thread& t : threads)
        {
            t.join();
        }
}

/*  - Submits a job.
    - Wraps the function and arguments into a callable task.
    - Store the task in the shared task queue.
    - Return a future that will eventually hold the return value.   */

/* Variadic template to submit any function 'f'
with any number and types of argumets 'args'. */

/* Returns a 'std::future<T>' where 'T'
is the return type of calling 'f(args...)'. */
template<class F, class... Args>
auto ThreadPool::submit(F&& f, Args&&... args)
    -> std::future<typename std::invoke_result<F, Args...>::type>
{
    using return_type = typename std::invoke_result<F, Args...>::type;

    /*  - Create a lambda that binds 'f' and all its arguments by value.
        - Wrap that lambda into a 'std::packaged_task'.
        - Wrap the packaged task into a shared pointer 'std::make_shared'.  */
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        [f = std::forward<F>(f),
         ... args = std::forward<Args>(args)]() mutable {
            return std::invoke(std::move(f), std::move(args)...);
        }
    );

    std::future<return_type> res = task->get_future(); // Allows 'fut.get()' to wait for results.
    {
        std::lock_guard<std::mutex> lock(queue_mutex);
        if (stop) throw std::runtime_error("submit on stopped ThreadPool");

        /*  Place a lambda into the tasks queue
            to allow later invocation.  */
        tasks.emplace([task]() { (*task)(); });
    }
    condition.notify_one(); // Wake up a sleeping thread
    return res; // Return the future to the caller.
}