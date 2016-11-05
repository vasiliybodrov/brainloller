// *****************************************************************************
// Program: Brainloller - interpretator of Brainfuck/Brainloller
// Filename: main.cpp
// Description: The implementation of interpretator of Brailoller/Brainfuck
// Note:        This code uses the Boost Graph Library and OpenCV.
//              I want to use the OpenCV, Boost Library and STL only in C++ style.
//              However, also I use native Linux (BSD, POSIX etc) libraries.
// Programmers: Vasiliy V. Bodrov aka Bodro and Denis Evgrafov
// Date: the 31-th of October, 2016 year
// *****************************************************************************
// The MIT License (MIT)
//
// Copyright (c) 2016 Vasiliy V. Bodrov aka Bodro and Denis Evgrafov, Ryazan, Russia
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
// OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
// THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// *****************************************************************************

// Includes: standard C-headers
#include <cstdlib>
#include <cassert>
#include <cerrno>
#include <cstring>

// Includes: standard C++-headers
#include <iostream>
#include <sstream>
#include <ios>
#include <iomanip>
#include <new>
#include <memory>
#include <string>
#include <map>
#include <vector>
#include <stack>
#include <iterator>
#include <algorithm>
#include <exception>
#include <functional>

// Includes: Boost C++-headers (different)
#include <boost/cstdint.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>

// Includes: OpenCV C++-headers
#include <opencv2/opencv.hpp>

// Include system C/C++-headers
#include <getopt.h>
#include <unistd.h>

int main(int argc, char** argv);

#ifndef UNUSED_SUPPRESSOR
#define UNUSED_SUPPRESSOR(x) do { utils::__unused_suppressor__(x); } while(0)
#else // UNUSED_SUPPRESSOR
#error ERROR! Redefenition macros UNUSED_SUPPRESSOR!
#endif // UNUSED_SUPPRESSOR

namespace {
	// *************************************************************************
	// Utilities' classes and namespaces
	// *************************************************************************
	namespace utils {
		template<typename T>
		void __unused_suppressor__(T& x) { (void) x; }
		
		template<typename T>
		void __unused_suppressor__(T const& x) { (void) x; }
	} // namespace utils

	// *************************************************************************
	// Common constants
	// *************************************************************************

	std::string const PRINTER_TYPE_NULL = std::string("null");
	std::string const PRINTER_TYPE_SIMPLE = std::string("simple");
	std::string const PRINTER_TYPE_SIMPLE_SPACE = std::string("simplespace");
	std::string const PRINTER_TYPE_CLASSIC_BRAINFUCK = std::string("brainfuck");

	std::string const OUTPUT_TYPE_NULL = std::string("null");
	std::string const OUTPUT_TYPE_CHAR = std::string("char");
	std::string const OUTPUT_TYPE_HEX = std::string("hex");
	std::string const OUTPUT_TYPE_BOTH = std::string("both");
	
	std::string const LANGUAGE_BRAINLOLLER = std::string("brainloller");
	std::string const LANGUAGE_BRAINFUCK = std::string("brainfuck");

	// *************************************************************************
	// Functions etc for program's arguments
	// *************************************************************************
	
	void usage(void) noexcept;
	void help() noexcept;
	void license() noexcept;
	void authors() noexcept;
	void version() noexcept;

	const char* PROG_NAME = "brainloller";
	const char* PROG_NAME_FULL = "Interpretator of Brainfuck/Brainloller";
	const char* PROG_VERSION = "0.7";
	const char* PROG_AUTHORS = "Vasiliy V. Bodrov aka Bodro (bodro-mail@list.ru) and Denis Evgrafov";
	const char* PROG_LICENSE = ""		\
    "The MIT License (MIT)\n"\
    "\n"\
    "Copyright (c) 2016 Vasiliy V. Bodrov aka Bodro and Denis Evgrafov, Ryazan, Russia\n"\
    "\n"\
    "Permission is hereby granted, free of charge, to any person obtaining a\n"\
    "copy of this software and associated documentation files (the \"Software\"),\n"\
    "Software is furnished to do so, subject to the following conditions:\n"\
    "\n"\
    "The above copyright notice and this permission notice shall be included\n"\
    "in all copies or substantial portions of the Software.\n"\
    "\n"\
    "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS\n"\
    "OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF\n"\
    "MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.\n"\
    "IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY\n"\
    "CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT\n"\
    "OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR\n"\
    "THE USE OR OTHER DEALINGS IN THE SOFTWARE.\n";

	struct configuration {
	public:
		/* Fields (direct access) */
		int global_argc;
		char const* const* global_argv;
		int flag_show_help;
		int flag_show_version;
		int flag_show_authors;
		int flag_show_license;
		int flag_show_variable;
		int flag_show_variable_stop;
		std::string filename;
		std::string printer_type;
		std::string output_type;
		std::string language;
		int flag_run;
		int flag_print;
		std::list<std::string> operands;

