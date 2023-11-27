#include "logger.hpp"

#include <args.hxx>
#include <csignal>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <thread>

class Main {
public:
  Main(std::string filename)
      : stream{std::ofstream{filename, std::ios::trunc | std::ios::out}},
        logger{new Logger(stream)} {}

  ~Main() {
    delete logger;
    stream.close();
  }

private:
  std::ofstream stream;

public:
  Logger *logger;
};

static Main *_main = nullptr;

void signal_handler(int s) {
  std::cout << "Caught signal " << s << std::endl;
  delete _main;
  std::exit(s);
}

int main(int argc, char **argv) {
  args::ArgumentParser parser(
      "Provide a log file name and see new timestamp every second");
  args::Positional<std::string> filename(
      parser, "filename", "Enter the filename and path to test logger");
  args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
  args::CompletionFlag completion(parser, {"complete"});
  try {
    parser.ParseCLI(argc, argv);
  } catch (const args::Completion &e) {
    std::cout << e.what();
    return 0;
  } catch (const args::Help &) {
    std::cout << parser;
    return 0;
  } catch (const args::ParseError &e) {
    std::cerr << e.what() << std::endl;
    std::cerr << parser;
    return 1;
  }

  if (filename) {
    auto file = args::get(filename);
    std::cout << "Check " << file << " for heartbeat" << std::endl;
    _main = new Main(file);
    signal(SIGINT, signal_handler);
    while (1) {
      // timestamp:
      auto t = std::time(nullptr);
      auto tm = *std::localtime(&t);
      
      *(_main->logger) << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << "\n";
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
  }

  return 0;
}
