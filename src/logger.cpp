#include "logger.h"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <stack>

//Sometimes intellisense is finicky
#include <stdarg.h>

using namespace vex;


void Logger::addToBuffer(bool makeNewLine, const char* format, va_list args) {
    std::cout << "add to buffer";
    uint32_t lineSize = this->maxLineSize;
    char buf[lineSize];

    va_list argsCopy;
    va_copy(argsCopy, args);
    //vsnprintf just takes the output of printf(format, argsCopy), or the formatted string of what we want to log,
    //and puts it into buf as long as long as it doesn't exceed the byte limit of the second arg, lineSize
    vsnprintf(buf, lineSize, format, argsCopy);

    //Maybe we could concatenate all the outputLines that would theoretically be on the same line?
    //Continuing from this idea, theoretically once this function concludes all outputLines on the buffer would be on separate lines
    //Also, we could make word wrapping, where we separate lengthy lines into multiple outputLines
    this->buffer.push_back(outputLine {
        std::string(buf), makeNewLine
    });

    if (this->logExternally) {
        std::cout << buf;
        if (makeNewLine) {
            std::cout << '\n';
        }
    }

    if (!this->logFile.empty()) {
        std::ofstream outputFile;
        outputFile.open(this->logFile);
        if (!outputFile.is_open()) {
            std::cerr << "[ERROR] Unable to open file: " << this->logFile << std::endl;
        }
        outputFile << buf;
        if (makeNewLine) {
            outputFile << std::endl;
        }
        outputFile.close();
    }
}

void Logger::print(const char* format, ...) { 
    va_list args;
    va_start(args, format);
    this->addToBuffer(false, format, args);
    //this->screen->print(this->buffer[0].contents.c_str());
    this->printBuffer();
    va_end(args);
}

void Logger::println(const char* format, ...) {
    this->screen->clearScreen();
    this->screen->print("entered println");
    va_list args;
    va_start(args, format);
    this->addToBuffer(true, format, args);
    this->printBuffer();
    va_end(args);
}

void Logger::printBuffer() { 
    //Initial state
    this->screen->clearScreen();
    //Ensures cursor is reset
    this->screen->setCursor(1, 1);
    bool currentlyOnNewLine = true;

    std::stack<outputLine> toPrint;
    /* this->buffer contains ALL outputs, toPrint is designed to contain only the ones that can fit on the screen.
     * Why toPrint is a stack is because we will can identify lines starting from end of this->buffer vector,
     * check if adding the line would exceed the maximum height of our screen, 
     * then push it onto our stack if it doesn't, ending the process of adding to toPrint if it does exceed.
     * The last thing we push onto the stack is the furthest our buffer goes back in history, aka where we start printing
    */
    int heightLeft = SCREEN_HEIGHT;
    int size = this->buffer.size();
    for (int i = 0; i < size; i++) {
        int index = size - i - 1;
        if (index < 0 || index >= size) {
            return;
        }
        outputLine lineContainer = this->buffer.at(index);
        const char* contents = lineContainer.contents.c_str();
        int height = this->screen->getStringHeight(contents);
        if (height < heightLeft) {
            toPrint.push(lineContainer);
            heightLeft -= height;
        }
    }

    //Maybe remove the new line checks 
    //(with the maybe implementation so outputLines are always onNewLine after addToBuffer adds them to the buffer)
    while (!toPrint.empty()) {
        outputLine line = toPrint.top();
        const char* string = line.contents.c_str();
        bool onNewLine = line.onNewLine;
        if (onNewLine && !currentlyOnNewLine) {
            this->screen->newLine();
            currentlyOnNewLine = true;
        }
        this->screen->print(string);
        currentlyOnNewLine = false;
        toPrint.pop();
    }
}

//CONSTRUCTION

Logger Logger::Builder::build() {
    return Logger(screen, maxLineSize, logExternally, logFile);
}

Logger::Builder& Logger::Builder::setMaxLineSize(uint32_t bytes) {
    this->maxLineSize = bytes;
    return *this;
}

Logger::Builder& Logger::Builder::printToStdout() {
    this->logExternally = true;
    return *this;
}

Logger::Builder& Logger::Builder::logToFile(std::string logFile) {
    this->logFile = logFile;
    return *this;
}

Logger::Builder Logger::create(brain::lcd* screen) {
    return Logger::Builder(screen);
}

Logger::Logger(brain::lcd* screen, uint32_t maxLineSize,
    bool logExternally, std::string logFile)
{
    this->screen = screen;
    this->maxLineSize = maxLineSize;
    this->logExternally = logExternally;
    this->logFile = logFile;
}

Logger::Builder::Builder(brain::lcd* screen) {
    this->screen = screen;
}