		/* Methods */
		inline void set_flag_show_help(char const* value) {
			this->flag_show_help = boost::lexical_cast<int>(value);
		}
		inline void set_flag_show_version(char const* value) {
			this->flag_show_version = boost::lexical_cast<int>(value);
		}
		inline void set_flag_show_authors(char const* value) {
			this->flag_show_authors = boost::lexical_cast<int>(value);
		}
		inline void set_flag_show_license(char const* value) {
			this->flag_show_license = boost::lexical_cast<int>(value);
		}
		inline void set_flag_show_variable(char const* value) {
			this->flag_show_variable = boost::lexical_cast<int>(value);
		}
		inline void set_flag_show_variable_stop(char const* value) {
			this->flag_show_variable_stop = boost::lexical_cast<int>(value);
		}
		inline void set_filename(char const* value) {
			this->filename = boost::lexical_cast<std::string>(value);
		}
		inline void set_printer_type(char const* value) {
			this->printer_type = boost::lexical_cast<std::string>(value);
		}
		inline void set_output_type(char const* value) {
			this->output_type = boost::lexical_cast<std::string>(value);
		}
		inline void set_language(char const* value) {
			this->language = boost::lexical_cast<std::string>(value);
		}
		inline void set_flag_run(char const* value) {
			this->flag_run = boost::lexical_cast<int>(value);
		}
		inline void set_flag_print(char const* value) {
			this->flag_print = boost::lexical_cast<int>(value);
		}
		inline void set_operands(char const* value) {
		std::istringstream iss(value);
		std::copy(std::istream_iterator<std::string>(iss),
				  std::istream_iterator<std::string>(),
				  std::back_inserter(this->operands));
		}
		inline configuration(void) :
			global_argc(0),
			global_argv(nullptr),
			flag_show_help(0),
			flag_show_version(0),
			flag_show_authors(0),
			flag_show_license(0),
			flag_show_variable(0),
			flag_show_variable_stop(0),
			filename(),
			printer_type(PRINTER_TYPE_CLASSIC_BRAINFUCK),
			output_type(OUTPUT_TYPE_CHAR),
			language(LANGUAGE_BRAINLOLLER),
			flag_run(1),
			flag_print(0),
			operands()
			{}
		inline ~configuration(void) {
			this->global_argc = 0;
			this->global_argv = nullptr;
			this->flag_show_help = 0;
			this->flag_show_version = 0;
			this->flag_show_authors = 0;
			this->flag_show_license = 0;
			this->flag_show_variable = 0;
			this->flag_show_variable_stop = 0;
			this->filename.clear();
			this->printer_type.clear();
			this->output_type.clear();
			this->language.clear();
			this->flag_run = 0;
			this->flag_print = 0;
			this->operands.clear();
		}
	};
		
	typedef struct environment_names_s {
		const char* name;
		std::function<void(char const*)> setter;
	} environment_names_t;

	extern char** environ;

	configuration config;

	option longopts[] = {
		{"help",                no_argument,       &config.flag_show_help,          0x01}, // 'h'
		{"version",             no_argument,       &config.flag_show_version,       0x01}, // 'v'
		{"authors",             no_argument,       &config.flag_show_authors,       0x01}, // none
		{"license",             no_argument,       &config.flag_show_license,       0x01}, // 'l'
		{"file",                required_argument, 0,                               'f' }, // 'f'
		{"show-variable",       no_argument,       &config.flag_show_variable,      0x01}, // 's'
		{"show-variable-stop",  no_argument,       &config.flag_show_variable_stop, 0x01}, // none
		{"set-printer-type",    no_argument,       0,                               'p'},  // 'p'
		{"set-output-type",     no_argument,       0,                               'o'},  // 'o'
		{"language",            no_argument,       0,                               'i'},  // 'i'
		{"run",                 no_argument,       &config.flag_run, 0x01},                // none
		{"no-run",              no_argument,       &config.flag_run, 0x00},                // none
		{"print",               no_argument,       &config.flag_print, 0x01},              // none
		{"no-print",            no_argument,       &config.flag_print, 0x00},              // none
		{0,          0,                 0,                                          0x00}  // end
	};

	environment_names_t env_names[] = {
		{"BRAINLOLLER_FLAG_SHOW_HELP",          boost::bind(&configuration::set_flag_show_help, &config, _1)},
		{"BRAINLOLLER_FLAG_SHOW_VERSION",       boost::bind(&configuration::set_flag_show_version, &config, _1)},
		{"BRAINLOLLER_FLAG_SHOW_AUTHORS",       boost::bind(&configuration::set_flag_show_authors, &config, _1)},
		{"BRAINLOLLER_FLAG_SHOW_LICENSE",       boost::bind(&configuration::set_flag_show_variable, &config, _1)},
		{"BRAINLOLLER_FLAG_SHOW_VARIABLE",      boost::bind(&configuration::set_flag_show_variable, &config, _1)},
		{"BRAINLOLLER_FLAG_SHOW_VARIABLE_STOP", boost::bind(&configuration::set_flag_show_variable_stop, &config, _1)},
		{"BRAINLOLLER_FILENAME",                boost::bind(&configuration::set_filename, &config, _1)},
		{"BRAINLOLLER_PRINTER_TYPE",            boost::bind(&configuration::set_printer_type, &config, _1)},
		{"BRAINLOLLER_OUTPUT_TYPE",             boost::bind(&configuration::set_output_type, &config, _1)},
		{"BRAINLOLLER_LANGUAGE",                boost::bind(&configuration::set_language, &config, _1)},
		{"BRAINLOLLER_FLAG_RUN",                boost::bind(&configuration::set_flag_run, &config, _1)},
		{"BRAINLOLLER_FLAG_PRINT",              boost::bind(&configuration::set_flag_print, &config, _1)},
		{"BRAINLOLLER_OPERANDS",                boost::bind(&configuration::set_operands, &config, _1)},
	};

	void usage(void) noexcept {
		std::cout << "Program: " << PROG_NAME << " (" << PROG_NAME_FULL << ")" << std::endl;
		std::cout << "Use --help or -h for help" << std::endl;
	}

