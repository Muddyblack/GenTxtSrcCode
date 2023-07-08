/**
 * This program generates C/C++ code from text input files in respect of the passed parameters.
 * This file mages all these parameters and helps the user to understand how all of this works.
 *
 * @file main.cpp
 * @brief Main entry point for the Generating program.
 */

#include <iostream>
#include <unistd.h>
#include <getopt.h>
#include <regex>
#include <filesystem>

#include <cstdio>

#include <CTextToCPP.h>
#include <ProjectPathFinder.h>
#include <ConsoleColors.h>
#include <Logger.h>
#include <Parameter.h>

/**
 * @class GenTxtSrcCode
 * @brief Handles the generation of C/C++ code based on user input parameters.
 */
class GenTxtSrcCode
{
private:
    int argc;
    char **argv;

    // Project infomations
    const std::string PROJECT_NAME = "GenTxtSrcCode";
    ProjectPathFinder pathFinder; // Added braces for initialization
    std::string projName = "GenTxtSrcCode";
    const std::string PROJECT_PATH = pathFinder.getProjectFolderPath(projName);

    // Standard directories

    struct ParamStruct parameter;
    bool checkArgs = true;

    // Options
    const static int optionsAmount = 10;
    const struct option longOptions[optionsAmount] = {
        {"headerdir", required_argument, nullptr, 'H'},
        {"sourcedir", required_argument, nullptr, 'S'},
        {"outputtype", required_argument, nullptr, 't'},
        {"outputfilename", required_argument, nullptr, 'f'},
        {"namespace", required_argument, nullptr, 'n'},
        {"signperline", required_argument, nullptr, 'l'},
        {"check", no_argument, nullptr, 'C'},
        {"help", no_argument, nullptr, 'h'},
        {nullptr, 0, nullptr, 0}};

    /**
     * Checks and edits a given file path.
     *
     * @param path The file path to check and edit.
     * @return The edited file path.
     */
    std::string checkPath(const std::string &path)
    {
        std::filesystem::path fsPath(path);

        // Special cases: "." and "directiry" are considered relative paths
        if (fsPath == "." || fsPath == "directiry")
        {
            fsPath = std::filesystem::current_path();
        }
        else if (!fsPath.is_absolute())
        {
            fsPath = std::filesystem::absolute(fsPath);
        }

        std::string sanitizedPath = fsPath.string();

        // Replace forward slashes and single backslashes with double backslashes
        size_t found = sanitizedPath.find_first_of("/\\");
        while (found != std::string::npos)
        {
            sanitizedPath.replace(found, 1, "\\\\");
            found = sanitizedPath.find_first_of("/\\", found + 2);
        }

        return sanitizedPath;
    }

    /**
     * @brief Clears the console screen
     */
    void clearConsole()
    {
        std::cout << "\033[2J\033[1;1H";
    }

