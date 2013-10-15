#include <cctype>

#include <algorithm>
#include <iostream>
#include <fstream>
#include <map>
#include <string>

struct ciless {
    bool operator() (const std::string& lhs, const std::string& rhs) const {
        return std::lexicographical_compare(
                lhs.begin(), lhs.end(),
                rhs.begin(), rhs.end(),
                [] (char x, char y) { return tolower(x) < tolower(y); });
    }
};

struct token {
    enum elements {
        error, eof, id, numeric, boolean, string, procedure, is, declare,
        constant, type, addop, mulop, not_, if_, then, while_, write, read,
        begin, end, loop, relop, semicolon, colon, assign, lparen, rparen
    };

    token () = default;

    token (elements elem, int lineno, char c)
        : element(elem)
        , line_number(lineno)
        , lexeme(1, c) { }

    token (elements elem, int lineno)
        : element(elem)
        , line_number(lineno) { }

    token (int lineno, char c)
        : element(error)
        , line_number(lineno)
        , lexeme(1, c) { }

    void classify_alpha () {
        auto it = s_keyword_to_token.find(lexeme);
        if (s_keyword_to_token.end() != it) {
            element = it->second;
        }
        else {
            element = id;
        }
    }

    elements element;
    int line_number;
    std::string lexeme;

private:
    static const std::map<std::string, token::elements, ciless> s_keyword_to_token;
    static const char* element_to_string (elements elem) {
        switch (elem) {
#define ENUMITEM(x) case x: return #x;
            ENUMITEM(error)
            ENUMITEM(eof)
            ENUMITEM(id)
            ENUMITEM(numeric)
            ENUMITEM(boolean)
            ENUMITEM(string)
            ENUMITEM(procedure)
            ENUMITEM(is)
            ENUMITEM(declare)
            ENUMITEM(constant)
            ENUMITEM(type)
            ENUMITEM(addop)
            ENUMITEM(mulop)
            ENUMITEM(not_)
            ENUMITEM(if_)
            ENUMITEM(then)
            ENUMITEM(while_)
            ENUMITEM(write)
            ENUMITEM(read)
            ENUMITEM(begin)
            ENUMITEM(end)
            ENUMITEM(loop)
            ENUMITEM(relop)
            ENUMITEM(semicolon)
            ENUMITEM(colon)
            ENUMITEM(assign)
            ENUMITEM(lparen)
            ENUMITEM(rparen)
#undef ENUMITEM
            default:
                return "(unknown token)";
        }
    }

    friend std::ostream& operator<< (std::ostream& output, const token& tok) {
        return output << "(line " << tok.line_number << "): "
                      << element_to_string(tok.element)
                      << " : " << tok.lexeme;
    }
};

const std::map<std::string, token::elements, ciless> token::s_keyword_to_token {
    { "true", boolean },
    { "false", boolean },
    { "procedure", procedure },
    { "is", is },
    { "declare", declare },
    { "constant", constant },
    { "integer", type },
    { "real", type },
    { "boolean", type },
    { "or", addop },
    { "mod", mulop },
    { "and", mulop },
    { "not", not_ },
    { "if", if_ },
    { "then", then },
    { "while", while_ },
    { "put", write },
    { "put_line", write },
    { "get", read },
    { "begin", begin },
    { "end", end },
    { "loop", loop }
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
        tok = token(token::eof, m_current_line);
        return input;
    }

    switch (c) {
        case '"':
            tok = token(token::string, m_current_line, c);
            return handle_quote(input, tok);
        case '+':
            tok = token(token::addop, m_current_line, c);
            return input;
        case '-':
            tok = token(m_current_line, c);
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
            tok = token(m_current_line, c);
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
        tok = token(m_current_line, c);
        return handle_alpha(input, tok);
    }
    else if (isdigit(c)) {
        tok = token(token::numeric, m_current_line, c);
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
        tok.element = token::error;
    }

    return input;
}

std::istream& scanner::handle_minus (std::istream& input, token& tok) {
    char c;
    if (input && '-' == input.peek()) {
        while (input.get(c) && '\n' != c) { }
        ++m_current_line;
        /* Recurse back into the main scanning operation. We only risk a stack
         * overflow if we read an insane amount of consecutive comments, and
         * this is, to me, more readable than setting a special "continue"
         * flag. */
        return (*this)(input, tok);
    }

    tok.element = token::addop;
    return input;
}

std::istream& scanner::handle_colon (std::istream& input, token& tok) {
    if (input && '=' == input.peek()) {
        tok.lexeme += input.get();
        tok.element = token::assign;
        return input;
    }

    tok.element = token::colon;
    return input;
}

std::istream& scanner::handle_alpha (std::istream& input, token& tok) {
    enum { terminated, unterminated } state = terminated;

    tok.line_number = m_current_line;

    while (input) {
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
                    tok.classify_alpha();
                    return input;
                }
                break;
            case unterminated:
                if (isalpha(input.peek()) || isdigit(input.peek())) {
                    state = terminated;
                    tok.lexeme += input.get();
                }
                else {
                    tok.element = token::error;
                    return input;
                }
                break;
        }
    }

    if (unterminated == state) {
        tok.element = token::error;
    }
    else {
        tok.classify_alpha();
    }
    return input;
}

std::istream& scanner::handle_digit (std::istream& input, token& tok) {
    enum {
        terminated_integer,
        terminated_real,
        unterminated
    } state = terminated_integer;

    while (input) {
        switch (state) {
            case terminated_integer:
                if ('.' == input.peek()) {
                    state = unterminated;
                    tok.lexeme += input.get();
                }
                else if (isdigit(input.peek())) {
                    tok.lexeme += input.get();
                }
                else {
                    return input;
                }
                break;
            case unterminated:
                if (isdigit(input.peek())) {
                    state = terminated_real;
                    tok.lexeme += input.get();
                }
                else {
                    tok.element = token::error;
                    return input;
                }
                break;
            case terminated_real:
                if (isdigit(input.peek())) {
                    tok.lexeme += input.get();
                }
                else {
                    return input;
                }
                break;
        }
    }

    if (unterminated == state) {
        tok.element = token::error;
    }
    return input;
}

int main (int argc, char** argv) {
    if (argc < 2) {
        printf("usage: %s <input file>\n", argv[0]);
        return 1;
    }

    token tok;
    scanner gettoken;
    std::ifstream input (argv[1]);

    while (gettoken(input, tok)) {
        std::cout << tok << '\n';
    }
    std::cout << tok << '\n';

    input.close();
}