	void help() noexcept {
		version();
		authors();
	
		std::cout << std::endl << "This program uses getopts' rules (with long opts)." << std::endl;
		std::cout << "You can see \"man 3 getopt_long\" or \"info getopt_long\" for more help." << std::endl;
		std::cout << std::endl << "Use " << config.global_argv[0] << " [OPTIONS]" << std::endl;
		std::cout << std::endl << "Options:" << std::endl;
		std::cout <<"-h\t--help\t\t\t\t" << "- show this help and exit" << std::endl;
		std::cout <<"-v\t--version\t\t\t" << "- show program version and exit" << std::endl;
		std::cout <<"\t--authors\t\t\t" << "- show authors and exit" << std::endl;
		std::cout <<"-l\t--license\t\t\t" << "- show license and exit" << std::endl;
		std::cout <<"-f\t--file=[STRING]\t\t\t" << "- set filename" << std::endl;
		std::cout <<"-s\t--show-variable\t\t\t" << "- show internal variables (for debug)" << std::endl;
		std::cout <<"\t--show-variable-stop\t\t" << "- show internal variables and stop (for debug)" << std::endl;
		std::cout <<"-p\t--set-printer-type\t\t" << "- set printer type" << std::endl;
		std::cout <<"-o\t--set-output-type\t\t" << "- set type of output" << std::endl;
		std::cout <<"-i\t--language\t\t\t" << "- set language name" << std::endl;
		std::cout <<"\t--run\t\t\t\t" << "- run interpretator" << std::endl;
		std::cout <<"\t--no-run\t\t\t" << "- don't run interpretator" << std::endl;
		std::cout <<"\t--print\t\t\t\t" << "- run printer (retranslator)" << std::endl;
		std::cout <<"\t--no-print\t\t\t" << "- don't run printer (retranslator)" << std::endl;
		std::cout << std::endl << "Environment:" << std::endl;
		std::cout << "\tBRAINLOLLER_FLAG_SHOW_HELP\t\t" << "- same as '-h|--help': {0,1}" << std::endl;
		std::cout << "\tBRAINLOLLER_FLAG_SHOW_VERSION\t\t" << "- same as '-v|--version': {0,1}" << std::endl;
		std::cout << "\tBRAINLOLLER_FLAG_SHOW_AUTHORS\t\t" << "- same as '--authors': {0,1}" << std::endl;
		std::cout << "\tBRAINLOLLER_FLAG_SHOW_LICENSE\t\t" << "- same as '-l|--license': {0,1}" << std::endl;
		std::cout << "\tBRAINLOLLER_FLAG_SHOW_VARIABLE\t\t" << "- same as '-s|--show-variable': {0,1}" << std::endl;
		std::cout << "\tBRAINLOLLER_FLAG_SHOW_VARIABLE_STOP\t" << "- same as '--show-variable-stop': {0,1}" << std::endl;
		std::cout << "\tBRAINLOLLER_FILENAME\t\t\t" << "- same as '-f|--file'" << std::endl;
		std::cout << "\tBRAINLOLLER_PRINTER_TYPE\t\t" << "- same as '-p|--set-printer-type'" << std::endl;
		std::cout << "\tBRAINLOLLER_OUTPUT_TYPE\t\t\t" << "- same as '-o|--set-output-type'" << std::endl;
		std::cout << "\tBRAINLOLLER_LANGUAGE\t\t\t" << "- same as '-i|--language'" << std::endl;
		std::cout << "\tBRAINLOLLER_FLAG_RUN\t\t\t" << "- same as '--run|--no-run'" << std::endl;
		std::cout << "\tBRAINLOLLER_FLAG_PRINT\t\t\t" << "- same as '--print|--no-print'" << std::endl;
		std::cout << std::endl << "Printers:" << std::endl;
		std::cout << "\t" << PRINTER_TYPE_NULL << "\t\t" << "- NULL printer (print nothing)" << std::endl;
		std::cout << "\t" << PRINTER_TYPE_SIMPLE << "\t\t" << "- SIMPLE printer (show internal codes without spaces)" << std::endl;
		std::cout << "\t" << PRINTER_TYPE_SIMPLE_SPACE << "\t" << "- SIMPLE WITH SPACE printer (show internal codes with spaces)" << std::endl;
		std::cout << "\t" << PRINTER_TYPE_CLASSIC_BRAINFUCK << "\t" << "- CLASSICAL BRAINFUCK printer (show comands of Brainfuck language)" << std::endl;
		std::cout << std::endl << "Outputs:" << std::endl;
		std::cout << "\t" << OUTPUT_TYPE_NULL << "\t\t" << "- NULL output" << std::endl;
		std::cout << "\t" << OUTPUT_TYPE_CHAR << "\t\t" << "- CHAR output" << std::endl;
		std::cout << "\t" << OUTPUT_TYPE_HEX << "\t\t" << "- HEX output" << std::endl;
		std::cout << "\t" << OUTPUT_TYPE_BOTH << "\t\t" << "- CHAR AND HEX output" << std::endl;
		std::cout << std::endl << "Language:" << std::endl;
		std::cout << "\t" << LANGUAGE_BRAINLOLLER << "\t" << "- Brainloller (limited edition)" << std::endl;
		std::cout << "\t" << LANGUAGE_BRAINFUCK << "\t" << "- Brainfuck (not support in current version of program) " << std::endl;
		std::cout << std::endl << "Example:" << std::endl;
		std::cout << "\t" << config.global_argv[0] << " --help" << std::endl;
		std::cout << "\t" << config.global_argv[0] << " -l" << std::endl;
		std::cout << "\t" << config.global_argv[0] << " -f l1.png -i brainloller -p brainfuck -o char --run --print --" << std::endl;
		std::cout << "\t" << config.global_argv[0] << " -f l1.png -i brainloller -o both --run --no-print --" << std::endl;
	}

	void license() noexcept {
		std::cout << PROG_LICENSE << std::endl;
	}

	void authors() noexcept {
		std::cout << "Authors: " << PROG_AUTHORS << std::endl;
	}

	void version() noexcept {
		std::cout << "Program: " << PROG_NAME << " (" << PROG_NAME_FULL << ")" << std::endl;
		std::cout << "Version: " << PROG_VERSION << std::endl;
	}
	
	// *************************************************************************
	// Common types, enums etc
	// *************************************************************************
	
