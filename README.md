<!-- TOC -->
# Logging Library for Vex V5 brain
<p>
  Default vex brain printing is awful. It doesn't automatically make a new line for you, 
  and it doesn't scroll your screen when you're at the bottom, 
  leading to incredibly weird printing at the bottom. This library is supposed to fix this problem.
</p>

<!-- TOC -->
## Usage
 - You need to create an instance of a logger to print
 - You can tack on methods like disableWordWrap to customize the logger
 - To finish the creation process, you need to call .build()
```c++
#include "logger.h"

auto LOGGER = Logger:create(&Brain.Screen)
    .disableWordWrap()
    .build();
```
 - If you run C++20 which introduces compatability between default member values and designated initializers, you can use a different builder
 - You do not to call .build() with these
```c++
#include "logger.h"

Logger LOGGER{&Brain.Screen, {
    .doWordWrap = false,
    .logFile = "mylog.csv"
}};

//Uses all default configurations
Logger default{&Brain.Screen};
```

### There's three methods you can use:
  - print: prints something WITHOUT a new line
  - println: prints something WITH a new line
  - clearScreen: clears the screen and resets cursor to top right
  - NOTE: Print functions mimic c's printf with print formatting

```c++
#include "logger.h"

int main() {
  Logger LOGGER{&Brain.Screen};
  //Table for exponents
  for (int i = 0; i < 50; i++) {
    LOGGER.clearScreen():
    LOGGER.println("%d^2: %d, ", i * i);
    LOGGER.print("%d^3: %d", i * i * i);
  }
}
```
