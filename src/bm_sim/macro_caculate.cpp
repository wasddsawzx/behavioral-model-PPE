#include<bm/bm_sim/macro_caculate.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <string>

namespace bm
{
    int is_in(char *s, char c)
{
    while (*s != '\0')
    {
        if (*s == c)
            return 1;
        else
            s++;
    }
    return 0;
}

int isOpsymbol(char ch)
{
    if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%' || ch == '(' || ch == ')' || ch == '^'
    || ch=='&' || ch=='|' || ch=='~' || ch=='<' || ch=='>')
        return 1;
    return 0;
}

int levelOp(char op) 
{
    if (op=='|')
        return 1;
    else if (op=='^')
        return 2;
    else if (op=='&')
        return 3;
    else if (op=='<' || op=='>')
        return 4;
    else if (op == '+' || op == '-')
        return 5;
    else if (op == '*' || op == '/' || op == '%')
        return 6;
    else if (op == '@' || op=='#' || op=='~')
        return 7;
    else if (op == '(')
        return -1;
    else
        return -3;
}

int calcValue(int od1, int od2, char tempop) //����Ӽ��˳������ָ࣬��
{
    switch (tempop)
    {
    case '+':
        return od1 + od2;
    case '-':
        return od1 - od2;
    case '*':
        return od1 * od2;
    case '/':
        return od1 / od2;
    case '%':
        return fmod(od1, od2);
    case '&':
        return od1 & od2;
    case '|':
        return od1 | od2;
    case '^':
        return od1 ^ od2;
    case '@':
        return -od1;
    case '#':
        return od1;
    case '~':
        return ~od1;
    case '<':
        return od1 << od2;
    case '>':
        return od1 >> od2;
    }
    return 0;
}


}