	/// @brief
	///
	///
	enum Operation {
		OP_UNKNOWN = 0,              // Неизвестная комманда
		OP_INC_CELL,                 // Увеличить текущее значение ячейки ленты памяти на единицу
		OP_DEC_CELL,                 // Уменьшить текущее значение ячейки ленты памяти на единицу
		OP_NEXT_CELL,                // Сдвинуть указатель текущей ячейки ленты памяти вправо на одну ячейку
		OP_PREV_CELL,                // Сдвинуть указатель текущей ячейки ленты памяти вправо на одну ячейку
		OP_READ,                     // Ввод
		OP_WRITE,                    // Вывод
		OP_BEGIN_LOOP,               //
		OP_END_LOOP,                 //
		OP_ROTATE_CLOCKWISE,         //
		OP_ROTATE_COUNTER_CLOCKWISE, // 
	};

	/// @brief
	///
	///
	enum OutputType {
		OT_NULL = 0,     // Пустой вывод (глушить вывод)
		OT_CHAR,         // Символы
		OT_HEX,          // Шестнадцатеричные числа
		OT_BOTH          // Оба (символы и шестнадцатеричные числа)
	};
	
	// *************************************************************************
	// Common and specific exceptions' interfaces and exceptions' classes
	// *************************************************************************

	/// @brief
	///
	///
	class IMachineException : public std::exception {
		typedef IMachineException self;
	protected:
		IMachineException(void) noexcept {}
	public:
		virtual ~IMachineException(void) noexcept {}
		virtual const char* what(void) const noexcept {
			return self::msg.c_str();
		}
	private:
		static std::string const msg;
	};

	/// @brief
	///
	///
	class ShitHappendGraphicMachineException : public IMachineException {
		typedef ShitHappendGraphicMachineException self;
	public:
		ShitHappendGraphicMachineException(void) {}
		virtual ~ShitHappendGraphicMachineException(void) noexcept {}
		virtual const char* what(void) const noexcept {
			return self::msg.c_str();
		}
	private:
		static std::string const msg;
	};

	/// @brief
	///
	///
	class UnsupportOperationGraphicMachineException : public IMachineException {
		typedef UnsupportOperationGraphicMachineException self;
	public:
		UnsupportOperationGraphicMachineException(void) {}
		virtual ~UnsupportOperationGraphicMachineException(void) noexcept {}
		virtual const char* what(void) const noexcept {
			return self::msg.c_str();
		}
	private:
		static std::string const msg;
	};

	/// @brief
	///
	///
	class UnknownOperationGraphicMachineException : public IMachineException {
		typedef UnknownOperationGraphicMachineException self;
	public:
		UnknownOperationGraphicMachineException(void) {}
		virtual ~UnknownOperationGraphicMachineException(void) noexcept {}
		virtual const char* what(void) const noexcept {
			return self::msg.c_str();
		}
	private:
		static std::string const msg;
	};

	/// @brief
	///
	///
	class UnexpectedOperationGraphicMachineException : public IMachineException {
		typedef UnexpectedOperationGraphicMachineException self;
	public:
		UnexpectedOperationGraphicMachineException(void) {}
		virtual ~UnexpectedOperationGraphicMachineException(void) noexcept {}
		virtual const char* what(void) const noexcept {
			return self::msg.c_str();
		}
	private:
		static std::string const msg;
	};

	/// @brief
	///
	///
	class UnexpectedEndOfProgrammGraphicMachineException : public IMachineException {
		typedef UnexpectedEndOfProgrammGraphicMachineException self;
	public:
		UnexpectedEndOfProgrammGraphicMachineException(void) {}
		virtual ~UnexpectedEndOfProgrammGraphicMachineException(void) noexcept {}
		virtual const char* what(void) const noexcept {
			return self::msg.c_str();
		}
	private:
		static std::string const msg;
	};
	
	std::string const IMachineException::msg = std::string("Error! It's a base IMachineException! Your code is shit");
	std::string const ShitHappendGraphicMachineException::msg = std::string("Unknown exception!");
	std::string const UnsupportOperationGraphicMachineException::msg = std::string("Unsupport operation!");
	std::string const UnknownOperationGraphicMachineException::msg = std::string("Unknown operation!");
	std::string const UnexpectedOperationGraphicMachineException::msg = std::string("Unexpected operation!");
	std::string const UnexpectedEndOfProgrammGraphicMachineException::msg = std::string("Unexpected end of program!");

	// *************************************************************************
	// Main interfaces
	// *************************************************************************
	
	/// @brief
	///
	///
	class IPrinter {
	public:
		/// @brief
		///
		///
		virtual ~IPrinter(void) noexcept {}

		/// @brief
		///
		///
		virtual std::string print(Operation op) const = 0;
	};

	/// @brief
	///
	///
	class IIteratorMachine {
	public:
		///
		///
		///
		IIteratorMachine(void) noexcept {}

		///
		///
		///
		virtual ~IIteratorMachine() noexcept {}
	
		///
		///
		///
		virtual void init(void) = 0;
	
		///
		///
		///
		virtual boost::uint32_t get_size(void) const = 0;
	
		///
		///
		///
		virtual boost::uint32_t get_cur_pos(void) const = 0;
	
		///
		///
		///
		virtual void set_cur_pos(boost::uint32_t pos) = 0;
	
		///
		///
		///
		virtual Operation get_next(void) = 0;
	
		///
		///
		///
		virtual bool is_done(void) const = 0;
	};

	/// @brief
	///
	///
	class IMachine {
	public:
	
		/// @brief
		///
		///
		virtual ~IMachine() noexcept {}

		/// @brief
		///
		///
		virtual void init(void) = 0;
	
		/// @brief
		///
		///
		virtual void run(void) = 0;
	
		/// @brief
		///
		///
		virtual void print(void) = 0;
	};
	
	/// @brief
	///
	///
	class IMachineCreator {
	public:
		IMachineCreator(void) noexcept {}
		virtual ~IMachineCreator(void) noexcept {}
		virtual std::shared_ptr<IMachine> create(void) const = 0;
	};
		
	// *************************************************************************
	// Main classes
	// *************************************************************************
	
	/// @brief
	///
	///
	class NullPrinter : public IPrinter {
		typedef NullPrinter self;
	public:
		/// @brief
		///
		///
		NullPrinter(void) {
		}

