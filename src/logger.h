#pragma once
#include "vex.h"

#include <vector>
#include <string>
#include <cstdarg>
#include <stack>
#include <cassert>

using namespace vex;

//Templates are miserable, so variadic functions(variable parameter count) are implemented with va_list from c

class Logger 
{
public:
    static constexpr int SCREEN_WIDTH = 480;
    static constexpr int SCREEN_HEIGHT = 240;
private:
    std::vector<std::string> buffer{};

    //Properties
    brain::lcd* screen;
    bool doWordWrap = true;
    bool logExternally = false;
    std::string logFile = "";

    //Designated initializer with default values can only be used in c++20
    struct DesignatedBuilder {
        bool doWordWrap = true;
        bool logExternally = false;
        std::string logFile = "";
    };
public:
    //Prefer construction via builder
    Logger(brain::lcd* screen, bool doWordWrap,
        bool logExternally, std::string logFile);
    Logger(brain::lcd* screen, DesignatedBuilder configs);
    Logger(brain::lcd* screen);

    static Logger basic(brain& brain);

    void clearScreen();

    //Some function declarations COPIED from vex_brain.h

    /** 
     * @brief Prints a number, string, or Boolean with a new line.
     * Provides formatting from printf
     * @param format This is a reference to a char format that prints the value of variables
     * @param ... A variable list of parameters to insert into format string
     * @note Some characters like \t and \\n don't work as they would normally
    */          
    void println(const char *format, ... );

    /** 
     * @brief Prints a number, string, or Boolean.
     * Provides formatting from printf
     * @param format This is a reference to a char format that prints the value of variables
     * @param ... A variable list of parameters to insert into format string
     * @note Some characters like \t and \\n don't work as they would normally
    */          
    void print(const char *format, ... );

    //These are just redefinitions stolen from vex_brain.h for simpler printing

    /** 
     * @brief Prints a number, string, or Boolean.
     * Provides formatting from printf
     * @param value Information to display on the screen
     * @note Some characters like \t and \\n don't work as they would normally
    */  
    template <class T>
    void     print( T value ) {
      if( std::is_same< T, int >::value )
        print( "%d", (int)value );
      else
      if( std::is_same< T, double >::value )
        print( "%.2f", (double)value );
      else {
        // primarily to handle modkit number
        if( (int)value == value )
          print( "%d", (int)value );
        else
          print( "%.2f", (double)value );
      }
    }

    /** 
     * @brief Prints a number, string, or Boolean.
     * Provides formatting from printf
     * @param value Information to display on the screen
     * @note Some characters like \t and \\n don't work as they would normally
    */  
    template <class T>
    void println( T value ) {
      if( std::is_same< T, int >::value )
        println( "%d", (int)value );
      else
      if( std::is_same< T, double >::value )
        println( "%.2f", (double)value );
      else {
        // primarily to handle modkit number (what's a modkit number??)
        if( (int)value == value )
          println( "%d", (int)value );
        else
          println( "%.2f", (double)value );
      }
    }

private: 
    void _printBuffer();
    void _addToBuffer(bool makeNewLine, const char* format, va_list args);
    std::stack<std::string> _fillPrintStack();

    //CONSTRUCTION

    //Classic Builder pattern
    class Builder 
    {
    private:
        brain::lcd* screen;
        bool doWordWrap = true;
        bool logExternally = false;
        std::string logFile = "";

    public:
        Builder(brain::lcd *screen);

        /**
         * @brief By default the logger wraps words onto a new line this method disables that
         */
        Builder& disableWordWrap();

        /**
         * @brief Enables printing to computer's standard output
         */
        Builder& printToStdout();

        /**
         * @brief Any output from the logger can also be logged into a file
         * @param fileName The file path as a string.
         */
        Builder& logToFile(std::string fileName);


        /**
         * @brief Finishes the building process
         * @return The completed customized logger
         */
        Logger build();
    };

public:
    /**
     * @brief Creates a new builder with some default values with the ability
     *  tack on methods from the builder class to customize
     * @param screen This is a pointer to the brain's screen the logger will output to.
     * Usually you would pass in @code &Brain.Screen @endcode
     */
    static Builder create(brain::lcd* screen);
};
