#include "logger.h"
#include <cstdio>
#include <iostream>
#include <fstream>

//Sometimes intellisense is finicky
#include <stdarg.h>

using namespace vex;

void Logger::clearScreen() {
    this->screen->clearScreen();
    this->screen->setCursor(0, 0);
}

void Logger::_addToBuffer(bool makeNewLine, const char* format, va_list args) {
    uint32_t lineSize = this->maxLineSize;
    char buf[lineSize];

    va_list argsCopy;
    va_copy(argsCopy, args);
    //vsnprintf just takes the output of printf(format, argsCopy), or the formatted string of what we want to log,
    //and puts it into buf as long as long as it doesn't exceed the byte limit of the second arg, lineSize
    vsnprintf(buf, lineSize, format, argsCopy);

    if (makeNewLine) {
        this->buffer.push_back(std::string(buf));
    } else {
        this->buffer.back().append(buf);
    }

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
        } else {
            outputFile << buf;
            if (makeNewLine) {
                outputFile << std::endl;
            }
        }
        outputFile.close();
    }
}

void Logger::print(const char* format, ...) { 
    va_list args;
    va_start(args, format);
    this->_addToBuffer(false, format, args);
    this->_printBuffer();
    va_end(args);
}

void Logger::println(const char* format, ...) {
    this->screen->clearScreen();
    va_list args;
    va_start(args, format);
    this->_addToBuffer(true, format, args);
    this->_printBuffer();
    va_end(args);
}

void Logger::_printBuffer() { 
    //Initial state
    this->screen->clearScreen();
    //Ensures cursor is reset
    this->screen->setCursor(1, 1);

    /* this->buffer contains ALL outputs, toPrint is designed to contain only the ones that can fit on the screen.
     * Why toPrint is a stack is because we will can identify lines starting from end of this->buffer vector,
     * check if adding the line would exceed the maximum height of our screen, 
     * then push it onto our stack if it doesn't, ending the process of adding to toPrint if it does exceed.
     * The last thing we push onto the stack is the furthest our buffer goes back in history, aka where we start printing
    */
    std::stack<std::string> toPrint = this->_fillPrintStack();

    while (!toPrint.empty()) {
        const char* contents = toPrint.top().c_str();
        this->screen->print(contents);
        this->screen->newLine();
        toPrint.pop();
    }
}

std::stack<std::string> Logger::_fillPrintStack() {
    std::stack<std::string> printStack;
    int heightLeft = SCREEN_HEIGHT;
    int size = this->buffer.size();
    for (int i = 0; i < size; i++) {
        int index = size - i - 1;
        assert(i >= 0 && i < this->buffer.size());
        std::string lineContainer = this->buffer.at(index);
        const char* contents = lineContainer.c_str();
        int width = this->screen->getStringWidth(contents);
        if (!this->doWordWrap || width <= SCREEN_WIDTH) {
            int height = this->screen->getStringHeight(contents);
            if (height < heightLeft) {
                printStack.push(lineContainer);
                heightLeft -= height;
            } else {
                break; //end early, no need to go back through rest of buffer
            }
        } else {
            std::vector<std::string> splitStrings;
            int strLen = lineContainer.length();
            int avgCharSize = width / strLen;
            //Guess split
            int guessStrLen = SCREEN_WIDTH / avgCharSize;
            int frontPointer = 0;
            int backPointer = frontPointer + guessStrLen;
            do {
                std::string sub = lineContainer.substr(frontPointer, guessStrLen);
                //Fine tune
                while (backPointer < strLen) {
                    int subStrWidth = this->screen->getStringWidth(sub.c_str());
                    if (subStrWidth > SCREEN_WIDTH) {
                        sub.pop_back();
                        backPointer--;
                    } else if (subStrWidth < SCREEN_WIDTH) {
                        int nextCharWidth = this->screen->getStringWidth(
                                lineContainer.substr(backPointer, 1).c_str());
                        if (subStrWidth + nextCharWidth <= SCREEN_WIDTH) {
                            backPointer++;
                            sub.push_back(lineContainer.at(backPointer));
                        } else {
                            break;
                        }
                    } else { //subStrWidth == SCREEN_WIDTH
                        break;
                    }
                }
                int height = this->screen->getStringHeight(sub.c_str());
                if (height < heightLeft) {
                    printStack.push(sub);
                    heightLeft -= height;
                } else {
                    break; //end early, no need to go back through rest of buffer
                }
                frontPointer = backPointer + 1;
            } while (backPointer <= strLen);
        }
    }
    return printStack;
}

//CONSTRUCTION

Logger Logger::Builder::build() {
    return Logger(screen, maxLineSize, logExternally, logFile);
}

Logger::Builder& Logger::Builder::setMaxLineSize(uint32_t bytes) {
    this->maxLineSize = bytes;
    return *this;
}

Logger::Builder& Logger::Builder::disableWordWrap() {
    this->doWordWrap = false;
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
