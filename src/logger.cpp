#include "logger.h"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <stack>

using namespace vex;

//There may be variadic function stuff from c++11
//I don't trust that this compiler supports it, so we're doing it the c way
void Logger::addToBuffer(bool makeNewLine, const char* format, va_list arg) {
    uint32_t lineSize = this->maxLineSize;
    char buf[lineSize];
    vsnprintf(buf, lineSize, format, arg);

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

void Logger::print(const char *format, ...) { 
    va_list arg;
    this->addToBuffer(false, format, arg);
    this->printBuffer();
    va_end(arg);
}

void Logger::println(const char *format, ...) {
    va_list arg;
    this->addToBuffer(true, format, arg);
    this->printBuffer();
    va_end(arg);
}

void Logger::printBuffer() { 
    //Initial state
    this->screen->clearScreen();
    bool currentlyOnNewLine = true;

    std::stack<outputLine> toPrint;
    //insert stuff into toPrint

    while (!toPrint.empty()) {
        outputLine line = toPrint.top();
        const char* string = line.contents.c_str();
        bool onNewLine = line.onNewLine;
        if (onNewLine && !currentlyOnNewLine) {
            this->screen->newLine();
            currentlyOnNewLine = true;
        }
        this->screen->print(string);
        if (!onNewLine) {
            currentlyOnNewLine = false;
        }
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

Logger::Builder& Logger::Builder::logToStdOut(bool logToStdOut) {
    this->logExternally = logToStdOut;
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