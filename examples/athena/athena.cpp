/**
 * @copyright 2021 YelloSoft
 */

#include <cstdlib>

#include <algorithm>
#include <iostream>
#include <random>

int main() {
    const std::string replicant{
        R"(
   ...    *    .   _  .
*  .  *     .   * (_)   *
  .      |*  ..   *   ..
   .  * \|  *  ___  . . *
*   \/   |/ \/{0,Q}     .
  _\_\   |  / /)  )* _/_ *
      \ \| /,--"-"---  ..
_-----`  |(,__,__/__/_ .
       \ ||      ..
        ||| .            *
        |||
        |||
  , -=-~' .-^- _
        )"
    };

    const std::vector<std::string> quotes{
        "Can the maker repair what he makes?",
        "Chew, if only you could see what I've seen with your eyes.",
        "Do you like our owl?",
        "Enhance.",
        "Is this to be an empathy test?",
        "It's artificial?",
        R"(More human than human" is our motto.)",
        "Must be expensive.",
        "You think I would be working in a place like this if I could afford a real snake?",
        "I'll take the drink.",
        "I’m not paying for that.",
        "Many is the night I dream of cheese.",
        "That's not a name. It's a serial number.",
        "What's the plan?",
        "Within cells interlinked."
    };

    std::vector<std::string> quote;

    std::sample(quotes.begin(), quotes.end(), std::back_inserter(quote), 1, std::mt19937{ std::random_device{}() });

    std::cout << replicant << "\n\n"
              << quote.front() << "\n\n";
    return EXIT_SUCCESS;
}
