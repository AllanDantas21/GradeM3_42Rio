#include "exam.hpp"

void exam::fail_ex()
{
    current_ex->up_assignement();
    current_ex->set_time_bef_grade(time(NULL) + current_ex->grade_time() * 60);
    store_data();
}

void exam::success_ex(bool force)
{
    // insert current_ex in lvl_ex
    lvl_ex.insert(std::pair<int, exercise>(current_ex->get_lvl(), *current_ex));
    // insert the success exercise into file .system/exam_token/success_ex
    if (!force)
    {
        std::ofstream file;
        file.open("success/success_ex", std::ios::app);
        file << current_ex->get_name() << std::endl;
    }
    std::cout << std::endl
              << LIME << ">>>>>>>>>> SUCCESS <<<<<<<<<<" << RESET << std::endl
              << std::endl;
    std::string tmp;
    up_lvl();
    std::cout << "(Press enter to continue...)" << std::endl;
    std::string input;
    if (!std::getline(std::cin, input))
        sigd();
    level_per_ex += level_per_ex_save;
    changex = 0;
    backup = 0;
    if (!force)
    {
        if (file_exists("rendu/"))
        {
            if (!file_exists("success"))
                system("mkdir success");
            system("cp -r rendu/* success/ 2> /dev/null");
        }
    }
    if (level_per_ex > 100)
        end_exam();
    start_new_ex();
}

void exam::end_exam()
{
    remove(".system/exam_token/actuel_token.txt");
    if (using_cheatcode == 0)
        std::cout << WHITE << BOLD << "🥳 Congratulation! You have finished the Exam Rank 0" << exam_number << " !" << std::endl;
    else
        std::cout << WHITE << BOLD << "🙁 You have finished the Exam Rank 0" << exam_number << ", " << RED << BOLD << "after using " << using_cheatcode << " cheat command..." << WHITE << BOLD << std::endl;
    
    std::cout << "\n\e[1m\e[96mA word from the creator:\e[0m\n"
              << std::endl;
    std::cout << "This program has been created entirely \e[92mfor free\e[0m and \e[92mopen-source\e[0m.\n";
    std::cout << "\e[1mThank you for your support, and happy coding! ♥︎\e[0m\n"
              << std::endl;
    char c;
    std::cin >> c;
    std::cout << WHITE << BOLD << "Thanks for studying with us " << LIME << BOLD << username << WHITE << BOLD << " ❤️" << std::endl;
    exit(0);
}

// ==> GradeMe function call by entering `grademe` into prompt
void exam::grademe(void)
{
    if (file_exists(".system/grading/beta"))
    {
        std::cout << std::endl;
        std::cout << YELLOW << " ⚠️  Warning: " << RESET << "This exercise is a contribution by ";
        // output what is in .system/grading/beta
        std::ifstream file;
        file.open(".system/grading/beta");
        std::string line;
        std::getline(file, line);
        std::cout << YELLOW << line << RESET;
        std::cout << ", it's still in " << YELLOW << "beta testing" << RESET << "." << std::endl;
        std::cout << " If you want to add your contribution, visit the Github ReadME 👋" << std::endl;
        std::cout << " If you find any " << RED << "bug" << RESET << ", please report it on the Github repository." << std::endl;
    }

    std::cout << std::endl
              << "Before continuing, please make " << RED << "ABSOLUTELY SURE" << RESET << " that you are in the right directory," << std::endl;
    std::cout << "that you didn't forget anything, etc..." << std::endl;
    std::cout << "If your assignment is wrong, you will have the same assignment" << std::endl;
    std::cout << std::endl
              << " but with less potential points to earn !" << std::endl;
    std::cout << RED << "Are you sure?" << RESET << " [y/N] ";
    std::string input;
    if (!std::getline(std::cin, input))
        sigd();
    if (input == "y" || input == "Y")
    {
        if (current_ex->time_bef_grade > time(NULL) && waiting_time)
        {
            std::cout << RED << "ERROR: " << RESET << "You must wait at least " << YELLOW << BOLD;
            if ((((current_ex->time_bef_grade - time(NULL))) / 60) >= 1)
                std::cout << (current_ex->time_bef_grade - time(NULL)) / 60 << " minutes" << RESET << " and " << YELLOW << BOLD << (current_ex->time_bef_grade - time(NULL)) % 60 << " seconds" << RESET;
            else
                std::cout << (current_ex->time_bef_grade - time(NULL)) << " seconds" << RESET;
            std::cout << " until next grading request, so take your time to make more tests and be sure you will succeed next try!" << std::endl;
            return;
        }
        std::cout << "Ok, making grading request to server now." << std::endl;
        grade_request(0);
    }
    else
    {
        std::cout << " Abort" << std::endl;
    }
}

// ==> Function that call the bash grade system
void exam::grade_request(bool i)
{
    if (!i)
    {
        std::cout << std::endl
                  << "We will now wait for the job to complete." << std::endl
                  << "Please be " << LIME << "patient" << RESET << ", this " << LIME << "CAN" << RESET " take several minutes..." << std::endl;
        std::cout << "(10 seconds is fast, 30 seconds is expected, 3 minutes is a maximum)" << std::endl;
        srand(time(NULL));
        int waiting_seconds = rand() % 5 + 1;
        for (int i = 0; i < waiting_seconds; i++)
        {
            srand(time(NULL));
            double random = rand() % 6500000 + 250000;
            std::cout << "waiting..." << std::endl;
            usleep(random);
        }
    }

    if (!file_exists(".system/grading/tester.sh"))
    {
        std::cout << "Error: Unable to find grading script for this exercise, it's coming soon. You can use \"force_success\" to pass this ex." << std::endl;
        return;
    }

    system("bash .system/grading/tester.sh");

    if (file_exists(".system/grading/passed"))
    {
        success_ex(0);
    }
    else
    {
        std::cout << RED << ">>>>>>>>>> FAILURE <<<<<<<<<<" << RESET << std::endl;
        sleep(1);
        std::cout << "You have failed the assignment." << std::endl;

        // if there is a traceback file, create a folder traces and copy the file to it with the good name
        if (file_exists("traceback"))
        {
            system("mkdir -p traces");
            std::string trace_name = std::to_string(level) + "-" + std::to_string(current_ex->get_assignement()) + "_" + current_ex->get_name() + ".trace";
            std::string cmd_system_call = "mv traceback traces/" + trace_name;
            system(cmd_system_call.c_str());
            std::cout << "Trace saved to " << LIME << current_path() << "/traces/" << trace_name << RESET << std::endl
                      << std::endl;
        }
        else
        {
            std::cout << "No traceback found." << std::endl
                      << std::endl;
        }
        fail_ex();
        std::cout << "(Press enter to continue...)" << std::endl;
        std::string input;
        if (!std::getline(std::cin, input))
            sigd();
        if (vip)
            infovip();
        else
            info();
    }
}