		/// @brief Destructor of class
		///
		///
		virtual ~NullPrinter(void) noexcept {}

		///
		///
		///
		virtual std::string print(Operation op) const {
			UNUSED_SUPPRESSOR(op);
			
			return std::string();
		}
	};

	/// @brief
	///
	///
	class SimplePrinter : public IPrinter {
		typedef SimplePrinter self;
	public:
		/// @brief
		///
		///
		SimplePrinter(void) {
		}

		/// @brief Destructor of class
		///
		///
		virtual ~SimplePrinter(void) noexcept {}

		///
		///
		///
		virtual std::string print(Operation op) const {
			return boost::lexical_cast<std::string>(
				static_cast<boost::int32_t>(op));
		}
	};

	/// @brief
	///
	///
	class SimpleSpacePrinter : public SimplePrinter {
		typedef SimpleSpacePrinter self;
		typedef SimplePrinter parent;
	public:
		/// @brief
		///
		///
		SimpleSpacePrinter(void) {
		}

		/// @brief Destructor of class
		///
		///
		virtual ~SimpleSpacePrinter(void) noexcept {}

		///
		///
		///
		virtual std::string print(Operation op) const {
			return parent::print(op) + " ";
		}
	};

	/// @brief
	///
	///
	class ClassicalBrainfuckPrinter : public IPrinter {
		typedef ClassicalBrainfuckPrinter self;
	public:
		/// @brief
		///
		///
		ClassicalBrainfuckPrinter(void) {
			this->command[OP_UNKNOWN] = self::OP_UNKNOWN_STR;
			this->command[OP_INC_CELL] = self::OP_INC_CELL_STR;
			this->command[OP_DEC_CELL] = self::OP_DEC_CELL_STR;
			this->command[OP_NEXT_CELL] = self::OP_NEXT_CELL_STR;
			this->command[OP_PREV_CELL] = self::OP_PREV_CELL_STR;
			this->command[OP_READ] = self::OP_READ_STR;
			this->command[OP_WRITE] = self::OP_WRITE_STR;
			this->command[OP_BEGIN_LOOP] = self::OP_BEGIN_LOOP_STR;
			this->command[OP_END_LOOP] = self::OP_END_LOOP_STR;
			this->command[OP_ROTATE_CLOCKWISE] = self::OP_ROTATE_CLOCKWISE_STR;
			this->command[OP_ROTATE_COUNTER_CLOCKWISE] = self::OP_ROTATE_COUNTER_CLOCKWISE_STR;
		}

		/// @brief Destructor of class
		///
		///
		virtual ~ClassicalBrainfuckPrinter(void) noexcept {}

		///
		///
		///
		virtual std::string print(Operation op) const {
			return this->command.at(op);
		}
	private:
		static std::string const OP_UNKNOWN_STR;
		static std::string const OP_INC_CELL_STR;
		static std::string const OP_DEC_CELL_STR;
		static std::string const OP_NEXT_CELL_STR;
		static std::string const OP_PREV_CELL_STR;
		static std::string const OP_READ_STR;
		static std::string const OP_WRITE_STR;
		static std::string const OP_BEGIN_LOOP_STR;
		static std::string const OP_END_LOOP_STR;
		static std::string const OP_ROTATE_CLOCKWISE_STR;
		static std::string const OP_ROTATE_COUNTER_CLOCKWISE_STR;
		
		std::map<Operation, std::string> command;
	};

	std::string const ClassicalBrainfuckPrinter::OP_UNKNOWN_STR = "";
	std::string const ClassicalBrainfuckPrinter::OP_INC_CELL_STR = "+";
	std::string const ClassicalBrainfuckPrinter::OP_DEC_CELL_STR = "-";
	std::string const ClassicalBrainfuckPrinter::OP_NEXT_CELL_STR = ">";
	std::string const ClassicalBrainfuckPrinter::OP_PREV_CELL_STR = "<";
	std::string const ClassicalBrainfuckPrinter::OP_READ_STR = ",";
	std::string const ClassicalBrainfuckPrinter::OP_WRITE_STR = ".";
	std::string const ClassicalBrainfuckPrinter::OP_BEGIN_LOOP_STR = "[";
	std::string const ClassicalBrainfuckPrinter::OP_END_LOOP_STR = "]";
	std::string const ClassicalBrainfuckPrinter::OP_ROTATE_CLOCKWISE_STR = "";
	std::string const ClassicalBrainfuckPrinter::OP_ROTATE_COUNTER_CLOCKWISE_STR = "";

	/// @brief
	///
	/// 
	class GraphicIteratorMachine : public IIteratorMachine {
		typedef GraphicIteratorMachine self;
	
		typedef std::map<boost::uint32_t, Operation> translator_map;
	public:
		/// @brief
		///
		///
		GraphicIteratorMachine(std::string const& filename, boost::uint32_t raw_cell_size) :
			filename(filename),
			graphic_cell_size(raw_cell_size),
			cur_pos(0),
			n(0) {
			
			this->gp = cv::imread(this->filename, 1);
			this->gp_size = this->gp.size();
			this->n = this->gp_size.width / this->graphic_cell_size;

			this->trans[self::CODE_INC_CELL]                 = OP_INC_CELL;
			this->trans[self::CODE_DEC_CELL]                 = OP_DEC_CELL;
			this->trans[self::CODE_NEXT_CELL]                = OP_NEXT_CELL;
			this->trans[self::CODE_PREV_CELL]                = OP_PREV_CELL;
			this->trans[self::CODE_READ]                     = OP_READ;
			this->trans[self::CODE_WRITE]                    = OP_WRITE;
			this->trans[self::CODE_BEGIN_LOOP]               = OP_BEGIN_LOOP;
			this->trans[self::CODE_END_LOOP]                 = OP_END_LOOP;
			this->trans[self::CODE_ROTATE_CLOCKWISE]         = OP_ROTATE_CLOCKWISE;
			this->trans[self::CODE_ROTATE_COUNTER_CLOCKWISE] = OP_ROTATE_COUNTER_CLOCKWISE;
		}

