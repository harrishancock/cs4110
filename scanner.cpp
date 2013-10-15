#include <cctype>

struct token {
    token (elements elem, int lineno, char c)
        : element(elem)
        , line_number(lineno)
        , lexeme(1, c) { }

    token (elements elem, int lineno)
        : element(elem)
        , line_number(lineno) { }

    enum elements {
        error, eof, id, numeric, boolean, string, procedure, is, declare,
        const_, type, addop, mulop, not_, if_, then, while_, write, read,
        begin, end, relop, semicolon, colon, assign, lparen, rparen
    } element;

    int line_number;
    std::string lexeme;
};

class scanner {
public:
    std::istream& operator() (std::istream& input, token& tok);
    void set_line (int i) { m_current_line = i; }

private:
    std::istream& handle_quote (std::istream& input, token& tok);
    std::istream& handle_minus (std::istream& input, token& tok);
    std::istream& handle_colon (std::istream& input, token& tok);
    std::istream& handle_alpha (std::istream& input, token& tok);
    std::istream& handle_digit (std::istream& input, token& tok);

    int m_current_line = 1;
};

std::istream& scanner::operator() (std::istream& input, token& tok) {
    char c;
    if (!input.get(c)) {
        tok = token(token::eof, m_current_line);
        return input;
    }

    while (input && isspace(c)) {
        if ('\n' == c) {
            ++m_current_line;
        }
        c = input.get();
    }

    if (!input) {
        return input;
    }

    switch (c) {
        case '"':
            return handle_quote(input, tok);
        case '+':
            tok = token(token::addop, m_current_line, c);
            return input;
        case '-':
            return handle_minus(input, tok);
        case '*':
            tok = token(token::mulop, m_current_line, c);
            return input;
        case '/':
            tok = token(token::mulop, m_current_line, c);
            return input;
        case '<':
            tok = token(token::relop, m_current_line, c);
            return input;
        case '>':
            tok = token(token::relop, m_current_line, c);
            return input;
        case '=':
            tok = token(token::relop, m_current_line, c);
            return input;
        case ';':
            tok = token(token::semicolon, m_current_line, c);
            return input;
        case ':':
            return handle_colon(input, tok);
        case '(':
            tok = token(token::lparen, m_current_line, c);
            return input;
        case ')':
            tok = token(token::rparen, m_current_line, c);
            return input;
        default:
            break;
    }

    if (isalpha(c)) {
        return handle_alpha(input, tok);
    }
    else if (isdigit(c)) {
        return handle_digit(input, tok);
    }
    else {
        tok = token(token::error, m_current_line);
    }

    return input;
}

std::istream& scanner::handle_quote (std::istream& input, token& tok) {
    enum { terminated, unterminated } state = unterminated;

    char c;
    tok = token(token::string, m_current_line, "\"");

    while (input) {
        switch (state) {
            case terminated:
                if ('"' == input.peek()) {
                    state = unterminated;
                    tok.lexeme += input.get();
                }
                else {
                    return input;
                }
                break;
            case unterminated:
                input.get(c);
                tok.lexeme += c;
                if ('"' == c) {
                    state = terminated;
                }
                break;
        }
    }

    if (unterminated == state) {
        tok = token(token::error);
    }

    return input;
}

std::istream& scanner::handle_minus (std::istream& input, token& tok) {
    if (input && '-' == input.peek()) {
        while (input.get(c) && '\n' != c) { }
        ++m_current_line;
        /* Recurse back into the main scanning operation. We only risk a stack
         * overflow if we read an insane amount of consecutive comments, and
         * this is, to me, more readable than setting a special "continue"
         * flag. */
        return (*this)(input, tok);
    }

    tok = token(token::addop, m_current_line, "-");
    return input;
}

std::istream& scanner::handle_colon (std::istream& input, token& tok) {
    if (input && '=' == input.peek()) {
        input.get();
        tok = token(token::assign, m_current_line, ":=");
        return input;
    }

    tok = token(token::colon, m_current_line, ":");
    return input;
}

std::istream& scanner::handle_alpha (std::istream& input, token& tok) {
    enum { terminated, unterminated } state = terminated;

    while (input && !sink) {
        switch (state) {
            case terminated:
                if (isalpha(input.peek()) || isdigit(input.peek())) {
                    tok.lexeme += input.get();
                }
                else if ('_' == input.peek()) {
                    state = unterminated;
                    tok.lexeme += input.get();
                }
                else {
                    /* return input */
                }
                break;
            case unterminated:
                if (isalpha(input.peek()) || isdigit(input.peek())) {
                    state = terminated;
                    tok.lexeme += input.get();
                }
                else {
                    /* return input */
                }
                break;
        }
    }
    /* handle error */
}

std::istream& scanner::handle_digit (std::istream& input, token& tok) {
}

int main (int argc, char** argv) {
    if (argc < 2) {
        usage();
        return 1;
    }

    token tok;
    scanner gettoken;
    std::ifstream input (argv[1]);

    while (gettoken(input, tok)) {
        std::cout << token << '\n';
    }
    std::cout << token << '\n';

    input.close();
}
