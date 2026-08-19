#include "logger.h"
#include <cstdio>
#include <iostream>
#include <fstream>

//Sometimes intellisense is finicky
#include <stdarg.h>

using namespace vex;

Logger Logger::basic(brain& brain) {
    return Logger::Builder(&brain.Screen).build();
}

void Logger::clearScreen() {
    this->screen->clearScreen();
    this->screen->setCursor(0, 0);
}

void Logger::_addToBuffer(bool makeNewLine, const char* format, va_list args) {
    va_list argsCopy1, argsCopy2;
    va_copy(argsCopy1, args);
    va_copy(argsCopy2, args);
    int size = vsnprintf(nullptr, 0, format, argsCopy1);
    std::vector<char> buf(size + 1); //include null terminator
    vsnprintf(buf.data(), buf.size(), format, argsCopy2);
    const char* bufData = buf.data();

    if (makeNewLine) {
        this->buffer.push_back(std::string(bufData));
    } else {
        if (this->buffer.empty()) {
            this->buffer.push_back("");
        }
        this->buffer.back().append(bufData);
    }

    if (this->logExternally) {
        std::cout << bufData;
        if (makeNewLine) {
            std::cout << '\n';
        }
    }

    if (!this->logFile.empty()) {
        std::ofstream outputFile;
        outputFile.open(this->logFile, std::ios::app);
        if (!outputFile.is_open()) {
            std::cerr << "[ERROR] Unable to open file: " << this->logFile << std::endl;
        } else {
            outputFile << bufData;
            if (makeNewLine) {
                outputFile << std::endl;
            }
        }
        outputFile.close();
    }
    va_end(argsCopy1);
    va_end(argsCopy2);
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
        assert(index >= 0 && index < this->buffer.size());
        std::string lineContainer = this->buffer.at(index);
        const char* contents = lineContainer.c_str();
        int width = this->screen->getStringWidth(contents);
        if (!this->doWordWrap || width <= SCREEN_WIDTH) {
            int height = this->screen->getStringHeight(contents);
            if (height < heightLeft) {
                printStack.push(lineContainer);
                heightLeft -= height;
            } else {
                break;
            }
        } else {
            std::vector<std::string> splitStrings;
            int strLen = lineContainer.length();
            int avgCharSize = width / strLen;
            int guessStrLen = SCREEN_WIDTH / avgCharSize;
            int frontPointer = 0;
            do {
                int backPointer = std::min(frontPointer + guessStrLen, strLen);
                std::string sub = lineContainer.substr(frontPointer, backPointer - frontPointer);
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
                            sub.push_back(lineContainer.at(backPointer));
                            backPointer++;
                        } else {
                            break;
                        }
                    } else {
                        break;
                    }
                }
                //Walk back to word boundary
                int wordBack = backPointer;
                while (wordBack > frontPointer && lineContainer.at(wordBack - 1) != ' ') {
                    wordBack--;
                }
                if (wordBack > frontPointer) {
                    sub = lineContainer.substr(frontPointer, wordBack - frontPointer);
                    backPointer = wordBack;
                }
                int height = this->screen->getStringHeight(sub.c_str());
                if (height < heightLeft) {
                    splitStrings.push_back(sub);
                    heightLeft -= height;
                } else {
                    break;
                }
                frontPointer = backPointer;
            } while (frontPointer < strLen);
            for (int j = splitStrings.size() - 1; j >= 0; j--) {
                printStack.push(splitStrings[j]);
            }
        }
    }
    return printStack;
}

//CONSTRUCTION

Logger Logger::Builder::build() {
    return Logger{screen, doWordWrap, logExternally, logFile};
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

Logger::Logger(brain::lcd* screen, bool doWordWrap, bool logExternally, std::string logFile)
    : screen(screen), doWordWrap(doWordWrap), logExternally(logExternally), logFile(logFile)
{}

Logger::Logger(brain::lcd *screen, DesignatedBuilder config)
    : screen(screen), doWordWrap(config.doWordWrap), logExternally(config.logExternally), logFile(config.logFile)
{}

Logger::Logger(brain::lcd *screen)
    : screen(screen)
{}


Logger::Builder::Builder(brain::lcd* screen) {
    this->screen = screen;
}