		/// @brief
		///
		///
		GraphicIteratorMachine(std::string const& filename) : GraphicIteratorMachine(filename, 20) {}

		/// @brief
		///
		///
		virtual ~GraphicIteratorMachine(void) noexcept {}
	
		/// @brief
		///
		///
		virtual void init(void) {
			this->cur_pos = 0;
		}
	
		/// @brief
		///
		///
		virtual boost::uint32_t get_size(void) const {
			return this->n;
		}

		/// @brief
		///
		///
		virtual boost::uint32_t get_cur_pos(void) const {
			return this->cur_pos;
		}
	
		/// @brief
		///
		///
		virtual void set_cur_pos(boost::uint32_t pos) {
			this->cur_pos = pos;
		}
	
		/// @brief
		///
		///
		virtual Operation get_next(void) {
			boost::uint32_t delta = this->cur_pos * this->graphic_cell_size;
			cv::Vec3b code = this->gp.at<cv::Vec3b>(1, delta);

			Operation op = this->code_to_op(this->pack(code));
			
			this->cur_pos++;
		
			return op;
		}

		/// @brief
		///
		///
		virtual bool is_done(void) const {
			return this->cur_pos >= this->n;
		}
	protected:
		virtual Operation code_to_op(boost::uint32_t code) const {
			translator_map::const_iterator i_end = this->trans.end();
			translator_map::const_iterator i = i_end;

			// RU: Пояснение. Тут не используется метод at или [] по причине генерации исключения,
			//     если такой элемент не будет найден. Генерация исключения - довольно медленный
			//     процесс. Чтобы гарантировать более-менее одинаковое время поиска операции
			//     используется алгоритм. Возможно, вариант с исключением более приемлем в некоторых
			//     случаях, т.к. данные исключения могут возникать при нормальной работе крайне редко.
			//     Однако тут всё равно используется алгоритм.
			i = std::find_if(this->trans.begin(), this->trans.end(), [code](auto x)->bool{
					return (x.first == code);
				});
		
			return (i != i_end) ? i->second : OP_UNKNOWN;
		}
	
		virtual boost::uint32_t pack(cv::Vec3b const value) const {
			boost::uint32_t res = 0x00000000;

			res =  (value[self::CH_NUM_RED]   << 0);
			res += (value[self::CH_NUM_GREEN] << 8);
			res += (value[self::CH_NUM_BLUE]  << 16);
			
			return res;
		}
	private:
		std::string const filename;
		boost::uint32_t graphic_cell_size;
		boost::uint32_t cur_pos;
		boost::uint32_t n;
		cv::Mat gp;
		cv::Size gp_size;
		self::translator_map trans;
	
		static boost::uint32_t const RED;
		static boost::uint32_t const GREEN;
		static boost::uint32_t const BLUE;
	
		static boost::uint8_t const CH_NUM_RED;
		static boost::uint8_t const CH_NUM_GREEN;
		static boost::uint8_t const CH_NUM_BLUE;

		static boost::uint32_t const CODE_INC_CELL;
		static boost::uint32_t const CODE_DEC_CELL;
		static boost::uint32_t const CODE_NEXT_CELL;
		static boost::uint32_t const CODE_PREV_CELL;
		static boost::uint32_t const CODE_READ;
		static boost::uint32_t const CODE_WRITE;
		static boost::uint32_t const CODE_BEGIN_LOOP;
		static boost::uint32_t const CODE_END_LOOP;
		static boost::uint32_t const CODE_ROTATE_CLOCKWISE;
		static boost::uint32_t const CODE_ROTATE_COUNTER_CLOCKWISE;
	};

	boost::uint32_t const GraphicIteratorMachine::RED   = 0x000000FF;
	boost::uint32_t const GraphicIteratorMachine::GREEN = 0x0000FF00;
	boost::uint32_t const GraphicIteratorMachine::BLUE  = 0x00FF0000;

	boost::uint8_t const GraphicIteratorMachine::CH_NUM_RED   = 2;
	boost::uint8_t const GraphicIteratorMachine::CH_NUM_GREEN = 1;
	boost::uint8_t const GraphicIteratorMachine::CH_NUM_BLUE  = 0;

	boost::uint32_t const GraphicIteratorMachine::CODE_INC_CELL                 = 0x00FF00;
	boost::uint32_t const GraphicIteratorMachine::CODE_DEC_CELL                 = 0x008000;
	boost::uint32_t const GraphicIteratorMachine::CODE_NEXT_CELL                = 0xFF0000;
	boost::uint32_t const GraphicIteratorMachine::CODE_PREV_CELL                = 0x800000;
	boost::uint32_t const GraphicIteratorMachine::CODE_READ                     = 0x000080;
	boost::uint32_t const GraphicIteratorMachine::CODE_WRITE                    = 0x0000FF;
	boost::uint32_t const GraphicIteratorMachine::CODE_BEGIN_LOOP               = 0xFFFF00;
	boost::uint32_t const GraphicIteratorMachine::CODE_END_LOOP                 = 0x808000;
	boost::uint32_t const GraphicIteratorMachine::CODE_ROTATE_CLOCKWISE         = 0x008080;
	boost::uint32_t const GraphicIteratorMachine::CODE_ROTATE_COUNTER_CLOCKWISE = 0x00FFFF;

	/// @brief
	///
	/// @note Непосредственно машина интерпретатора. Является интерпретатором языка Брейнроллер,
	///       однако, не все комманды реализованы. Комманды смены направления движения не
	///       реализованы.
	class GraphicMachine : public IMachine {
		typedef GraphicMachine self;
	protected:
		typedef boost::uint8_t cell_t;
		
		typedef std::vector<self::cell_t> cells_t;
		typedef self::cells_t& cells_ref;
		typedef self::cells_t const& cells_cref;
		
