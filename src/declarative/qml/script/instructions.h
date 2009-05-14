#ifndef _INSTRUCTIONS_H_
#define _INSTRUCTIONS_H_

struct ScriptInstruction {
    enum {
        Load,       // fetch
        Fetch,       // fetch

        Add,         // NA
        Subtract,    // NA
        Multiply,    // NA
        Equals,      // NA
        And,         // NA

        Int,         // integer
        Bool,        // boolean
    } type;

    union {
        struct {
            int idx;
        } fetch;
        struct {
            int value;
        } integer;
        struct {
            bool value;
        } boolean;
    };
};

#endif // _INSTRUCTIONS_H_
