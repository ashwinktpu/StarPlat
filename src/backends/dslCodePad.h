#ifndef DSLCODEPAD_H
#define DSLCODEPAD_H

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <string>

class dslCodePad {
   private:
    FILE* outputFile;
    int indentation;
    int filePtr;
    char* buffer;
    char* fileBuffer;
    int column;

   public:
    dslCodePad() {
        indentation = 0;
        outputFile = stdout;
        filePtr = 0;
        buffer = new char[1024 * 2];
        fileBuffer = new char[1024 * 1024 * 1024];
        column = 0;
        // printf("%s","HELLO INSIDE DSLCODEPAD");
    }

    ~dslCodePad() {
        fflush(outputFile);
        // delete[] buffer;
        // delete[] fileBuffer;
    }

    void addIncludeToFile(const std::string& includeName, bool isCppLib) {  

        if (!isCppLib)
            push('"');
        else
            push('<');

        pushString(includeName);

        if (!isCppLib)
            push('"');
        else
            push('>');
        NewLine();
    }

    void addHeaderFile(const std::string& headerName, bool isCppLib) {
        pushString("#include ");
		addIncludeToFile(headerName, isCppLib);
    }

    void setOutputFile(FILE* f) {
        outputFile = f;
        filePtr = 0;
    }

    void insert_indent() {
        indentation++;
    }
    void decrease_indent() {
        indentation--;
    }

    void pushString(const char* str) {
        int length = strlen(str);
        for (int i = 0; i < length; i++) {
            push(str[i]);
        }
    }

    void pushString(const std::string& str) {
        int length = str.size();
        for (int i = 0; i < length; i++) {
            push(str[i]);
        }
    }

    void AddSpace() {
        push(' ');
    }

    void pushStringWithNewLine(const std::string& str) {
        pushString(str.c_str());
        NewLine();
    }

    void pushstr_newL(const char* str) {  // printf("%d HELLO FILEPTR VALUE",filePtr);
        pushString(str);
        NewLine();
    }
    void pushstr_space(const char* str) {
        pushString(str);
        space();
    }

    void pushUpper(const char* str) {
        int l = strlen(str);
        for (int i = 0; i < l; i++) {
            if (islower(str[i]))
                push(toupper(str[i]));
            else
                push(str[i]);
        }
    }

    void NewLine()  // TODO: make the name as newLine() to conform to standards.
    {
        push('\n');
    }
    void space() {
        push(' ');
    }

    void outputToFile() {
        buffer[column++] = '\0';
        int i = 0;
        char temporary_buf[1024 * 2];
        filePtr = filePtr + sprintf(&fileBuffer[filePtr], "%s", buffer);
        while (i < filePtr) {
            int pointer = 0;
            bool noChar = true;
            while (true) {
                char c = fileBuffer[i++];
                temporary_buf[pointer++] = c;
                if (!isspace(c))
                    noChar = false;
                if (c == '\n' || i == filePtr)
                    break;
            }

            if (noChar)
                fprintf(outputFile, "\n");
            else {
                fwrite(temporary_buf, pointer, 1, outputFile);
            }
        }

        column = 0;
        filePtr = 0;
    }
    void push(const char c) {
        buffer[column++] = c;
        /*  if(c=='\n')
           {
               filePtr+=sprintf(fileBuffer+filePtr,"%s",buffer);
               column=0;
           }
 */

        if (c == '\n')  // to be checked again
        {               // printf("INSIDE CCCCCCCCCC");
            if (buffer[0] == '}')
                indentation--;
            else if (buffer[0] == ')')
                indentation--;
            buffer[column++] = '\0';
            if (column != 1 || buffer[0] != '\n') {
                for (int i = 0; i < 2 * indentation; i++)
                    fileBuffer[filePtr++] = ' ';
            }

            filePtr += sprintf(&fileBuffer[filePtr], "%s", buffer);

            int i = 0;
            for (i = 0; i < column; i++) {
                if (buffer[i] == '}' || buffer[i] == ')')
                    continue;
                break;
            }
            int indentCount = 0;
            for (; i < column; i++) {
                if (buffer[i] == '{')
                    indentCount++;
                if (buffer[i] == '(')
                    indentCount++;
                if (buffer[i] == '}')
                    indentCount--;
                if (buffer[i] == ')')
                    indentCount--;
            }

            if (indentCount > 0)
                indentation++;
            else if (indentCount < 0)
                indentation--;

            column = 0;
        }
    }
};

#endif
