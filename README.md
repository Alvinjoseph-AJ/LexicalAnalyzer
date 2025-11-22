<h2>Lexical Analyzer</h2>

A lightweight Lexical Analyzer that scans a C source file and converts it into a sequence of tokens.
It detects keywords, identifiers, constants, operators, string/character literals, preprocessor directives, header files, and reports lexical inconsistencies.

<h3>Features:-</h3> 
<b>Token Classification</b>

The lexer identifies and categorizes:
<ul>
  <li>Reserved Keywords (int, float, struct, return, …)</li>
  <li>Non-reserved Keywords (if, else, for, while, switch, …)</li>
  <li>Identifiers</li>
  <li>Numeric Constants</li>
  <li>String Literals (with escape sequence handling)</li>
  <li>Character Constants</li>
  <li>Operators (+, -, *, /, %, =, <, >, !, |, &)</li>
  <li>Special Characters (; , { } ( ) [ ])</li>
  <li>Preprocessor Directives (#define, #ifdef, etc.)</li>
  <li>Header Files (#include <stdio.h> or "file.h")</li>
  <li>Unknown or malformed tokens</li>
</ul>
<h3>Comment & Directive Handling</h3>
<ul>
  <li>Single-line and multi-line comments are recognized as separate tokens</li>
  <li>Preprocessor directives are not skipped — they are tokenized as:</li>
  <ul>
  <li>PREPROCESSOR_DIRECTIVE</li>
  <li>HEADER_FILE (if matched)</li>
  </ul>
</ul>
<h3>Error Detection</h3>

Includes a comprehensive lexical validation system:

<h3>Bracket / Structure Checking</h3>

Detects:

<ul>
  <li>Unbalanced {}, (), []</li>
  <li>Extra closing brackets</li>
  <li>Mismatched bracket sequences</li>
</ul>

<h3>String / Character Literal Errors</h3>
<ul>
  <li>Unterminated quotes</li>
  <li>Empty character constants</li>
  <li>Invalid escape sequences</li>
  <li>Multi-character constants in single quotes</li>
</ul>

<h3>Semicolon Validation</h3>

Warns about possible missing semicolons, while intelligently skipping:

<ul>
  <li>Control statements (if, while, for, do, switch)</li>
  <li>Function definitions</li>
  <li>Blocks { ... }</li>
  <li>Enum declarations</li>
</ul>

<h3>Project Structure</h3>
├── main.c        # Driver code – reads file and prints tokens<br>
├── lexer.c       # Lexical analyzer implementation<br>
├── lexer.h       # Token structures and function declarations<br>
└── test.c        # Sample input file (optional)<br>

<h3>How It Works</h3>
<ul>
  <li>initializeLexer() opens the input file and loads the first character.</li>
  <li>getNextToken() scans the file and returns one token at a time.</li>
  <li>categorizeToken() classifies the token based on:</li>
  <ul>
    <li>Keywords</li>
    <li>Operators</li>
    <liIdentifiers></li>
    <li>Constants</li>
    <li>Directives</li>
  </ul>
  <li>After scanning, check_error() and related functions validate:</li>
  <ul>
    <li>Bracket balance</li>
    <li>Quote closure</li>
    <li>Escape sequences</li>
    <li>Semicolon rules</li>
  </ul>
</ul>

<h3>Sample Output</h3>
<img width="397" height="209" alt="image" src="https://github.com/user-attachments/assets/f71f0ccf-9155-47a7-9f37-b134f15982a4" />

<h3>Build & Run</h3>
<b>Compile</b><br>
gcc main.c lexer.c -o lexer

<b>Run</b><br>
./lexer lexer.txt

<h3>Limitations</h3>
<ul>
    <li>Multi-character operators (==, >=, &&, ||) not yet supported</li>
    <li>Comment tokens are recognized but not skipped</li>
    <li>Constant validation is basic and can be extended</li>
</ul>
<h3>Future Improvements</h3>
<ul>
    <li>Support multi-character operators</li>
    <li>Support floating-point constants more robustly</li>
    <li>Add symbol table generation</li>
    <li>Add DFA visualization for token states</li>