    /**
     * @brief Prints the help text with usage instructions and contact information.
     */
    void printHelpText()
    {
        std::cout << STRONG_GREEN_COLOR << " ______  ______  __   __  ______  __  __  ______  ______  ______  ______  ______  ______  _____   ______    \n";
        std::cout << "/\\  ___\\/\\  ___\\/\\ \"-.\\ \\/\\__  _\\/\\_\\_\\/\\__  _\\/\\  ___\\/\\  == \\\\/\\  ___\\/\\  ___\\/\\  __ \\\\/\\  __-./\\  ___\\   \n";
        std::cout << "\\ \\ \\__ \\ \\  __\\\\ \\ \\-.  \\/_/\\ \\/\\/_/\\_\\/\\ \\/\\ \\/\\___  \\ \\  __<\\ \\ \\___\\ \\ \\___\\ \\ \\/\\ \\ \\ \\/\\ \\ \\  __\\   \n";
        std::cout << " \\ \\_____\\ \\_____\\ \\_\\\\\"\\_\\ \\ \\_\\  /\\/_/\\_\\ \\ \\_\\ \\/____/\\ \\_\\ \\_\\ \\_____\\ \\_____\\ \\_____\\ \\____-\\ \\_____\\ \n";
        std::cout << "  \\/_____/\\/_____/\\/_/ \\/_/  \\/_/  \\/_/\\/_/  \\/_/  \\/_____/\\/_/ /_/\\/_____/\\/_____/\\/_____/\\/____/ \\/_____/ \n";
        std::cout << "                                                                                                           \n"
                  << RESET_COLOR << std::endl;

        std::cout << "Usage: program_name [options] input-file1 input-file2 ...\n\n";

        std::cout << "-H, --headerdir <dir>     " << BLUE_COLOR << "Header file directory" << RESET_COLOR << "\n";
        std::cout << "-S, --sourcedir <dir>     " << BLUE_COLOR << "Source file directory" << RESET_COLOR << "\n";
        std::cout << "-t, --outputtype <type>  " << BLUE_COLOR << "Output file type (C or CPP)" << RESET_COLOR << "\n";
        std::cout << "-f, --outputfilename <name>  " << BLUE_COLOR << "Output filename (without extension)" << RESET_COLOR << "\n";
        std::cout << "-n, --namespace <name>        " << BLUE_COLOR << "Flag to use namespaces" << RESET_COLOR << "\n";
        std::cout << "-l, --signperline <number>    " << BLUE_COLOR << "Number of characters per line" << RESET_COLOR << "\n";
        std::cout << "-C, --check                   " << BLUE_COLOR << "Flag to just create without checking the paths" << RESET_COLOR << "\n";
        std::cout << "-h, --help                    " << BLUE_COLOR << "Print help message" << RESET_COLOR << "\n";

        std::cout << GREEN_COLOR << "\n\n################################################################################\n";
        std::cout << "#                                                                              #\n";
        std::cout << "#                            Authors and Contact                               #\n";
        std::cout << "#                                                                              #\n";
        std::cout << "#          Authors: Anna-Sophie Schneider, Julia Egger, Jonas Lehmann,         #\n";
        std::cout << "#                         Christian Kerhault, Jamie Fisher                     #\n";
        std::cout << "#                                                                              #\n";
        std::cout << "#          Contact: kerhault.chris-it22.@it.dhbw-ravensburg.de                 #\n";
        std::cout << "#                                                                              #\n";
        std::cout << "################################################################################\n";
        std::cout << RESET_COLOR << std::endl;
    }

    /**
     * Determine the programming language from string.
     *
     * @param input The input string to check.
     * @return The detected programming language: "cpp" for C++, "c" for C
     */
    std::string checkLanguageType(const std::string &input)
    {
        if (input == "cpp" || input == "c++" || input == "g++")
        {
            return "cpp";
        }
        else if (input == "c")
        {
            return "c";
        }

        BOOST_LOG_TRIVIAL(fatal) << RED_COLOR << "Cannot deterimine: " << BLUE_COLOR << "'" << input << "'" << RED_COLOR << " as a Language."
                                 << "\n"
                                 << "We have " << CYAN_COLOR << "c " << RED_COLOR << "or " << CYAN_COLOR << "cpp " << RED_COLOR << "as option" << RESET_COLOR << std::endl;
        exit(1);
    }

    /**
     * Checks if the provided file name is valid.
     *
     * @param fileName The file name to be validated.
     * @return True if the file name is valid, false otherwise.
     */
    void isValidFileName(const std::string &fileName)
    {
        // Regular expression pattern for valid file name
        std::regex pattern(R"([^\x00-\x1F\x7F\\/:*?"<>|]+)");

