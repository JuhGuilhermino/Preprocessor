#include <WhitespaceMinifier.h>

/**
 * Trims characters from the beginning of a string.
 *
 * @param s The input string.
 * @return A new string with leading characters removed.
 */

std::string WhitespaceMinifier::ltrim(const std::string& s) {
  std::string clone{ s };
  //clone.erase(0, clone.find_first_not_of(" "));
  clone.erase(0, clone.find_first_not_of(" \t"));
  return clone;
}

/**
 * Trims characters from the end of a string.
 *
 * @param s The input string.
 * @return A new string with trailing characters removed.
 */
std::string WhitespaceMinifier::rtrim(const std::string& s) {
  std::string clone{ s };
  //clone.erase(clone.find_last_not_of(" ") + 1);
  clone.erase(clone.find_last_not_of(" \t") + 1);
  return clone;
}

/**
 * Trims characters from the middle of a string.
 *
 * @param s The input string.
 * @return A new string with trailing characters removed.
 */
std::string WhitespaceMinifier::mtrim(const std::string& s){
    if(s.empty()){
        return s;
    }

    std::string clone{s};
    std::string result{clone[0]};

    for (int i{1}; i < clone.length(); ++i){ 

        if(clone[i] == ' ' || clone[i] == '\t'){
           if (clone[i - 1] == ' ' ){
            continue;
           }
        }

        result += clone[i];
    }

    return result;

}

/**
 * Applies ltrim and rtrim in sequence.
 *
 * @param s The input string.
 * @return A new string with leading and trailing characters removed.
 */
std::string WhitespaceMinifier::minify(const std::string& s) {
  std::string clone{ s };
  clone = ltrim(clone);
  clone = mtrim(clone);
  clone = rtrim(clone);

  return clone;
}
