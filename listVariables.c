/*
 * =====================================================================================
 *
 *       Filename:  listVariables.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  Friday 21 December 2018 09:44:25  IST
 *       Revision:  1.1
 *       Compiler:  gcc
 *
 *         Author:  Jagdish Prajapati (JP), prajapatijagdish22@gmail.com
 *        Company:  Emsoftronic
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static char *statments[10] = {0};
static char *variables[10] = {0};
static int count = 0;

char *ltrim(char **str)
{
    if ((! *str) || (**str == '\0')) return *str;
    while (**str) {
        if ((**str == ' ') || (**str == '\t') || (**str == '\r') || (**str == '\n')) (*str)++;
        else break;
    }

    return *str;
}

static const char *getComment(const char *current, char **outcomment, char **endptr)
{
    const char *comment, *end;
    do {
        comment = strchr(current, '/');
        if (comment ) {
            if ((*(comment+1) == '*') || (*(comment+1) == '/')) {
                if (*(comment+1) == '*') {
                    end = strstr(comment+2,"*/");
                    if (!end) *endptr = (char *)(comment+strlen(comment));
                    else *endptr = (char *)(end + 1);
                    break;
                }
                else {
                    end = strchr(comment+2,'\n');
                    if (!end) *endptr = (char *)(comment+strlen(comment));
                    else *endptr = (char *)end;
                    break;
                }
            }
            else current = comment+1;
        }
    }while (comment);
    *outcomment = (char *)comment;
    if (!comment) *endptr = NULL;
    return comment;
}

static const char *findValidKeyword(const char *current, const char *type)
{
    const char *cur, *found;
    char *end, *comment;

    cur = current;
    found = strstr(cur, type);
    getComment(cur, &comment, &end);
    while (comment) {
        if (comment && found) {
            if ((comment < found) && (end > found)) {
                cur = end+1;
                found = strstr(cur, type);
                getComment(cur, &comment, &end);
                continue;
            }
            else if (found < comment) {
                return found;
            }
            else if (found > end) {
                cur = end+1;
                getComment(cur, &comment, &end);
                continue;
            }
        }
        else return found;
    }

    return found;
}


static char *updateShaderVarInfo(char **strinout)
{
    char *found = *strinout;
    char *end = NULL;
    char statment[32] = {0};
    char variable[32] = {0};

    if (found) end = strpbrk(found, " \n\t\r");
    else return found;

    if (end) {
        strncpy(statment, found, end - found);
        strcat(statment, " ");
    }
    else { *strinout = NULL; return NULL;}

    found = ltrim(&end);

    if (found) end = strpbrk(found, " \n\t\r");
    if (end) {
        strncat(statment, found, end - found);
    }
    else { *strinout = NULL; return NULL;}

    found = ltrim(&end);
    while (end = strpbrk(found, "=,{[(; \n\t\r")) {
        memset(variable, 0, sizeof(variable));
        strncpy(variable, found, end - found);
        /*******************/
        statments[count] = malloc(strlen(statment) + strlen(variable) + 1);
        if (!statments[count]) {
            printf("Error: No memory!\n");
            break;
        }
        variables[count] = malloc(strlen(variable) + 1);
        if (!variables[count]) {
            printf("Error: No memory!\n");
            free(statments[count]);
            statments[count] = 0;
            break;
        }
        strcpy(statments[count], statment);
        strcpy(variables[count], variable);
        count++;
        /*******************/
        found = ltrim(&end);
        if ((*found == '[')) found = strpbrk(found, "]");
        if ((*found == ']')) found = strpbrk(found, "=,;");
        if ((*found == '=')) found = strpbrk(found, "({,;");
        if ((*found == '{')) found = strpbrk(found, "}");
        if ((*found == '(')) found = strpbrk(found, ")");
        if ((*found == '}') || (*found == ')')) found = strpbrk(found, ",;");
        if ((*found == ','))  {
            found++;
            ltrim(&found);
            continue;
        }
        if ((*found == ';')) break;
    }

    *strinout = found;
    return found;
}

void updatelist(const char *prog, const char *type)
{
    const char *current, *found;
    if (!prog && !type) return;
    current = prog;
    while (current && (*current)) {
        found = findValidKeyword(current, type);
        if (found) found = updateShaderVarInfo((char **)&found);
        current = found;
    }
}


const char *shadder =
"attribute vec4 gl_Color;\n"
"    varying vec4 gl_FrontColor; \n"
    "varying vec4 gl_BackColor; \n"

    "varying vec4 gl_Color; \n"
"attribute bvec3 status, color_state = 5, jagdish; //commented\n"
"attribute ivec2 hello=7, hi22, jagd = 5;\n"
" uniform mat4 scalmat;\n"
"uniform mat4 rotatmat;\n"
"void main()\n"
"{\n"
"    gl_FrontColor = gl_Color;\n"
"    gl_Position = ftransform();\n"
"}\n";


int main(int argc, char **argv)
{
    int i;
    updatelist(shadder, "attribute");
    updatelist(shadder, "uniform");
    for(i = 0; i < count; i++) {
        printf("%d.\t%s:\t%s\n", i+1, variables[i], statments[i]);
    }
    return 0;
}

