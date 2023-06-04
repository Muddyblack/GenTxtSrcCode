/**
 * @file main.cpp
 * @brief Main entry point for the Generating program.
 * This program generates C/C++ code from text input files in respect of the passed parameters.
 * This file mages all these parameters and helps the user to understand how all of this works.
 */

#include <iostream>
#include <unistd.h>
#include <getopt.h>
#include <regex>

#include <CTextToCPP.h>
#include <ProjectPathFinder.h>
#include <ConsoleColors.h>

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
    ProjectPathFinder pathFinder{"GenTxtSrcCode"}; // Added braces for initialization
    const std::string PROJECT_PATH = pathFinder.GetProjectFolderPath();

    // Standard directories
    std::string outputDir = PROJECT_PATH + "output\\"; /**< Output directory */
    std::string headerDir = outputDir + "include\\";   /**< Header file directory */
    std::string sourceDir = outputDir + "lib\\";       /**< Source file directory */
    std::string outputType = "CPP";                    /**< Output file type (C or CPP) */
    std::string outputFilename = "main";               /**< Output filename (without extension) */
    bool namespaceName;                                /**< Namespace yes or no (only for CPP) */
    int signPerLine = -1;                              /**< Number of characters per line */

    // Options
    struct option longOptions[9];

    /**
     * @brief Checks if a given path is a valid one for the current system
     *
     * @param path The path to be validated.
     * @return true if it is a correct path, false otherwise
     */
    void isValidPath(const std::string &opt_name, const std::string &path)
    {
        std::regex pathRegex;
#ifdef _WIN32
        // Windows path regex
        pathRegex = std::regex("^[a-zA-Z]:\\\\?[a-zA-Z0-9_\\-.\\s]*$");
#else
        // POSIX path regex
        pathRegex = std::regex("^/([a-zA-Z0-9_\\-.\\s]+/)*[a-zA-Z0-9_\\-.\\s]+$");
#endif

        if (std::regex_match(path, pathRegex) == false)
        {
            std::cout << ORANGE_COLOR << opt_name << ": " << BLUE_COLOR << path << ORANGE_COLOR << "\nThis is not a valid Path!" << RESET_COLOR << std::endl;
            exit(1);
        }
    }

    /**
     * @brief Clears the console screen
     */
    void clearConsole()
    {
        std::cout << "\033[2J\033[1;1H";
    }

    /**
     * @brief Prints the program arguments.
     */
    void printArguments()
    {
        std::cout << "Program arguments:\n";
        std::cout << "Output Directory: " << CYAN_COLOR << outputDir << RESET_COLOR << std::endl;
        std::cout << "Header Directory: " << CYAN_COLOR << headerDir << RESET_COLOR << std::endl;
        std::cout << "Source Directory: " << CYAN_COLOR << sourceDir << RESET_COLOR << std::endl;
        std::cout << "Output Type: " << CYAN_COLOR << outputType << RESET_COLOR << std::endl;
        std::cout << "Output Filename: " << CYAN_COLOR << outputFilename << RESET_COLOR << std::endl;
        std::cout << "Namespace Name: " << CYAN_COLOR << (namespaceName ? "Yes" : "No") << RESET_COLOR << std::endl;
        std::cout << "Sign Per Line: " << CYAN_COLOR << signPerLine << RESET_COLOR << std::endl;
        std::cout << std::endl;
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

        std::cout << "-O, --output-dir <dir>    " << BLUE_COLOR << "Output directory" << RESET_COLOR << "\n";
        std::cout << "-H, --headerdir <dir>     " << BLUE_COLOR << "Header file directory" << RESET_COLOR << "\n";
        std::cout << "-S, --sourcedir <dir>     " << BLUE_COLOR << "Source file directory" << RESET_COLOR << "\n";
        std::cout << "-t, --output-type <type>  " << BLUE_COLOR << "Output file type (C or CPP)" << RESET_COLOR << "\n";
        std::cout << "-f, --output-filename <name>  " << BLUE_COLOR << "Output filename (without extension)" << RESET_COLOR << "\n";
        std::cout << "-n, --namespace <name>        " << BLUE_COLOR << "Namespace yes/no" << RESET_COLOR << "\n";
        std::cout << "-l, --signperline <number>    " << BLUE_COLOR << "Number of characters per line" << RESET_COLOR << "\n";
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
     * @brief Parses the command-line options and sets the corresponding member variables.
     */
    void parseOptions()
    {
        int opt;
        while ((opt = getopt_long(argc, argv, "O:H:S:t:f:n:l:h", longOptions, nullptr)) != -1)
        {
            std::string long_opt = longOptions[opt].name;
            switch (opt)
            {
            case 'O':
                outputDir = optarg;
                isValidPath(long_opt, outputDir);
                break;
            case 'H':
                headerDir = optarg;
                isValidPath(long_opt, headerDir);
                break;
            case 'S':
                sourceDir = optarg;
                isValidPath(long_opt, sourceDir);
                break;
            case 't':
                outputType = optarg;
                break;
            case 'f':
                outputFilename = optarg;
                break;
            case 'n':
                namespaceName = optarg;
                break;
            case 'l':
                signPerLine = std::stoi(optarg);
                break;
            case 'h':
                printHelpText();
                exit(0);
            case '?':
                if ((optopt == 'O' || optopt == 'H' || optopt == 'S' || optopt == 't' || optopt == 'f' || optopt == 'n' || optopt == 'l' || optopt == 'h') && isprint(optopt))
                {
                    std::cout << ORANGE_COLOR << "OK ... option '-" << static_cast<char>(optopt) << "' without argument"
                              << RESET_COLOR << std::endl;
                    exit(1);
                }
                else if (isprint(optopt))
                {
                    std::cerr << RED_COLOR << "ERR ... Unknown option -" << static_cast<char>(optopt) << RESET_COLOR << std::endl;
                    exit(-1);
                }
                else
                {
                    std::cerr << RED_COLOR << "ERR ... Unknown option character \\x" << static_cast<char>(optopt) << RESET_COLOR << std::endl;
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
                CTextToCPP codeGenerator;
                for (int i = optind; i < argc; ++i)
                {
                    std::string inputFileName = argv[i];
                    codeGenerator.generateCode(inputFileName, outputDir, outputType);
                    std::cout << GREEN_COLOR << "Code generation successful for file: " << inputFileName << RESET_COLOR << std::endl;
                }
            }
            catch (const std::exception &e)
            {
                std::cerr << RED_COLOR << "Code generation failed: " << e.what() << RESET_COLOR << std::endl;
            }
        }
        else
        {
            std::cout << "Usage: program_name [options] input-file1 input-file2 ...\n";
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
        // Init the longOptions array
        longOptions[0] = {"output-dir", required_argument, nullptr, 'O'};
        longOptions[1] = {"headerdir", required_argument, nullptr, 'H'};
        longOptions[2] = {"sourcedir", required_argument, nullptr, 'S'};
        longOptions[3] = {"output-type", required_argument, nullptr, 't'};
        longOptions[4] = {"output-filename", required_argument, nullptr, 'f'};
        longOptions[5] = {"namespace", required_argument, nullptr, 'n'};
        longOptions[6] = {"signperline", required_argument, nullptr, 'l'};
        longOptions[7] = {"help", no_argument, nullptr, 'h'};
        longOptions[8] = {nullptr, 0, nullptr, 0};

        parseOptions();
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
    GenTxtSrcCode generator(argc, argv);
    // Rest of the program logic
    return 0;
}