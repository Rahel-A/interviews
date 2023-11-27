#include <memory>
#include <mutex>
#include <ostream>

/*! \mainpage See \ref Logger for main documentation
 */

/*! \class Logger
 * \brief Logger can output messages to many different types of outputs.
 *
 * Generate an instance for each output type such as console output:
 * ```
 * #include <iostream>
 * Logger logger{std::cout};
 * ```
 *
 * or to a file:
 * ```
 * auto stream{std::ofstream{filename, std::ios::trunc | std::ios::out }};
 * Logger logger{stream}; // Takes a ref, ensure stream is not deleted.
 * ```
 *
 * Logging is done with operator<<, such as:
 * ```
 * std::string my_name{"Bob"};
 * logger << "Hi " << my_name << "!" << std::endl;
 * ```
 */
class Logger {
public:
  Logger() = delete;
  /*! \brief Create a multi-threaded safe object for logging messages.
   */
  Logger(std::ostream &output) : m_out{output} {}

  /** \brief Output standard c++ types.
   *
   * To log/output non standard types, declare this class as a friend and
   * format the output to the desired log line, see \ref custom_types
   * for an example on how to log other non-standard types.
   * \param[in] output A basic value to write to output
   */
  template <typename T> Logger &operator<<(const T &output) {

    /* Mutex for \ref m_out stream.
     * We can instantiate as many loggers as we want for each output stream,
     * therefore we use a global static mutex, so that only 1 logger is
     * activate at any time.
     * alt solution - map each unique output stream to a mutex which is
     * possible with files (filename == id) but unsure how to differentiate
     * std::cout...
     */
    static std::mutex m_out_mutex;
    std::lock_guard<std::mutex> guard(m_out_mutex);

    if (m_out.good()) {
      m_out << output;
      m_out.flush(); // \todo maybe flush should be done by user??
    }
    // \todo throw to indicate failure?

    return *this;
  }

private:
  std::ostream &m_out; ///!< Output stream for this logger:
                       /// file/console/or other streams...

  /** \todo: maybe provide static instances for non-initialized standard logs,
   * such as console logs.
   * \todo: could add timestamps to each log entry.
   * \todo: create a builder class? that statically owns the streams? Also would
   * help move away from global mutex and allow multiple threads to run each
   * logger
   */
};