        if (std::regex_match(fileName, pattern) == false)
        {
            BOOST_LOG_TRIVIAL(fatal) << BLUE_COLOR << fileName << RED_COLOR << " is not a valid fileName" << RESET_COLOR << std::endl;
            exit(1);
        }
    }

    void isValidNamespace(const std::string &ns)
    {
        // Regular expression pattern for valid C++ namespace
        std::regex pattern("^(::)?[a-zA-Z_][a-zA-Z0-9_]*(::[a-zA-Z_][a-zA-Z0-9_]*)*$");

        // Check if the string matches the pattern
        if (std::regex_match(ns, pattern) == false)
        {
            BOOST_LOG_TRIVIAL(fatal) << BLUE_COLOR << ns << RED_COLOR << " is not a valid namespace!" << RESET_COLOR << std::endl;
            exit(1);
        }
    }

    /**
     * @brief Parses the command-line options and sets the corresponding member variables.
     */
    void parseOptions()
    {
        int opt;
        int optionIndex;

        BOOST_LOG_TRIVIAL(info) << "Checking for User-Input";
        while ((opt = getopt_long(argc, argv, "H:S:t:f:n:l:Ch", longOptions, &optionIndex)) != -1)
        {
            std::string optionName;
            if (optionIndex > optionsAmount - 1 || optionIndex < 0)
            {
                optionName = "-";
                optionName += static_cast<char>(optopt);
            }
            else
            {
                optionName = "--";
                optionName += longOptions[optionIndex].name;
            }
            switch (opt)
            {
            case 'H':
                parameter.headerDir = checkPath(optarg);
                ;
                // isValidPath(optionName, headerDir);
                break;
            case 'S':
                parameter.sourceDir = checkPath(optarg);
                // isValidPath(optionName, sourceDir);
                break;
            case 't':
                parameter.outputType = checkLanguageType(optarg);
                break;
            case 'f':
                parameter.outputFilename = optarg;
                isValidFileName(parameter.outputFilename);
                break;
            case 'n':
                if (parameter.outputType == "cpp")
                {
                    parameter.namespaceName = optarg;
                    isValidNamespace(parameter.namespaceName);
                }
                break;
            case 'l':
                parameter.signPerLine = std::stoi(optarg);
                break;
            case 'C':
                checkArgs = false;
                break;
            case 'h':
                printHelpText();
                exit(0);
            case '?':
                if ((optopt == 'O' || optopt == 'H' || optopt == 'S' || optopt == 't' || optopt == 'f' || optopt == 'n' || optopt == 'l'))
                {
                    BOOST_LOG_TRIVIAL(fatal) << ORANGE_COLOR << "OK ... option " << optionName << "' without argument"
                                             << RESET_COLOR << std::endl;
                    exit(1);
                }
                else if (isprint(optopt))
                {
                    BOOST_LOG_TRIVIAL(fatal) << RED_COLOR << "ERR ... Unknown option: " << optionName << RESET_COLOR << std::endl;
                    exit(-1);
                }
                else
                {
                    BOOST_LOG_TRIVIAL(fatal) << RED_COLOR << "ERR ... Unknown option character \\x" << optionName << RESET_COLOR << std::endl;
                    exit(-1);
                }
            }
        }
    }

    /**
     * @brief Generates the code based on the parsed command-line options and input files.
     */
    void codeGeneration()
    {
        if (optind < argc)
        {
            try
            {
                for (int i = optind; i < argc; ++i)
                {
                    // This is where the magic happens
                    std::string inputFileName = argv[i];
                    std::string inputFilePath = PROJECT_PATH + "\\" + inputFileName;
                    CTextToCPP textToCPP(PROJECT_PATH, inputFilePath, parameter);
                    textToCPP.generateCode();

                    BOOST_LOG_TRIVIAL(info) << GREEN_COLOR << "Code generation successful for file: " << inputFileName << RESET_COLOR << std::endl;
                }
            }
            catch (const std::exception &e)
            {
                BOOST_LOG_TRIVIAL(error) << RED_COLOR << "Code generation failed: " << e.what() << RESET_COLOR << std::endl;
            }
        }
        else
        {
            BOOST_LOG_TRIVIAL(warning) << RED_COLOR << "Usage: program_name [options] input-file1 input-file2 ..." << RESET_COLOR << std::endl;
        }
    }

public:
    /**
     * @brief Constructor for the GenTxtSrcCode class.
     * @param argc The number of command-line arguments.
     * @param argv The array of command-line arguments.
     */
    GenTxtSrcCode(int argc, char *argv[]) : argc(argc), argv(argv)
    {
        setup_logging(PROJECT_PATH + "/GenTxtSrcCode.log");

        BOOST_LOG_TRIVIAL(info) << "Starting Programm";
        parseOptions();
        if (checkArgs == true)
        {
            printParamStruct(parameter);
            std::cout << GREEN_COLOR << "Press any key to continue..." << RESET_COLOR << std::endl;
            getchar(); // Wait for any key
        }
        codeGeneration();
    }
};

/**
 * @brief The main entry point for the program.
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 * @return The exit code of the program.
 */
int main(int argc, char *argv[])
{
    EnableConsoleColors();
    GenTxtSrcCode generator(argc, argv);
    return 0;
}