		typedef self::cells_t::size_type index_t;
		typedef self::index_t& index_ref;
		typedef self::index_t const& index_cref;
		
		typedef std::stack<boost::uint32_t> ret_stack_t;
		typedef self::ret_stack_t& ret_stack_ref;
		typedef self::ret_stack_t const& ret_stack_cref;
	public:
	
		/// @brief
		///
		///
		GraphicMachine(std::shared_ptr<IPrinter> p,
					   std::shared_ptr<IIteratorMachine> m,
					   OutputType o) :
			printer(p),
			im(m),
			ot(o) {}

		/// @brief
		///
		///
		virtual ~GraphicMachine(void) noexcept {}

		/// @brief
		///
		///
		virtual void init(void) {
			this->im.get()->init();
		}
	
		/// @brief
		///
		///
		virtual void run(void) throw(IMachineException) {
			self::index_t num = 0;
			self::cells_t cells(30000);
			self::ret_stack_t st;

			std::fill(cells.begin(), cells.end(), 0);

			this->interpretator(num, cells, st);

			std::cout << std::endl;
		}

		/// @brief
		///
		///
		virtual void print(void) {
			std::cout << "BEGIN PROGRAM >>>>>>>>>>" << std::endl;
			while(!this->im.get()->is_done()) {
				Operation op = this->im.get()->get_next();
				std::cout << this->printer.get()->print(op);
			}
			std::cout << std::endl;
			std::cout << "<<<<<<<<<< END PROGRAM" << std::endl;
		}
	protected:
		virtual void interpretator(self::index_ref num, self::cells_ref cells, self::ret_stack_ref st) {
			while(!this->im.get()->is_done()) {
				Operation op = this->im.get()->get_next();
				
				switch(op) {
				case OP_INC_CELL:
					cells[num]++;
					break;
				case OP_DEC_CELL:
					cells[num]--;
					break;
				case OP_NEXT_CELL:
					num++;
					break;
				case OP_PREV_CELL:
					num--;
					break;
				case OP_READ:
					std::cout << "Enter one symbol and press <ENTER>: ";
					std::cin >> cells[num];
					break;
				case OP_WRITE:
					this->output(cells[num]);

					break;
				case OP_BEGIN_LOOP:
					if(!cells[num]) {
						do {
							op = this->im.get()->get_next();
						}
						while(op != OP_END_LOOP && !this->im.get()->is_done());

						if(op != OP_END_LOOP && this->im.get()->is_done()) {
							throw UnexpectedEndOfProgrammGraphicMachineException();
						}
					}
					else {
						if(this->im.get()->get_cur_pos()) {
							st.push(this->im.get()->get_cur_pos() - 1);
						}
						else {
							throw ShitHappendGraphicMachineException();
						}
					}
					break;
				case OP_END_LOOP:
					if(st.empty()) {
						throw UnexpectedOperationGraphicMachineException();
					}
					else {
						this->im.get()->set_cur_pos(st.top());
						st.pop();
					}
					break;
				case OP_UNKNOWN:
					throw UnknownOperationGraphicMachineException();
				case OP_ROTATE_CLOCKWISE:
				case OP_ROTATE_COUNTER_CLOCKWISE:
				default:
					throw UnsupportOperationGraphicMachineException();
				};
			}
		}

		virtual void output(self::cell_t value) {
			switch(this->ot) {
			case OT_CHAR:
				std::cout << value;
				break;
			case OT_HEX:
				std::cout << std::uppercase
						  << std::hex
						  << std::setfill('0')
						  << std::setw(2)
						  << static_cast<boost::uint32_t>(value)
						  << std::nouppercase
						  << std::dec
						  << std::setw(0);
				break;
			case OT_BOTH:
				std::cout << value
						  << "["
						  << std::uppercase
						  << std::hex
						  << std::setfill('0')
						  << std::setw(2)
						  << static_cast<boost::uint32_t>(value)
						  << "]"
						  << std::nouppercase
						  << std::dec
						  << std::setw(0);
				break;
			case OT_NULL:
			default:
				break;
			};
		}
	private:
		std::shared_ptr<IPrinter> printer;
		std::shared_ptr<IIteratorMachine> im;
		OutputType ot;
	};

	/// @brief
	///
	///
	class GraphicMachineCreator : public IMachineCreator {
	public:
		/// @brief
		///
		///
		explicit GraphicMachineCreator(std::shared_ptr<IPrinter> p,
									   std::shared_ptr<IIteratorMachine> im,
									   OutputType o)
			: IMachineCreator(),
			  printer(p),
			  iterator_machine(im),
			  ot(o) {
		}

		/// @brief
		///
		///
		virtual ~GraphicMachineCreator(void) noexcept {}
		
		/// @brief
		///
		///
		virtual std::shared_ptr<IMachine> create(void) const {
			return std::make_shared<GraphicMachine>(printer, iterator_machine, ot);
		}
	private:
		std::shared_ptr<IPrinter> printer;
		std::shared_ptr<IIteratorMachine> iterator_machine;
		OutputType ot;
	};
} // namespace

/// @brief
///
///
int main(int argc, char** argv) {
	[&]()->void {
		config.global_argc = argc;
		config.global_argv = argv;
	
		// Read environment variable and set them value
		std::for_each(&env_names[0], &env_names[sizeof(env_names) / sizeof(environment_names_t)], [](auto x)->void{
				auto current_env = getenv(x.name);
				if(current_env) {
					x.setter(current_env);
				}
			});

		// Read options and set them value
		[&argc, &argv]()->void{
			int optc = 0;
			while((optc = getopt_long(argc, argv, ":shvlf:p:o:i:", longopts, 0)) != -1) {
				switch(optc) {
				case 's':
					config.flag_show_variable = 1;
					break;
				case 'h':
					config.flag_show_help = 1;
					break;
				case 'v':
					config.flag_show_version = 1;
					break;
				case 'l':
					config.flag_show_license = 1;
					break;
				case 'f':
					if(optarg!= nullptr) {
						config.filename = optarg;
					}
					break;
				case 'p':
					if(optarg!= nullptr) {
						config.printer_type = optarg;
					}
					break;
				case 'o':
					if(optarg!= nullptr) {
						config.output_type = optarg;
					}
					break;
				case 'i':
					if(optarg!= nullptr) {
						config.language = optarg;
					}
					break;
				case 0:
					break;
				case ':':
					std::cerr << argv[0] << ": option '-" << optopt << "' requires an argument: break!" << std::endl;
					usage();
					exit(EXIT_FAILURE);
					break;
				case '?':
				default:
					std::cerr << argv[0] << ": option '" << optopt << "' is invalid: break!" << std::endl;
					usage();
					exit(EXIT_FAILURE);
					break;
				}
			}
		}();

		// Read operands and set them
		std::for_each(&argv[optind], &argv[argc], [](auto x)->void{
				config.operands.push_back(std::string(x));
			});
	
		if(config.flag_show_variable || config.flag_show_variable_stop) {
			std::cout << "Program variables (internal flags): " << std::endl;
			std::cout << "\tflag_show_help = " << config.flag_show_help << std::endl;
			std::cout << "\tflag_show_version = " << config.flag_show_version << std::endl;
			std::cout << "\tflag_show_authors = " << config.flag_show_authors<< std::endl;
			std::cout << "\tflag_show_license = " << config.flag_show_license << std::endl;
			std::cout << "\tflag_show_variable = " << config.flag_show_variable << std::endl;
			std::cout << "\tflag_show_variable_stop = " << config.flag_show_variable_stop << std::endl;
			std::cout << "\tflag_run = " << config.flag_run << std::endl;
			std::cout << "\tflag_print = " << config.flag_print << std::endl;
			std::cout << "\tfilename = " << config.filename << std::endl;
			std::cout << "\tprinter_type = " << config.printer_type << std::endl;
			std::cout << "\toutput_type = " << config.output_type << std::endl;
			std::cout << "\tlanguage = " << config.language << std::endl;
			std::cout << "\toperands = " << ((config.operands.empty()) ? "(absense)" : "") << std::endl;

			[]()->void {
				int counter = 0;
				std::for_each(config.operands.begin(), config.operands.end(), [&counter](auto x)->void {
						std::cout << "\t\t" << counter++ << " = " << x << std::endl;
					});
			}();
		
			if(config.flag_show_variable_stop) {
				::exit(EXIT_SUCCESS);
			}
		}
	
		if(config.flag_show_help) {
			help();
			::exit(EXIT_SUCCESS);
		}

		if(config.flag_show_version) {
			version();
			::exit(EXIT_SUCCESS);
		}

		if(config.flag_show_authors) {
			authors();
			::exit(EXIT_SUCCESS);
		}

		if(config.flag_show_license) {
			license();
			::exit(EXIT_SUCCESS);
		}

		if(config.filename.empty()) {
			std::cerr << "Filename is empty: break!" << std::endl;
			usage();
			::exit(EXIT_FAILURE);
		}
	}();

	// Work
	[]()->void{
		std::string const filename(config.filename);
		
		std::shared_ptr<IMachineCreator> creator;
		std::shared_ptr<IPrinter> printer; 
		std::shared_ptr<IIteratorMachine> iterator_machine;
		OutputType ot;

		if(!config.language.compare(LANGUAGE_BRAINFUCK)) {
			std::cerr << "This language is not supporting in current version! Sorry!: break!" << std::endl;
			usage();
			::exit(EXIT_FAILURE);
		}
		else if(!config.language.compare(LANGUAGE_BRAINLOLLER)) {
			iterator_machine = std::make_shared<GraphicIteratorMachine>(filename);
		}
		else {
			std::cerr << "Uncorrect language name: break!" << std::endl;
			usage();
			::exit(EXIT_FAILURE);
		}

		if(!config.printer_type.compare(PRINTER_TYPE_NULL)) {
			printer = std::make_shared<NullPrinter>();
		}
		else if(!config.printer_type.compare(PRINTER_TYPE_SIMPLE)) {
			printer = std::make_shared<SimplePrinter>();
		}
		else if(!config.printer_type.compare(PRINTER_TYPE_SIMPLE_SPACE)) {
			printer = std::make_shared<SimpleSpacePrinter>();
		}
		else if(!config.printer_type.compare(PRINTER_TYPE_CLASSIC_BRAINFUCK)) {
			printer = std::make_shared<ClassicalBrainfuckPrinter>();
		}
		else {
			std::cerr << "Uncorrect printer type: break!" << std::endl;
			usage();
			::exit(EXIT_FAILURE);
		}

		if(!config.output_type.compare(OUTPUT_TYPE_NULL)) {
			ot = OT_NULL;
		}
		else if(!config.output_type.compare(OUTPUT_TYPE_CHAR)) {
			ot = OT_CHAR;
		}
		else if(!config.output_type.compare(OUTPUT_TYPE_HEX)) {
			ot = OT_HEX;
		}
		else if(!config.output_type.compare(OUTPUT_TYPE_BOTH)) {
			ot = OT_BOTH;
		}
		else {
			std::cerr << "Uncorrect output type: break!" << std::endl;
			usage();
			::exit(EXIT_FAILURE);
		}
		
		creator = std::make_shared<GraphicMachineCreator>(printer, iterator_machine, ot);
		
		std::shared_ptr<IMachine> m = creator.get()->create();
		
		try {
			if(config.flag_print) {
				m.get()->init();
				m.get()->print();
			}

			if(config.flag_run) {
				m.get()->init();
				m.get()->run();
			}
		}
		catch(IMachineException const& ex) {
			std::cerr << "Exception! " << ex.what() << std::endl;
		}
	}();
	
	return EXIT_SUCCESS;
}

#undef UNUSED_SUPPRESSOR

/* *****************************************************************************
 * End of file
 **************************************************************************** */
